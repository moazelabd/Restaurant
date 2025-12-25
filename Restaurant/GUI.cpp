#include "GUI.h"

GUI::GUI()
{
    DrawingItemsCount = 0;
    pWind = new window(WindWidth + 15, WindHeight, 0, 0);
    pWind->ChangeTitle("The Restaurant");

    // set color for each order type (NORMAL, VEGAN, VIP)
    DrawingColors[0] = RED;       // NORMAL
    DrawingColors[1] = DARKBLUE;  // VEGAN
    DrawingColors[2] = VIOLET;    // VIP

    ClearStatusBar();
    ClearDrawingArea();
    DrawRestArea();
}

GUI::~GUI()
{
    delete pWind;
}

// ================= INPUT =================

void GUI::waitForClick() const
{
    int x, y;
    pWind->WaitMouseClick(x, y);
}

string GUI::GetString() const
{
    string Label;
    char Key;
    while (1)
    {
        pWind->WaitKeyPress(Key);
        if (Key == 27)          // ESC
            return "";
        if (Key == 13)          // ENTER
            return Label;
        if ((Key == 8) && (Label.size() >= 1))   // BackSpace
            Label.resize(Label.size() - 1);
        else
            Label += Key;

        PrintMessage(Label);
    }
}

// ================= OUTPUT =================

void GUI::PrintMessage(string msg) const
{
    ClearStatusBar();

    pWind->SetPen(DARKRED);
    pWind->SetFont(18, BOLD, BY_NAME, "Arial");
    pWind->DrawString(10, WindHeight - (int)(StatusBarHeight / 1.5), msg);
}

void GUI::DrawString(const int iX, const int iY, const string Text)
{
    pWind->SetPen(DARKRED);
    pWind->SetFont(18, BOLD, BY_NAME, "Arial");
    pWind->DrawString(iX, iY, Text);
}

void GUI::ClearStatusBar() const
{
    pWind->SetPen(WHITE, 3);
    pWind->SetBrush(WHITE);
    pWind->DrawRectangle(0, WindHeight - StatusBarHeight, WindWidth, WindHeight);

    pWind->SetPen(BROWN, 3);
    pWind->DrawLine(0, WindHeight - StatusBarHeight, WindWidth, WindHeight - StatusBarHeight);
}

void GUI::ClearDrawingArea() const
{
    pWind->SetPen(KHAKI, 3);
    pWind->SetBrush(KHAKI);
    pWind->DrawRectangle(0, MenuBarHeight, WindWidth, WindHeight - StatusBarHeight);
}

void GUI::DrawRestArea() const
{
    int L = RestWidth / 2;

    // 1- brown square
    pWind->SetPen(BROWN);
    pWind->SetBrush(BROWN);
    pWind->DrawRectangle(RestStartX, RestStartY, RestEndX, RestEndY);

    // 2- brown cross lines
    pWind->SetPen(BROWN, 3);
    pWind->DrawLine(0, YHalfDrawingArea, WindWidth, YHalfDrawingArea);
    pWind->DrawLine(WindWidth / 2, MenuBarHeight, WindWidth / 2, WindHeight - StatusBarHeight);

    // 3- white cross inside the Rest
    pWind->SetPen(WHITE);
    pWind->DrawLine(WindWidth / 2, YHalfDrawingArea - RestWidth / 2,
        WindWidth / 2, YHalfDrawingArea + RestWidth / 2);
    pWind->DrawLine(WindWidth / 2 - RestWidth / 2, YHalfDrawingArea,
        WindWidth / 2 + RestWidth / 2, YHalfDrawingArea);

    // 5- region labels
    pWind->SetPen(WHITE);
    pWind->SetFont(20, BOLD, BY_NAME, "Arial");
    pWind->DrawString(RestStartX + (int)(0.1 * L), RestStartY + 5 * L / 12, "WAIT");
    pWind->DrawString(WindWidth / 2 + (int)(0.1 * L), RestStartY + 5 * L / 12, "COOK");
    pWind->DrawString(WindWidth / 2 + (int)(0.1 * L), YHalfDrawingArea + 5 * L / 12, "SRVG");
    pWind->DrawString(RestStartX + (int)(0.1 * L), YHalfDrawingArea + 5 * L / 12, "DONE");
}

