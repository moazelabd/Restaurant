#include "resturant.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


/*


resturant class


*/


// Constructor
Restaurant::Restaurant() : AutoPromotionLimit(0), CurrentTimeStep(0) {}

// Compute deadline for an order
double Restaurant::computeDeadline(Order* o) {
    double AT = o->getArraivalTime();
    int size = o->getOrderSize();
    double price = o->getOrderPrice();

    return AT + 0.5 * size + 0.01 * price;
}




// LoadFile 
void Restaurant::LoadFile(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }
    int normalID = 1, veganID = 1, vipID = 1;
    int numNormal, numVIP, numVegan;
    int speedNormal, speedVIP, speedVegan;
    int breakDuration ,breakNormal, breakVIP, breakVegan;
    // ordersBeforeBreak is optional in some input formats; default to 0
    int ordersBeforeBreak = 0;

    //  read num of cooks for every type
    fin >> numNormal >> numVegan >> numVIP;

    // read speeds 
    fin >> speedNormal >>   speedVegan >> speedVIP;

    //  break 
    fin >>breakDuration >> breakNormal >> breakVegan  >> breakVIP;

    // read auto promotion limit
    fin >> AutoPromotionLimit;



    // num of events
    int numEvents;
    fin >> numEvents;



    // creating cooks
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
 

    // reading event
    for (int i = 0; i < numEvents; ++i) {
        string eventType;
        fin >> eventType;
        if (eventType == "R") {
            string orderType;
            int size, id, time;
            double price;
            fin >> orderType >> time   >> id >>size >> price;
            Order* o = new Order();
            if (orderType == "N") o->setOrderType(NORMAL);
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

// SimpleSimulator Implementation
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
        Event* e;
        while (events.peek(e) && e->getTimeStep() == CurrentTimeStep) {
            events.dequeue(e);

            if (e->getType() == ARRIVAL) {
                Order* o = e->getOrder();
                if (o->getType() == NORMAL) normalOrders.enqueue(o);
                else if (o->getType() == VEGAN) veganOrders.enqueue(o);
                else if (o->getType() == VIP) vipOrders.enqueue(o);
            }
            else if (e->getType() == CANCELLATION) {
                int cancelID = e->getOrderID();
                LinkedQueue<Order*> tempQ;
                Order* tempO;
                while (normalOrders.dequeue(tempO)) {
                    if (tempO->getID() != cancelID) tempQ.enqueue(tempO);
                    else delete tempO;
                }
                normalOrders = tempQ;
            }
        }

        Order* o1; Order* o2; Order* o3;
        if (normalOrders.dequeue(o1)) inServiceOrders.InsertEnd(o1);
        if (veganOrders.dequeue(o2)) inServiceOrders.InsertEnd(o2);
        if (vipOrders.dequeue(o3)) inServiceOrders.InsertEnd(o3);

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
            } else {
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


        Event* e;
        while (events.peek(e) && e->getTimeStep() == CurrentTimeStep) {
            events.dequeue(e);

            if (e->getType() == ARRIVAL) {
                Order* o = e->getOrder();
                o->setStatus(waiting);
                if (o->getType() == NORMAL) normalOrders.enqueue(o);
                else if (o->getType() == VEGAN) veganOrders.enqueue(o);
                else if (o->getType() == VIP) vipOrders.enqueue(o);
            }
            else if (e->getType() == CANCELLATION) {
                int cancelID = e->getOrderID();
                LinkedQueue<Order*> tempQ;
                Order* tempO;
                while (normalOrders.dequeue(tempO)) {
                    if (tempO->getID() != cancelID)
                        tempQ.enqueue(tempO);
                    else
                        delete tempO;
                }
                normalOrders = tempQ;
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
                    }
                    else {
                        tempQ.enqueue(tempO);
                    }
                }
                normalOrders = tempQ;
            }
        }


        {
            // Normal
            LinkedQueue<Order*> tmpN;
            Order* o;
            while (normalOrders.dequeue(o)) {
                o->setWaitingTime(CurrentTimeStep - o->getArraivalTime());
                tmpN.enqueue(o);
            }
            normalOrders = tmpN;

            // Vegan
            LinkedQueue<Order*> tmpG;
            while (veganOrders.dequeue(o)) {
                o->setWaitingTime(CurrentTimeStep - o->getArraivalTime());
                tmpG.enqueue(o);
            }
            veganOrders = tmpG;

            // VIP (priority queue)
            PriorityQueue<Order*> tmpV;
            Order* ov;
            while (vipOrders.dequeue(ov)) {
                ov->setWaitingTime(CurrentTimeStep - ov->getArraivalTime());
                tmpV.enqueue(ov);
            }
            vipOrders = tmpV;
        }


        auto computeServiceTime = [](Order* o, Cook* c) -> int {
            int rem = o->getRemainingSize();
            int speed = c->getCurrentSpeed();
            if (speed <= 0) speed = 1;
            int st = rem / speed;
            if (rem % speed != 0) st++;
            return st;
            };

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
}



// insert cook into list maintaining descending order of baseSpeed
void Restaurant::insertCookSortedBySpeed(LinkedList<Cook*>& list, Cook* c) {
    // TODO: traverse list and insert c before the first cook with lower speed
    // use list.InsertBeg / InsertEnd / manual Node traversal (you may extend LinkedList if needed)
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

        }
        else {
            tmp.enqueue(o);
        }
    }
    normalOrders = tmp;
}

