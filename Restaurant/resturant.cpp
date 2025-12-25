#include "resturant.h"     // أو "Restaurant.h" حسب اسم الفايل عندك
#include "GUI.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
using namespace std;

// ======================= Constructor & basic helpers =======================

Restaurant::Restaurant()
    : AutoPromotionLimit(0), CurrentTimeStep(0),
    totalNormalOrders(0), totalVeganOrders(0),
    totalVIPOrders(0), autoPromotedCount(0),
    pGUI(nullptr), mode(MODE_SLNT)
{
}

void Restaurant::setGUI(GUI* g, PROG_MODE m) {
    pGUI = g;
    mode = m;
}

// Compute deadline for an order
double Restaurant::computeDeadline(Order* o) {
    double AT = o->getArraivalTime();
    int size = o->getOrderSize();
    double price = o->getOrderPrice();

    return AT + 0.5 * size + 0.01 * price;
}

// ======================= Load File =======================

void Restaurant::LoadFile(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    int normalID = 1, veganID = 1, vipID = 1;
    int numNormal, numVIP, numVegan;
    int speedNormal, speedVIP, speedVegan;
    int ordersBeforeBreak, breakNormal, breakVIP, breakVegan;

    // 1) read num of cooks for every type  -> N G V
    fin >> numNormal >> numVegan >> numVIP;

    // 2) read speeds  -> SN SG SV
    fin >> speedNormal >> speedVegan >> speedVIP;

    // 3) BO BN BG BV
    fin >> ordersBeforeBreak >> breakNormal >> breakVegan >> breakVIP;

    // 4) read auto promotion limit -> AutoP
    fin >> AutoPromotionLimit;

    // 5) num of events -> M
    int numEvents;
    fin >> numEvents;

    // -------- create cooks --------
    // Normal cooks
    for (int i = 0; i < numNormal; ++i) {
        Cook* c = new Cook();
        c->setID(normalID++);
        c->setSpecialization(NORMAL);
        c->setBaseSpeed(speedNormal);
        c->setCurrentSpeed(speedNormal);
        c->setOrdersBeforeBreak(ordersBeforeBreak);
        c->setBreakDuration(breakNormal);
        c->setNextAvailableTime(0);
        normalCooks.InsertEnd(c);
    }

    // Vegan cooks
    for (int i = 0; i < numVegan; ++i) {
        Cook* c = new Cook();
        c->setID(veganID++);
        c->setSpecialization(VEGAN);
        c->setBaseSpeed(speedVegan);
        c->setCurrentSpeed(speedVegan);
        c->setOrdersBeforeBreak(ordersBeforeBreak);
        c->setBreakDuration(breakVegan);
        c->setNextAvailableTime(0);
        veganCooks.InsertEnd(c);
    }

    // VIP cooks
    for (int i = 0; i < numVIP; ++i) {
        Cook* c = new Cook();
        c->setID(vipID++);
        c->setSpecialization(VIP);
        c->setBaseSpeed(speedVIP);
        c->setCurrentSpeed(speedVIP);
        c->setOrdersBeforeBreak(ordersBeforeBreak);
        c->setBreakDuration(breakVIP);
        c->setNextAvailableTime(0);
        vipCooks.InsertEnd(c);
    }

    // -------- read events --------
    for (int i = 0; i < numEvents; ++i) {
        string eventType;
        fin >> eventType;

        if (eventType == "R") {
            string orderType;
            int size, id, time;
            double price;
            fin >> orderType >> time >> id >> size >> price;

            Order* o = new Order();
            if (orderType == "N")      o->setOrderType(NORMAL);
            else if (orderType == "V") o->setOrderType(VIP);
            else if (orderType == "G") o->setOrderType(VEGAN);

            o->setOrderSize(size);
            o->setID(id);
            o->setArrivalTime(time);
            o->setOrderPrice(price);

            Event* e = new Event(ARRIVAL, time, o, id);
            events.enqueue(e);
        }
        else if (eventType == "X") {
            int time, id;
            fin >> time >> id;
            Event* e = new Event(CANCELLATION, time, nullptr, id);
            events.enqueue(e);
        }
        else if (eventType == "P") {
            int time, id;
            double extraMoney;
            fin >> time >> id >> extraMoney;
            Event* e = new Event(PROMOTION, time, nullptr, id, extraMoney);
            events.enqueue(e);
        }
    }

    fin.close();
    cout << "File loaded successfully.\n";
    cout << "Events loaded: " << events.GetCount() << endl;
}

// ======================= Simple Simulator =======================