// draw ONE item
void GUI::DrawSingleItem(const DrawingItem* pDitem, int RegionCount) const
{
    if (RegionCount > MaxRegionOrderCount)
        return;

    int DrawDistance = RegionCount;
    int YPos = 1;
    if (RegionCount >= MaxHorizOrders) {
        DrawDistance = (RegionCount - 1) % MaxHorizOrders + 1;
        YPos = (RegionCount - 1) / MaxHorizOrders + 1;
    }

    GUI_REGION Region = pDitem->region;

    int x, y, refX, refY;
    switch (Region)
    {
    case ORD_REG:
        refX = (WindWidth / 2 - RestWidth / 2);
        refY = YHalfDrawingArea - OrderHeight;
        x = refX - DrawDistance * OrderWidth - DrawDistance;
        y = refY - YPos * OrderHeight - YPos;
        break;
    case COOK_REG:
        refX = (WindWidth / 2 + RestWidth / 2);
        refY = YHalfDrawingArea - OrderHeight;
        x = refX + (DrawDistance - 1) * OrderWidth + DrawDistance;
        y = refY - YPos * OrderHeight - YPos;
        break;
    case SRV_REG:
        refX = (WindWidth / 2 + RestWidth / 2);
        refY = YHalfDrawingArea + OrderHeight;
        x = refX + (DrawDistance - 1) * OrderWidth + DrawDistance;
        y = refY + (YPos - 1) * OrderHeight + YPos;
        break;
    case DONE_REG:
        refX = (WindWidth / 2 - RestWidth / 2);
        refY = YHalfDrawingArea + OrderHeight;
        x = refX - DrawDistance * OrderWidth - DrawDistance;
        y = refY + (YPos - 1) * OrderHeight + YPos;
        break;
    default:
        return;
    }

    pWind->SetPen(pDitem->clr);
    pWind->SetFont(20, BOLD, MODERN);
    pWind->DrawInteger(x, y, pDitem->ID);
}

void GUI::DrawAllItems()
{
    int RegionsCounts[REG_CNT] = { 0 };

    for (int i = 0; i < DrawingItemsCount; i++)
    {
        DrawingItem* pDitem = DrawingList[i];
        RegionsCounts[pDitem->region]++;
        DrawSingleItem(pDitem, RegionsCounts[pDitem->region]);
    }
}

void GUI::UpdateInterface()
{
    ClearDrawingArea();
    DrawRestArea();
    DrawAllItems();
}

// add order to drawing list
void GUI::AddToDrawingList(Order* pOrd)
{
    if (!pOrd) return;

    DrawingItem* pDitem = new DrawingItem;
    pDitem->ID = pOrd->getID();

    OrderType t = pOrd->getType();      // NORMAL, VEGAN, VIP
    pDitem->clr = DrawingColors[(int)t];

    Orderstatus st = pOrd->getStatus();
    GUI_REGION reg = ORD_REG;

    switch (st)
    {
    case waiting:
        reg = ORD_REG;
        break;
    case in_service:
        reg = SRV_REG;
        break;
    case finished:
        reg = DONE_REG;
        break;
    default:
        reg = ORD_REG;
        break;
    }

    pDitem->region = reg;
    DrawingList[DrawingItemsCount++] = pDitem;
}

// add cook to drawing list
void GUI::AddToDrawingList(Cook* pC)
{
    if (!pC) return;

    DrawingItem* pDitem = new DrawingItem;
    pDitem->ID = pC->getID();

    OrderType t = pC->getSpecialization();
    pDitem->clr = DrawingColors[(int)t];
    pDitem->region = COOK_REG;

    DrawingList[DrawingItemsCount++] = pDitem;
}

void GUI::ResetDrawingList()
{
    for (int i = 0; i < DrawingItemsCount; i++)
        delete DrawingList[i];

    DrawingItemsCount = 0;
}

PROG_MODE GUI::getGUIMode() const
{
    PROG_MODE Mode;
    do
    {
        PrintMessage("Please select GUI mode: (1)Interactive, (2)StepByStep, (3)Silent...");
        string S = GetString();
        int choice = atoi(S.c_str());          // 1,2,3
        Mode = static_cast<PROG_MODE>(choice - 1);
    } while (Mode < MODE_INTR || Mode >= MODE_CNT);

    return Mode;
}
