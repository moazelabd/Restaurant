#ifndef RESTURANT_H
#define RESTURANT_H

#include <string>
#include "enums.h"
#include "cook.h"
#include "order.h"
#include "event.h"
#include "LinkedList.h"
#include "LinkedQueue.h"
#include "priorty_queue.h"

class GUI;   // forward declaration

class Restaurant {
private:
    // cooks lists
    LinkedList<Cook*> normalCooks;
    LinkedList<Cook*> veganCooks;
    LinkedList<Cook*> vipCooks;
    LinkedList<Cook*> familyCooks;
    LinkedList<Cook*> expressCooks;

    // waiting orders
    LinkedQueue<Order*> normalOrders;
    LinkedQueue<Order*> veganOrders;
	PriorityQueue<Order*> vipOrders;//priority queue for vip orders
    LinkedQueue<Order*> familyOrders;
    LinkedQueue<Order*> expressOrders;

    // in-service and finished
    LinkedList<Order*> inServiceOrders;
    LinkedList<Order*> finishedOrders;

    // events
    LinkedQueue<Event*> events;

    int AutoPromotionLimit;
    int CurrentTimeStep;

    int totalNormalOrders;
    int totalVeganOrders;
    int totalVIPOrders;
    int autoPromotedCount;

    GUI* pGUI;
    PROG_MODE mode;

public:
    Restaurant();
    void setGUI(GUI* g, PROG_MODE m);
    void LoadFile(const std::string& filename);
    void SimpleSimulator();
    void FullSimulator();

    void insertCookSortedBySpeed(LinkedList<Cook*>& list, Cook* c);
    Cook* getFastestAvailableCook(LinkedList<Cook*>& list, OrderType orderType);
    double computeDeadline(Order* o);
    void handleAutoPromotion();
    void writeOutputFile(const std::string& outName);
    void Run();
};

#endif