void Restaurant::SimpleSimulator() {
    CurrentTimeStep = 0;

    while (
        !events.isEmpty() ||
        !normalOrders.isEmpty() ||
        !veganOrders.isEmpty() ||
        !vipOrders.isEmpty() ||
        !inServiceOrders.isEmpty())
    {
        if (CurrentTimeStep >= 1) {
            cout << "\nTime Step: " << CurrentTimeStep << endl;
        }

        // handle events at this timestep
        Event* e;
        while (events.peek(e) && e->getTimeStep() == CurrentTimeStep) {
            events.dequeue(e);

            if (e->getType() == ARRIVAL) {
                Order* o = e->getOrder();
                if (o->getType() == NORMAL) normalOrders.enqueue(o);
                else if (o->getType() == VEGAN) veganOrders.enqueue(o);
                else if (o->getType() == VIP)  vipOrders.enqueue(o);
            }
            else if (e->getType() == CANCELLATION) {
                int cancelID = e->getOrderID();
                LinkedQueue<Order*> tempQ;
                Order* tempO;
                while (normalOrders.dequeue(tempO)) {
                    if (tempO->getID() != cancelID) tempQ.enqueue(tempO);
                    else delete tempO;
                }
                // move items back to normalOrders
                while (tempQ.dequeue(tempO)) normalOrders.enqueue(tempO);
            }
        }

        // assign one from each queue to in-service
        Order* o1; Order* o2; Order* o3;
        if (normalOrders.dequeue(o1)) inServiceOrders.InsertEnd(o1);
        if (veganOrders.dequeue(o2))  inServiceOrders.InsertEnd(o2);
        if (vipOrders.dequeue(o3))    inServiceOrders.InsertEnd(o3);

        // every 5 timesteps, move one from in-service to finished
        if (CurrentTimeStep % 5 == 0) {
            if (!inServiceOrders.isEmpty()) {
                Order* finishedO;
                inServiceOrders.DeleteFirst(finishedO);
                finishedOrders.InsertEnd(finishedO);
            }
        }

        if (CurrentTimeStep >= 1) {
            if (normalOrders.GetCount() == 0 && veganOrders.GetCount() == 0 && vipOrders.GetCount() == 0) {
                cout << "no orders in waiting" << endl;
            }
            else {
                cout << "Normal Orders waiting: " << normalOrders.GetCount() << endl;
                cout << "Vegan Orders waiting: " << veganOrders.GetCount() << endl;
                cout << "VIP Orders waiting: " << vipOrders.GetCount() << endl;
            }
            cout << "In-Service Orders: " << inServiceOrders.GetCount() << endl;
            cout << "Finished Orders: " << finishedOrders.GetCount() << endl;
        }

        CurrentTimeStep++;
    }

    cout << "\nSimulation finished." << endl;
}

// ======================= Full Simulator (with GUI) =======================

