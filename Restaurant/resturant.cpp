#include "resturant.h"     
#include "GUI.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include "LinkedList.h"
#include <ctime>
using namespace std;

// ======================= Constructor & basic helpers =======================

Restaurant::Restaurant()
    : AutoPromotionLimit(0), CurrentTimeStep(0),
    totalNormalOrders(0), totalVeganOrders(0),
    totalVIPOrders(0), autoPromotedCount(0),
    pGUI(nullptr), mode(MODE_SLNT)
{
    srand((unsigned)time(nullptr));   
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

    // IDs for each cook type
    int normalID = 1, veganID = 1, vipID = 1, familyID = 1, expressID = 1;

    // numbers of cooks
    int numNormal, numVIP, numVegan, numFamily, numExpress;

    // speeds
    int speedNormal, speedVIP, speedVegan, speedFamily, speedExpress;

    // break info
    int ordersBeforeBreak;
    int breakNormal, breakVIP, breakVegan, breakFamily, breakExpress;

    //  read number of cooks for each type: N G V F E
    fin >> numNormal >> numVegan >> numVIP >> numFamily >> numExpress;

    //  read speeds: SN SG SV SF SE
    fin >> speedNormal >> speedVegan >> speedVIP >> speedFamily >> speedExpress;

    //  read break info: BO BN BG BV BF BE
    fin >> ordersBeforeBreak >> breakNormal >> breakVegan >> breakVIP >> breakFamily >> breakExpress;

    // read auto promotion limit
    fin >> AutoPromotionLimit;

    //  read number of events: M
    int numEvents;
    fin >> numEvents;

    // -------- create cooks (sorted by speed) --------

    // Normal cooks
    for (int i = 0; i < numNormal; ++i) {
        Cook* c = new Cook();
        c->setID(normalID++);
        c->setSpecialization(NORMAL);

        // Variable speed: each cook gets slightly different speed
        int cookSpeed = speedNormal + (i % 3) - 1;
        if (cookSpeed < 1) cookSpeed = 1;
        c->setBaseSpeed(cookSpeed);
        c->setCurrentSpeed(cookSpeed);

        c->setOrdersBeforeBreak(ordersBeforeBreak);

        // Variable break: each cook gets slightly different break duration
        int cookBreak = breakNormal + (i % 2);
        c->setBreakDuration(cookBreak);

        c->setNextAvailableTime(0);
        insertCookSortedBySpeed(normalCooks, c);
    }

    // Vegan cooks
    for (int i = 0; i < numVegan; ++i) {
        Cook* c = new Cook();
        c->setID(veganID++);
        c->setSpecialization(VEGAN);

        // Variable speed
        int cookSpeed = speedVegan + (i % 3) - 1;
        if (cookSpeed < 1) cookSpeed = 1;
        c->setBaseSpeed(cookSpeed);
        c->setCurrentSpeed(cookSpeed);

        c->setOrdersBeforeBreak(ordersBeforeBreak);

        // Variable break
        int cookBreak = breakVegan + (i % 2);
        c->setBreakDuration(cookBreak);

        c->setNextAvailableTime(0);
        insertCookSortedBySpeed(veganCooks, c);
    }

    // VIP cooks
    for (int i = 0; i < numVIP; ++i) {
        Cook* c = new Cook();
        c->setID(vipID++);
        c->setSpecialization(VIP);

        // Variable speed
        int cookSpeed = speedVIP + (i % 3) - 1;
        if (cookSpeed < 1) cookSpeed = 1;
        c->setBaseSpeed(cookSpeed);
        c->setCurrentSpeed(cookSpeed);

        c->setOrdersBeforeBreak(ordersBeforeBreak);

        // Variable break
        int cookBreak = breakVIP + (i % 2);
        c->setBreakDuration(cookBreak);

        c->setNextAvailableTime(0);
        insertCookSortedBySpeed(vipCooks, c);
    }

    // Family cooks 
    for (int i = 0; i < numFamily; ++i) {
        Cook* c = new Cook();
        c->setID(familyID++);
        c->setSpecialization(FAMILY);

        // Variable speed
        int cookSpeed = speedFamily + (i % 3) - 1;
        if (cookSpeed < 1) cookSpeed = 1;
        c->setBaseSpeed(cookSpeed);
        c->setCurrentSpeed(cookSpeed);

        c->setOrdersBeforeBreak(ordersBeforeBreak);

        // Variable break
        int cookBreak = breakFamily + (i % 2);
        c->setBreakDuration(cookBreak);

        c->setNextAvailableTime(0);
        insertCookSortedBySpeed(familyCooks, c);
    }

    // Express cooks 
    for (int i = 0; i < numExpress; ++i) {
        Cook* c = new Cook();
        c->setID(expressID++);
        c->setSpecialization(EXPRESS);

        // Variable speed
        int cookSpeed = speedExpress + (i % 3) - 1;
        if (cookSpeed < 1) cookSpeed = 1;
        c->setBaseSpeed(cookSpeed);
        c->setCurrentSpeed(cookSpeed);

        c->setOrdersBeforeBreak(ordersBeforeBreak);

        // Variable break
        int cookBreak = breakExpress + (i % 2);
        c->setBreakDuration(cookBreak);

        c->setNextAvailableTime(0);
        insertCookSortedBySpeed(expressCooks, c);
    }

    // -------- read events --------
    for (int i = 0; i < numEvents; ++i) {
        string eventType;
        fin >> eventType;

        if (eventType == "R") {
            // arrival event
            string orderType;
            int size, id, time;
            double price;
            fin >> orderType >> time >> id >> size >> price;

            Order* o = new Order();
            if (orderType == "N") o->setOrderType(NORMAL);
            else if (orderType == "V") o->setOrderType(VIP);
            else if (orderType == "G") o->setOrderType(VEGAN);
            else if (orderType == "F") o->setOrderType(FAMILY);
            else if (orderType == "E") o->setOrderType(EXPRESS);

            o->setOrderSize(size);
            o->setID(id);
            o->setArrivalTime(time);
            o->setOrderPrice(price);

            Event* e = new Event(ARRIVAL, time, o, id);
            events.enqueue(e);
        }
        else if (eventType == "X") {
            // cancellation event
            int time, id;
            fin >> time >> id;
            Event* e = new Event(CANCELLATION, time, nullptr, id);
            events.enqueue(e);
        }
        else if (eventType == "P") {
            // promotion event
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

/*void Restaurant::SimpleSimulator() {
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
*/
// ======================= Full Simulator +  GUI =======================

void Restaurant::FullSimulator() {
    CurrentTimeStep = 0;
   // int safetyMax = 100; // Stop after 100 steps no matter what

    while (
       // CurrentTimeStep < safetyMax &&
        (
            !events.isEmpty() ||
            !normalOrders.isEmpty() ||
            !veganOrders.isEmpty() ||
            !vipOrders.isEmpty() ||
            !familyOrders.isEmpty() ||
            !expressOrders.isEmpty() ||
            !inServiceOrders.isEmpty()
            )
        )
    {

        if (CurrentTimeStep>=10) {
            if (inServiceOrders.isEmpty() &&
                normalOrders.isEmpty() &&
                veganOrders.isEmpty() &&
                vipOrders.isEmpty() &&
                familyOrders.isEmpty() &&
                expressOrders.isEmpty()) {
                break;
            }
        }
            
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
        auto applyHealthEmergencies = [this](LinkedList<Cook*>& list) {
			const double injProb = 0.01;    //1% injury probability per timestep 
			const int recoveryPeriod = 10;   // fixed recovery period of 10 timesteps

            Node<Cook*>* cur = list.getHead();
            while (cur) {
                Cook* c = cur->getItem();

                if (c->getStatus() != INJURED && c->getStatus() != ON_BREAK &&
                    (c->getStatus() == AVAILABLE || c->getStatus() == BUSY)) {

                    double r = (double)rand() / RAND_MAX;
                    if (r <= injProb) {
                        c->markInjured(recoveryPeriod);
                        c->setNextAvailableTime(CurrentTimeStep + recoveryPeriod);
                      
                      
                    }
                }

                if (c->getStatus() == INJURED &&
                    c->getNextAvailableTime() <= CurrentTimeStep) {
                    c->setStatus(AVAILABLE);
                    c->restoreSpeed();   
                }

                cur = cur->getNext();
            }
            };

        refreshCooks(normalCooks);
        refreshCooks(veganCooks);
        refreshCooks(vipCooks);
        refreshCooks(familyCooks);      
        refreshCooks(expressCooks); 
        applyHealthEmergencies(normalCooks);
        applyHealthEmergencies(veganCooks);
        applyHealthEmergencies(vipCooks);
        applyHealthEmergencies(familyCooks);
        applyHealthEmergencies(expressCooks);


        // debug
        cout << "DBG TS=" << CurrentTimeStep
            << " Events=" << events.GetCount()  
            << " eventsEmpty=" << events.isEmpty() << endl;

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
                else if (o->getType() == FAMILY) {
                    familyOrders.enqueue(o);
                }
                else if (o->getType() == EXPRESS) {
                    expressOrders.enqueue(o);
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
                        delete tempO;
                    }
                }
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
			delete e;
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

            // VIP
            PriorityQueue<Order*> tmpV;
            Order* ov;
            while (vipOrders.dequeue(ov)) {
                ov->setWaitingTime(CurrentTimeStep - ov->getArraivalTime());
                tmpV.enqueue(ov);
            }
            while (tmpV.dequeue(ov)) vipOrders.enqueue(ov);

            // FAMILY
            LinkedQueue<Order*> tmpF;
            while (familyOrders.dequeue(o)) {
                o->setWaitingTime(CurrentTimeStep - o->getArraivalTime());
                tmpF.enqueue(o);
            }
            while (tmpF.dequeue(o)) familyOrders.enqueue(o);

            // EXPRESS
            LinkedQueue<Order*> tmpE;
            while (expressOrders.dequeue(o)) {
                o->setWaitingTime(CurrentTimeStep - o->getArraivalTime());
                tmpE.enqueue(o);
            }
            while (tmpE.dequeue(o)) expressOrders.enqueue(o);
        }

        // -------- auto promotion (only from NORMAL orders) --------
        handleAutoPromotion();

        // -------- progress in-service orders (decrease remaining size) --------
        {
            LinkedList<Order*> remainingInService;
            Order* inO;
            while (inServiceOrders.DeleteFirst(inO)) {
                if (inO->getStatus() == in_service && inO->getAssignedCook() != nullptr) {
                    Cook* c = inO->getAssignedCook();
                    int speed = c->getCurrentSpeed();
                    if (speed <= 0) speed = 1;
                    int rem = inO->getRemainingSize();
                    rem -= speed;
                    inO->setRemainingSize(rem);

                    if (rem <= 0) {
                        inO->setStatus(finished);
                        inO->setFinishTime(CurrentTimeStep);
                        finishedOrders.InsertEnd(inO);


                        c->applyFatigue();
                        c->incrementOrdersSinceLastBreak();

                        if (c->getOrdersSinceLastBreak() >= c->getOrdersBeforeBreak()) {

                            int vipWaiting = vipOrders.GetCount();
                            bool overloaded = (vipWaiting > 5);   

                            if (overloaded) {
                                // ====== OVERTIME  ======
                                c->applyFatigue();         
                                c->skipNextBreak();        
                                c->setStatus(AVAILABLE);   
                                c->setNextAvailableTime(CurrentTimeStep); 
                            }
                            else {
                                // ====== Break  ======
                                c->setStatus(ON_BREAK);
                                c->setNextAvailableTime(CurrentTimeStep + c->getBreakDuration());
                                c->resetOrdersSinceLastBreak();    
                            }
                        }
                        continue;
                    }

                }
                remainingInService.InsertEnd(inO);
            }
            inServiceOrders = remainingInService;
        }

        // helper to compute service time
        auto computeServiceTime = [](Order* o, Cook* c) -> int {
            int rem = o->getRemainingSize();
            int speed = c->getCurrentSpeed();
            if (speed <= 0) speed = 1;
            int st = rem / speed;
            if (rem % speed != 0) st++;
            return st;
            };

        // -------- VIP orders: VIP -> Normal -> Vegan cooks (with preemption) --------
        {
            Order* o;
            while (vipOrders.peek(o)) {
                Cook* cook = nullptr;

                cook = getFastestAvailableCook(vipCooks, VIP);
                if (!cook) cook = getFastestAvailableCook(normalCooks, VIP);
                if (!cook) cook = getFastestAvailableCook(veganCooks, VIP);

                // preempt a normal in-service order if still no cook
                if (!cook) {
                    Node<Order*>* cur = inServiceOrders.getHead();
                    Order* candidate = nullptr;
                    int bestRem = INT_MAX;
                    while (cur) {
                        Order* ord = cur->getItem();
                        if (ord->getType() == NORMAL && ord->getStatus() == in_service && ord->getAssignedCook()) {
                            int r = ord->getRemainingSize();
                            if (r < bestRem) {
                                bestRem = r;
                                candidate = ord;
                            }
                        }
                        cur = cur->getNext();
                    }
                    if (candidate) {
                        Cook* victimCook = candidate->getAssignedCook();
                        inServiceOrders.DeleteNode(candidate);
                        candidate->setAssignedCook(nullptr);
                        candidate->setStatus(waiting);
                        normalOrders.enqueue(candidate);
                        victimCook->setStatus(AVAILABLE);
                        cook = victimCook;
                    }
                }

                if (!cook) break;

                vipOrders.dequeue(o);

                if (o->getRemainingSize() <= 0) o->setRemainingSize(o->getOrderSize());
                int st = computeServiceTime(o, cook);
                o->setServiceTime(st);
                o->setFinishTime(CurrentTimeStep + st);
                o->setDeadline(computeDeadline(o));
                o->setStatus(in_service);
                o->setAssignedCook(cook);

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
                o->setAssignedCook(cook);

                cook->setStatus(BUSY);
                cook->setNextAvailableTime(CurrentTimeStep + st);
                cook->setBusyTime(cook->getBusyTime() + st);
                cook->setHandledVeganOrders(cook->getHandledVeganOrders() + 1);

                inServiceOrders.InsertEnd(o);
            }
        }

        // -------- Family orders: Family cooks -> Normal cooks --------
        {
            Order* o;
            while (familyOrders.peek(o)) {
                Cook* cook = nullptr;

                cook = getFastestAvailableCook(familyCooks, FAMILY);
                if (!cook) cook = getFastestAvailableCook(normalCooks, FAMILY);

                if (!cook) break;

                familyOrders.dequeue(o);

                if (o->getRemainingSize() <= 0) o->setRemainingSize(o->getOrderSize());
                int st = computeServiceTime(o, cook);
                o->setServiceTime(st);
                o->setFinishTime(CurrentTimeStep + st);
                o->setDeadline(computeDeadline(o));
                o->setStatus(in_service);
                o->setAssignedCook(cook);

                cook->setStatus(BUSY);
                cook->setNextAvailableTime(CurrentTimeStep + st);
                cook->setBusyTime(cook->getBusyTime() + st);
                cook->setHandledfamilyOrders(cook->gethandlefamilyOrders() + 1);
                inServiceOrders.InsertEnd(o);
            }
        }

        // -------- Express orders: Express cooks -> VIP cooks --------
        {
            Order* o;
            while (expressOrders.peek(o)) {
                Cook* cook = nullptr;

                cook = getFastestAvailableCook(expressCooks, EXPRESS);
                if (!cook) cook = getFastestAvailableCook(vipCooks, EXPRESS);

                if (!cook) break;

                expressOrders.dequeue(o);

                if (o->getRemainingSize() <= 0) o->setRemainingSize(o->getOrderSize());
                int st = computeServiceTime(o, cook);
                o->setServiceTime(st);
                o->setFinishTime(CurrentTimeStep + st);
                o->setDeadline(computeDeadline(o));
                o->setStatus(in_service);
                o->setAssignedCook(cook);

                cook->setStatus(BUSY);
                cook->setNextAvailableTime(CurrentTimeStep + st);
                cook->setBusyTime(cook->getBusyTime() + st);
                cook->setHandledexpressOrders(cook->gethandleexpressOrders() + 1);
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

                if (o->getRemainingSize() <= 0) o->setRemainingSize(o->getOrderSize());
                int st = computeServiceTime(o, cook);
                o->setServiceTime(st);
                o->setFinishTime(CurrentTimeStep + st);
                o->setDeadline(computeDeadline(o));
                o->setStatus(in_service);
                o->setAssignedCook(cook);

                cook->setStatus(BUSY);
                cook->setNextAvailableTime(CurrentTimeStep + st);
                cook->setBusyTime(cook->getBusyTime() + st);
                cook->setHandledNormalOrders(cook->getHandledNormalOrders() + 1);

                inServiceOrders.InsertEnd(o);
            }
        }

        // -------- move finished orders to finished list --------
//        {
//            LinkedList<Order*> remainingInService;
//            Order* inO;
//            while (inServiceOrders.DeleteFirst(inO)) {
//                if (inO->getFinishTime() <= CurrentTimeStep) {
//                    inO->setStatus(finished);
//                    finishedOrders.InsertEnd(inO);
//                }
//                else {
//                    remainingInService.InsertEnd(inO);
//                }
//            }
//            inServiceOrders = remainingInService;
//        }

        // -------- GUI drawing  --------
        cout << "DBG: counts - Normal=" << normalOrders.GetCount()
            << " Vegan=" << veganOrders.GetCount()
            << " VIP=" << vipOrders.GetCount()
            << " Family=" << familyOrders.GetCount()
            << " Express=" << expressOrders.GetCount()
            << " InService=" << inServiceOrders.GetCount()
            << " Finished=" << finishedOrders.GetCount() << "\n";
        if (pGUI) {
            pGUI->ResetDrawingList();

            // cooks
            {
                Node<Cook*>* cur = normalCooks.getHead();
                while (cur) { pGUI->AddToDrawingList(cur->getItem()); cur = cur->getNext(); }
                cur = veganCooks.getHead();
                while (cur) { pGUI->AddToDrawingList(cur->getItem()); cur = cur->getNext(); }
                cur = vipCooks.getHead();
                while (cur) { pGUI->AddToDrawingList(cur->getItem()); cur = cur->getNext(); }
                cur = familyCooks.getHead();
                while (cur) { pGUI->AddToDrawingList(cur->getItem()); cur = cur->getNext(); }
                cur = expressCooks.getHead();
                while (cur) { pGUI->AddToDrawingList(cur->getItem()); cur = cur->getNext(); }
            }

            // waiting orders
            {
                LinkedQueue<Order*> tmpN;
                Order* o;

                while (normalOrders.dequeue(o)) { pGUI->AddToDrawingList(o); tmpN.enqueue(o); }
                while (tmpN.dequeue(o)) normalOrders.enqueue(o);

                LinkedQueue<Order*> tmpG;
                while (veganOrders.dequeue(o)) { pGUI->AddToDrawingList(o); tmpG.enqueue(o); }
                while (tmpG.dequeue(o)) veganOrders.enqueue(o);

                PriorityQueue<Order*> tmpV;
                Order* ov;
                while (vipOrders.dequeue(ov)) { pGUI->AddToDrawingList(ov); tmpV.enqueue(ov); }
                while (tmpV.dequeue(ov)) vipOrders.enqueue(ov);

                LinkedQueue<Order*> tmpF;
                while (familyOrders.dequeue(o)) { pGUI->AddToDrawingList(o); tmpF.enqueue(o); }
                while (tmpF.dequeue(o)) familyOrders.enqueue(o);

                LinkedQueue<Order*> tmpE;
                while (expressOrders.dequeue(o)) { pGUI->AddToDrawingList(o); tmpE.enqueue(o); }
                while (tmpE.dequeue(o)) expressOrders.enqueue(o);
            }

            // in-service + finished
            {
                Node<Order*>* curO = inServiceOrders.getHead();
                while (curO) { pGUI->AddToDrawingList(curO->getItem()); curO = curO->getNext(); }
                curO = finishedOrders.getHead();
                while (curO) { pGUI->AddToDrawingList(curO->getItem()); curO = curO->getNext(); }
            }

            pGUI->UpdateInterface();


            // waiting counts
            int wN = normalOrders.GetCount();
            int wG = veganOrders.GetCount();
            int wV = vipOrders.GetCount();
            int wF = familyOrders.GetCount();
            int wE = expressOrders.GetCount();

            // available cooks
            auto countAvailable = [](LinkedList<Cook*>& lst) {
                int c = 0;
                Node<Cook*>* cur = lst.getHead();
                while (cur) {
                    if (cur->getItem()->getStatus() == AVAILABLE)
                        c++;
                    cur = cur->getNext();
                }
                return c;
                };

            int avN = countAvailable(normalCooks);
            int avG = countAvailable(veganCooks);
            int avV = countAvailable(vipCooks);
            int avF = countAvailable(familyCooks);
            int avE = countAvailable(expressCooks);

            // served so far
            int sN = 0, sG = 0, sV = 0, sF = 0, sE = 0;
            {
                Node<Order*>* cur = finishedOrders.getHead();
                while (cur) {
                    Order* o = cur->getItem();
                    switch (o->getType()) {
                    case NORMAL:  sN++; break;
                    case VEGAN:   sG++; break;
                    case VIP:     sV++; break;
                    case FAMILY:  sF++; break;
                    case EXPRESS: sE++; break;
                    }
                    cur = cur->getNext();
                }
            }

            string msg = "TS " + to_string(CurrentTimeStep);

            msg += " | Waiting N/G/V/F/E = " +
                to_string(wN) + "/" + to_string(wG) + "/" +
                to_string(wV) + "/" + to_string(wF) + "/" +
                to_string(wE);

            msg += " | Av Cooks N/G/V/F/E = " +
                to_string(avN) + "/" + to_string(avG) + "/" +
                to_string(avV) + "/" + to_string(avF) + "/" +
                to_string(avE);

            msg += " | Served N/G/V/F/E = " +
                to_string(sN) + "/" + to_string(sG) + "/" +
                to_string(sV) + "/" + to_string(sF) + "/" +
                to_string(sE);

            if (mode == MODE_INTR) {
                msg += "  (click to continue)";
                pGUI->PrintMessage(msg);
                pGUI->waitForClick();
            }
            else if (mode == MODE_STEP) {
                pGUI->PrintMessage(msg);
                Sleep(1000);
            }
        }

 
 
      

        CurrentTimeStep++;
    }

    cout << "\nFull simulation finished." << endl;
    cout << "Totals: N=" << totalNormalOrders
        << " G=" << totalVeganOrders
        << " V=" << totalVIPOrders
        << " | AutoPromoted=" << autoPromotedCount << endl;
    pGUI = new GUI();
    pGUI->ResetDrawingList();
	// write output file
    writeOutputFile("output.txt");
	// sucssesful end message
    string msg = "==========\n";
    msg += "    SIMULATION COMPLETED SUCCESSFULLY    \n";
    msg += "==========\n\n";
    msg += "Final Timestep: " + to_string(CurrentTimeStep) + "\n";
    msg += "Total Finished Orders: " + to_string(finishedOrders.GetCount()) + "\n\n";
    msg += "Normal: " + to_string(totalNormalOrders) + " | ";
    msg += "Vegan: " + to_string(totalVeganOrders) + " | ";
    msg += "VIP: " + to_string(totalVIPOrders) + "\n\n";
    msg += "Output file 'output.txt' .\n\n";

    pGUI->PrintMessage(msg);
    pGUI->UpdateInterface();
    pGUI->waitForClick();


   
}

void Restaurant::insertCookSortedBySpeed(LinkedList<Cook*>& list, Cook* c) {
    
    if (!list.getHead()) {
        list.InsertBeg(c);
        return;
    }

    Node<Cook*>* cur = list.getHead();
    Node<Cook*>* prev = nullptr;

   
    while (cur) {
        Cook* cc = cur->getItem();
        if (c->getBaseSpeed() > cc->getBaseSpeed()) {
            
            break;
        }
        prev = cur;
        cur = cur->getNext();
    }

    if (!prev) {
        
        list.InsertBeg(c);
    }
    else if (!cur) {
        
        list.InsertEnd(c);
    }
    else {

        list.InsertAfter(prev, c);
    }
}

Cook* Restaurant::getFastestAvailableCook(LinkedList<Cook*>& list, OrderType order) {
    Node<Cook*>* cur = list.getHead();
    while (cur) {
        Cook* c = cur->getItem();
        if (c->getStatus() == AVAILABLE &&
            c->getNextAvailableTime() <= CurrentTimeStep) {   
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

void Restaurant::writeOutputFile(const string& outName)
{
    ofstream fout(outName);
    if (!fout.is_open()) {
        cout << "Error: cannot open output file\n";
        return;
    }

    // ================= Collect Finished Orders =================
    const int MAX_ORDERS = 10000;
    Order* arr[MAX_ORDERS];
    int n = 0;

    LinkedList<Order*> tempFinished = finishedOrders;
    Order* o;

    while (tempFinished.DeleteFirst(o)) {
        if (n < MAX_ORDERS)
            arr[n++] = o;
    }

    // ================= Sort by Finish Time then Service Time =================
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[j]->getFinishTime() < arr[i]->getFinishTime() ||
                (arr[j]->getFinishTime() == arr[i]->getFinishTime() &&
                    arr[j]->getServiceTime() < arr[i]->getServiceTime()))
            {
                Order* tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }

    // ================= Statistics =================
    int finishedN = 0, finishedG = 0, finishedV = 0, finishedF = 0, finishedE = 0;
    int lateN = 0, lateG = 0, lateV = 0, lateF = 0, lateE = 0;

    double sumWT = 0, sumST = 0;

    fout << "FT ID AT WT ST\n";

    for (int i = 0; i < n; i++) {
        o = arr[i];

        fout << o->getFinishTime() << " "
            << o->getID() << " "
            << o->getArraivalTime() << " "
            << o->getWaitingTime() << " "
            << o->getServiceTime() << "\n";

        sumWT += o->getWaitingTime();
        sumST += o->getServiceTime();

        double FT = o->getFinishTime();
        double D = o->getDeadline();

        switch (o->getType()) {
        case NORMAL:   finishedN++; if (FT > D) lateN++; break;
        case VEGAN:    finishedG++; if (FT > D) lateG++; break;
        case VIP:      finishedV++; if (FT > D) lateV++; break;
        case FAMILY:   finishedF++; if (FT > D) lateF++; break;
        case EXPRESS:  finishedE++; if (FT > D) lateE++; break;
        }
    }

    double avgWT = (n > 0) ? sumWT / n : 0;
    double avgST = (n > 0) ? sumST / n : 0;

    // ================= Orders Summary =================
    fout << "\nOrders: " << n
        << " [Norm:" << finishedN
        << ", Veg:" << finishedG
        << ", VIP:" << finishedV
        << ", Fam:" << finishedF
        << ", Exp:" << finishedE << "]\n";

    // ================= Cooks Summary =================
    int nN = normalCooks.GetCount();
    int nG = veganCooks.GetCount();
    int nV = vipCooks.GetCount();
    int nF = familyCooks.GetCount();
    int nE = expressCooks.GetCount();

    fout << "Cooks: " << (nN + nG + nV + nF + nE)
        << " [Norm:" << nN
        << ", Veg:" << nG
        << ", VIP:" << nV
        << ", Fam:" << nF
        << ", Exp:" << nE << "]\n";

    // ================= Averages =================
    fout << "Avg Wait = " << avgWT
        << ", Avg Serv = " << avgST << "\n";

    // ================= Auto Promotion =================
    double autoPerc = (totalNormalOrders > 0)
        ? (100.0 * autoPromotedCount /
            (totalNormalOrders + autoPromotedCount))
        : 0;

    fout << "Auto-promoted: " << autoPromotedCount
        << " (" << autoPerc << "%)\n";

    // ================= Late Orders =================
    int totalLate = lateN + lateG + lateV + lateF + lateE;
    fout << "Late orders: " << totalLate
        << " [Norm:" << lateN
        << ", Veg:" << lateG
        << ", VIP:" << lateV
        << ", Fam:" << lateF
        << ", Exp:" << lateE << "]\n\n";

    // ================= Per Cook Statistics =================
    auto printCookStats = [&fout](LinkedList<Cook*>& list, char prefix)
        {
            Node<Cook*>* cur = list.getHead();
            while (cur) {
                Cook* c = cur->getItem();

                int busy = c->getBusyTime();
                int idle = c->getIdleTime();
                int rest = c->getBreakTime() + c->getInjuryTime();
                int total = busy + idle + rest;

                double util = (total > 0) ? (100.0 * busy / total) : 0;

                fout << "Cook " << prefix << c->getID()
                    << ": Orders [Norm:" << c->getHandledNormalOrders()
                    << ", Veg:" << c->getHandledVeganOrders()
                    << ", VIP:" << c->getHandledVIPOrders()
                    << ", Fam:" << c->gethandlefamilyOrders()
                    << ", Exp:" << c->gethandleexpressOrders()
                    << "], Busy:" << busy
                    << ", Idle:" << idle
                    << ", Break/Injury:" << rest << "\n";

                fout << "Utilization: " << util << "%\n";

                cur = cur->getNext();
            }
        };

    printCookStats(normalCooks, 'N');
    printCookStats(veganCooks, 'G');
    printCookStats(vipCooks, 'V');
    printCookStats(familyCooks, 'F');
    printCookStats(expressCooks, 'E');

    fout.close();
}



void Restaurant::Run() {
  
        pGUI->PrintMessage("Please enter input file name:");
        string fname = pGUI->GetString();      
        LoadFile(fname);
    
    FullSimulator();
}
