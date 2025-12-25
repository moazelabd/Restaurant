#ifndef RESTURANT_H
#define RESTURANT_H

#include "cook.h"
#include "order.h"
#include "event.h"
#include <string>
#include "Linkedlist.h"
#include "LinkedQueue.h"
#include "priorty_queue.h"

//RESTURANT CLASS
class Restaurant {
private:
    

    // lists of cooks
    LinkedList<Cook*> normalCooks;
    LinkedList<Cook*> veganCooks;
    LinkedList<Cook*> vipCooks;
    //bouns
    LinkedList<Cook*> familyCooks;   // for FAMILY orders 
    LinkedList<Cook*> expressCooks;  // for EXPRESS orders

    // waiting orders
    LinkedQueue<Order*> normalOrders;
    LinkedQueue<Order*> veganOrders;
    PriorityQueue<Order*> vipOrders;
	//bouns
    LinkedQueue<Order*> familyOrders;  
    LinkedQueue<Order*> expressOrders; 

    // in_service and finished
    LinkedList<Order*> inServiceOrders;
    LinkedList<Order*> finishedOrders;

    // events
    LinkedQueue<Event*> events;

    int AutoPromotionLimit;
    int CurrentTimeStep;

public:
    Restaurant();
    void LoadFile(const string& filename);
    void SimpleSimulator();
	//full simulation
    void FullSimulator();
    //bouns
    void insertCookSortedBySpeed(LinkedList<Cook*>& list, Cook* c);
    Cook* getFastestAvailableCook(LinkedList<Cook*>& list, OrderType orderType);
    double computeDeadline(Order* o);

   void handleAutoPromotion();

};

#endif