void Restaurant::FullSimulator() {
    CurrentTimeStep = 0;

    cout << "Starting FullSimulator..." << endl;

    while (
        !events.isEmpty() ||
        !normalOrders.isEmpty() ||
        !veganOrders.isEmpty() ||
        !vipOrders.isEmpty() ||
        !inServiceOrders.isEmpty())
    {
        // -------- update cooks (busy -> available, idle time) --------
        auto refreshCooks = [this](LinkedList<Cook*>& list) {
            Node<Cook*>* cur = list.getHead();
            while (cur) {
                Cook* c = cur->getItem();
                if (c->getStatus() == BUSY && c->getNextAvailableTime() <= CurrentTimeStep) {
                    c->setStatus(AVAILABLE);
                }
                else if (c->getStatus() == AVAILABLE) {
                    c->setIdleTime(c->getIdleTime() + 1);
                }
                cur = cur->getNext();
            }
            };

        refreshCooks(normalCooks);
        refreshCooks(veganCooks);
        refreshCooks(vipCooks);

        // -------- handle events at this timestep --------
        Event* e;
        while (events.peek(e) && e->getTimeStep() == CurrentTimeStep) {
            events.dequeue(e);

            if (e->getType() == ARRIVAL) {
                Order* o = e->getOrder();
                o->setStatus(waiting);

                if (o->getType() == NORMAL) {
                    normalOrders.enqueue(o);
                    totalNormalOrders++;
                }
                else if (o->getType() == VEGAN) {
                    veganOrders.enqueue(o);
                    totalVeganOrders++;
                }
                else if (o->getType() == VIP) {
                    vipOrders.enqueue(o);
                    totalVIPOrders++;
                }
            }
            else if (e->getType() == CANCELLATION) {
                int cancelID = e->getOrderID();
                LinkedQueue<Order*> tempQ;
                Order* tempO;
                while (normalOrders.dequeue(tempO)) {
                    if (tempO->getID() != cancelID)
                        tempQ.enqueue(tempO);
                    else {
                        // totalNormalOrders-- ; // لو حابب تعدّل
                        delete tempO;
                    }
                }
                // move items back to normalOrders
                while (tempQ.dequeue(tempO)) normalOrders.enqueue(tempO);
            }
            else if (e->getType() == PROMOTION) {
                int pid = e->getOrderID();
                double extra = e->getExtraMoney();
                LinkedQueue<Order*> tempQ;
                Order* tempO;
                bool promoted = false;
                while (normalOrders.dequeue(tempO)) {
                    if (!promoted && tempO->getID() == pid) {
                        tempO->setOrderType(VIP);
                        tempO->setOrderPrice(tempO->getOrderPrice() + extra);
                        vipOrders.enqueue(tempO);
                        promoted = true;

                        totalNormalOrders--;
                        totalVIPOrders++;
                        autoPromotedCount++;
                    }
                    else {
                        tempQ.enqueue(tempO);
                    }
                }
                normalOrders = tempQ;
            }
        }

        // -------- update waiting time of all waiting orders --------
        {
            // normal
            LinkedQueue<Order*> tmpN;
            Order* o;
            while (normalOrders.dequeue(o)) {
                o->setWaitingTime(CurrentTimeStep - o->getArraivalTime());
                tmpN.enqueue(o);
            }
            while (tmpN.dequeue(o)) normalOrders.enqueue(o);

            // vegan
            LinkedQueue<Order*> tmpG;
            while (veganOrders.dequeue(o)) {
                o->setWaitingTime(CurrentTimeStep - o->getArraivalTime());
                tmpG.enqueue(o);
            }
            while (tmpG.dequeue(o)) veganOrders.enqueue(o);

            // VIP (priority queue)
            PriorityQueue<Order*> tmpV;
            Order* ov;
            while (vipOrders.dequeue(ov)) {
                ov->setWaitingTime(CurrentTimeStep - ov->getArraivalTime());
                tmpV.enqueue(ov);
            }
            while (tmpV.dequeue(ov)) vipOrders.enqueue(ov);
        }

        // -------- auto-promotion --------
        handleAutoPromotion();

        // helper to compute service time
        auto computeServiceTime = [](Order* o, Cook* c) -> int {
            int rem = o->getRemainingSize();
            int speed = c->getCurrentSpeed();
            if (speed <= 0) speed = 1;
            int st = rem / speed;
            if (rem % speed != 0) st++;
            return st;
            };

        // -------- VIP orders: VIP -> Normal -> Vegan cooks --------
        {
            Order* o;
            while (vipOrders.peek(o)) {
                Cook* cook = nullptr;

                cook = getFastestAvailableCook(vipCooks, VIP);
                if (!cook) cook = getFastestAvailableCook(normalCooks, VIP);
                if (!cook) cook = getFastestAvailableCook(veganCooks, VIP);

                if (!cook) break;

                vipOrders.dequeue(o);

                int st = computeServiceTime(o, cook);
                o->setServiceTime(st);
                o->setFinishTime(CurrentTimeStep + st);
                o->setDeadline(computeDeadline(o));
                o->setStatus(in_service);

                cook->setStatus(BUSY);
                cook->setNextAvailableTime(CurrentTimeStep + st);
                cook->setBusyTime(cook->getBusyTime() + st);
                cook->setHandledVIPOrders(cook->getHandledVIPOrders() + 1);

                inServiceOrders.InsertEnd(o);
            }
        }

        // -------- Vegan orders: Vegan cooks --------
        {
            Order* o;
            while (veganOrders.peek(o)) {
                Cook* cook = getFastestAvailableCook(veganCooks, VEGAN);
                if (!cook) break;

                veganOrders.dequeue(o);

                int st = computeServiceTime(o, cook);
                o->setServiceTime(st);
                o->setFinishTime(CurrentTimeStep + st);
                o->setDeadline(computeDeadline(o));
                o->setStatus(in_service);

                cook->setStatus(BUSY);
                cook->setNextAvailableTime(CurrentTimeStep + st);
                cook->setBusyTime(cook->getBusyTime() + st);
                cook->setHandledVeganOrders(cook->getHandledVeganOrders() + 1);

                inServiceOrders.InsertEnd(o);
            }
        }

        // -------- Normal orders: Normal -> VIP cooks --------
        {
            Order* o;
            while (normalOrders.peek(o)) {
                Cook* cook = nullptr;

                cook = getFastestAvailableCook(normalCooks, NORMAL);
                if (!cook) cook = getFastestAvailableCook(vipCooks, NORMAL);

                if (!cook) break;

                normalOrders.dequeue(o);

                int st = computeServiceTime(o, cook);
                o->setServiceTime(st);
                o->setFinishTime(CurrentTimeStep + st);
                o->setDeadline(computeDeadline(o));
                o->setStatus(in_service);

                cook->setStatus(BUSY);
                cook->setNextAvailableTime(CurrentTimeStep + st);
                cook->setBusyTime(cook->getBusyTime() + st);
                cook->setHandledNormalOrders(cook->getHandledNormalOrders() + 1);

                inServiceOrders.InsertEnd(o);
            }
        }

        // -------- move finished orders to finished list --------
        {
            LinkedList<Order*> remainingInService;
            Order* inO;
            while (inServiceOrders.DeleteFirst(inO)) {
                if (inO->getFinishTime() <= CurrentTimeStep) {
                    inO->setStatus(finished);
                    finishedOrders.InsertEnd(inO);
                }
                else {
                    remainingInService.InsertEnd(inO);
                }
            }
            inServiceOrders = remainingInService;
        }

        // -------- GUI drawing --------
        // DEBUG: print counts to help diagnose drawing issues
        cout << "DBG: counts - Normal=" << normalOrders.GetCount()
            << " Vegan=" << veganOrders.GetCount()
            << " VIP=" << vipOrders.GetCount()
            << " InService=" << inServiceOrders.GetCount()
            << " Finished=" << finishedOrders.GetCount() << "\n";

        // print first few in-service order IDs and statuses
        {
            Node<Order*>* curDbg = inServiceOrders.getHead();
            int shown = 0;
            while (curDbg && shown < 5) {
                Order* od = curDbg->getItem();
                cout << "DBG: InService Order ID=" << od->getID() << " Status=" << od->getStatus() << "\n";
                curDbg = curDbg->getNext();
                shown++;
            }
        }

        if (pGUI) {
            pGUI->ResetDrawingList();

            // cooks
            {
                Node<Cook*>* cur = normalCooks.getHead();
                while (cur) {
                    pGUI->AddToDrawingList(cur->getItem());
                    cur = cur->getNext();
                }
                cur = veganCooks.getHead();
                while (cur) {
                    pGUI->AddToDrawingList(cur->getItem());
                    cur = cur->getNext();
                }
                cur = vipCooks.getHead();
                while (cur) {
                    pGUI->AddToDrawingList(cur->getItem());
                    cur = cur->getNext();
                }
            }

            // waiting orders
            {
                LinkedQueue<Order*> tmpN;
                Order* o;
                // Normal
                while (normalOrders.dequeue(o)) {
                    pGUI->AddToDrawingList(o);
                    tmpN.enqueue(o);
                }
                normalOrders = tmpN;

                // Vegan
                LinkedQueue<Order*> tmpG;
                while (veganOrders.dequeue(o)) {
                    pGUI->AddToDrawingList(o);
                    tmpG.enqueue(o);
                }
                veganOrders = tmpG;

                // VIP
                PriorityQueue<Order*> tmpV;
                Order* ov;
                while (vipOrders.dequeue(ov)) {
                    pGUI->AddToDrawingList(ov);
                    tmpV.enqueue(ov);
                }
                vipOrders = tmpV;
            }

            // in-service + finished
            {
                Node<Order*>* curO = inServiceOrders.getHead();
                while (curO) {
                    pGUI->AddToDrawingList(curO->getItem());
                    curO = curO->getNext();
                }
                curO = finishedOrders.getHead();
                while (curO) {
                    pGUI->AddToDrawingList(curO->getItem());
                    curO = curO->getNext();
                }
            }

            pGUI->UpdateInterface();

            if (mode == MODE_INTR) {
                pGUI->PrintMessage("Time step: " + to_string(CurrentTimeStep) + "  (click to continue)");
                pGUI->waitForClick();
            }
            else if (mode == MODE_STEP) {
                pGUI->PrintMessage("Time step: " + to_string(CurrentTimeStep));
                Sleep(1000);
            }
            // MODE_SLNT: لا توقف
        }

        // -------- console report (اختياري) --------
        cout << "\nTime Step: " << CurrentTimeStep << endl;
        cout << "Waiting N/Veg/VIP = "
            << normalOrders.GetCount() << " / "
            << veganOrders.GetCount() << " / "
            << vipOrders.GetCount() << endl;
        cout << "In-Service: " << inServiceOrders.GetCount()
            << "  Finished: " << finishedOrders.GetCount() << endl;

        CurrentTimeStep++;
    }

    cout << "\nFull simulation finished." << endl;
    cout << "Totals: N=" << totalNormalOrders
        << " G=" << totalVeganOrders
        << " V=" << totalVIPOrders
        << " | AutoPromoted=" << autoPromotedCount << endl;

    writeOutputFile("output.txt");
}

