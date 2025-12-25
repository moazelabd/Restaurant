#ifndef __GUI_H_
#define __GUI_H_

#include "CMUgraphicsLib/CMUgraphics.h"
#include "enums.h"
#include "order.h"
#include "cook.h"
#include <string>
using namespace std;

class GUI
{
    enum GUI_REGION {
        ORD_REG,   // waiting orders
        COOK_REG,  // cooks
        SRV_REG,   // in-service orders
        DONE_REG,  // finished orders
        REG_CNT
    };

    struct DrawingItem
    {
        int ID;
        GUI_REGION region;
        color clr;
    };

private:
    window* pWind;
    color DrawingColors[3];   // NORMAL, VEGAN, VIP

    static const int
        WindWidth = 1200, WindHeight = 650,
        StatusBarHeight = 150,
        MenuBarHeight = 0,
        MenuItemWidth = 80,

        DrawingAreaHeight = WindHeight - MenuBarHeight - StatusBarHeight,
        YHalfDrawingArea = MenuBarHeight + DrawingAreaHeight / 2,

        RestStartX = (int)(WindWidth * 0.45),
        RestEndX = (int)(WindWidth * 0.55),

        RestWidth = RestEndX - RestStartX,

        RestStartY = YHalfDrawingArea - RestWidth / 2,
        RestEndY = YHalfDrawingArea + RestWidth / 2,

        FontSize = 20,
        OrderWidth = 2 * FontSize,
        OrderHeight = FontSize,

        MaxHorizOrders = ((WindWidth - RestWidth) / 2) / (OrderWidth + 1),
        MaxVerticalOrders = (DrawingAreaHeight / 2) / (OrderHeight + 1),

        MaxRegionOrderCount = MaxHorizOrders * MaxVerticalOrders;

    static const int maxItemCnt = 250;
    DrawingItem* DrawingList[maxItemCnt];
    int DrawingItemsCount;

    void DrawSingleItem(const DrawingItem* pDitem, int RegionCount) const;
    void DrawAllItems();

    void DrawString(const int iX, const int iY, const string Text);
    void DrawRestArea() const;

    void ClearStatusBar() const;
    void ClearDrawingArea() const;

public:
    GUI();
    ~GUI();

    // Input
    void waitForClick() const;
    string GetString() const;

    // Output
    void PrintMessage(string msg) const;

    void UpdateInterface();
    void AddToDrawingList(Order* pOrd);
    void AddToDrawingList(Cook* pC);
    void ResetDrawingList();

    PROG_MODE getGUIMode() const;
};

#endif