// ======================= Helpers =======================

// لسه TODO لو محتاج ترتّب الليست بالسيرعة
void Restaurant::insertCookSortedBySpeed(LinkedList<Cook*>& list, Cook* c) {
    // تقدر تكملها بعدين لو عايز ترتيب معين
}

Cook* Restaurant::getFastestAvailableCook(LinkedList<Cook*>& list, OrderType orderType) {
    Node<Cook*>* cur = list.getHead();
    while (cur) {
        Cook* c = cur->getItem();
        if (c->getStatus() == AVAILABLE && c->getNextAvailableTime() <= CurrentTimeStep) {
            return c;
        }
        cur = cur->getNext();
    }
    return nullptr;
}

void Restaurant::handleAutoPromotion() {
    LinkedQueue<Order*> tmp;
    Order* o;
    while (normalOrders.dequeue(o)) {
        double wt = CurrentTimeStep - o->getArraivalTime();
        o->setWaitingTime(wt);
        if (wt >= AutoPromotionLimit) {
            o->setOrderType(VIP);
            vipOrders.enqueue(o);
            autoPromotedCount++;
            totalNormalOrders--;
            totalVIPOrders++;
        }
        else {
            tmp.enqueue(o);
        }
    }
    normalOrders = tmp;
}

// ======================= Output file =======================

void Restaurant::writeOutputFile(const string& outName) {
    ofstream fout(outName);
    if (!fout.is_open()) return;

    // collect finished orders into array
    const int MAX_ORDERS = 10000;
    Order* arr[MAX_ORDERS];
    int n = 0;

    Order* o;
    while (finishedOrders.DeleteFirst(o)) {
        if (n < MAX_ORDERS) {
            arr[n++] = o;
        }
    }

    // sort by FT then ST
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (arr[j]->getFinishTime() < arr[i]->getFinishTime() ||
                (arr[j]->getFinishTime() == arr[i]->getFinishTime() &&
                    arr[j]->getServiceTime() < arr[i]->getServiceTime())) {
                Order* tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }

    // print finished orders
    fout << "FT ID AT WT ST\n";
    double sumWT = 0, sumST = 0;
    for (int i = 0; i < n; ++i) {
        o = arr[i];
        fout << (int)o->getFinishTime() << " "
            << o->getID() << " "
            << (int)o->getArraivalTime() << " "
            << (int)o->getWaitingTime() << " "
            << (int)o->getServiceTime() << "\n";

        sumWT += o->getWaitingTime();
        sumST += o->getServiceTime();
    }

    double avgWT = (n > 0) ? sumWT / n : 0.0;
    double avgST = (n > 0) ? sumST / n : 0.0;

    fout << "\nOrders: "
        << (totalNormalOrders + totalVeganOrders + totalVIPOrders)
        << " [Normal: " << totalNormalOrders
        << ", Vegan: " << totalVeganOrders
        << ", VIP: " << totalVIPOrders << "]\n";

    fout << "Auto-promoted: " << autoPromotedCount << "\n";
    fout << "Avg Wait: " << avgWT << "\n";
    fout << "Avg Serv: " << avgST << "\n";

    auto printCookStats = [&fout](LinkedList<Cook*>& list, char typePrefix) {
        Node<Cook*>* cur = list.getHead();
        while (cur) {
            Cook* c = cur->getItem();
            int busy = c->getBusyTime();
            int idle = c->getIdleTime();
            int br = c->getBreakTime() + c->getInjuryTime();
            int denom = busy + idle + br;
            double util = (denom > 0) ? (100.0 * busy / denom) : 0.0;

            fout << "Cook " << typePrefix << c->getID()
                << ": Norm=" << c->getHandledNormalOrders()
                << ", Veg=" << c->getHandledVeganOrders()
                << ", VIP=" << c->getHandledVIPOrders()
                << ", Busy=" << busy
                << ", Idle=" << idle
                << ", Break/Injury=" << br
                << ", Util=" << util << "%\n";

            cur = cur->getNext();
        }
        };

    printCookStats(normalCooks, 'N');
    printCookStats(veganCooks, 'G');
    printCookStats(vipCooks, 'V');

    fout.close();
}
