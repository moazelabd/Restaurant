#ifndef EVENT_H
#define EVENT_H


#include "enums.h"
#include "order.h"

// EVENT CLASS
class Event {
private:
    EventType type;
    int TimeStep;
    Order* order; // only for Arrival
    int OrderID;  // only for Cancellation & Promotion
    double ExtraMoney; // only for Promotion

public:
    Event(EventType t, int ts, Order* o = nullptr, int id = -1, double m = 0.0);
    ~Event();
    //setters
    void setType(EventType t);
    void setTimeStep(int ts);
    void setOrder(Order* o);
    void setOrderID(int id);
    void setExtraMoney(double m);
    //getters
    EventType getType() const;
    int getTimeStep() const;
    Order* getOrder() const;
    int getOrderID() const;
    double getExtraMoney() const;
    //print evebt values
    void print() const;
};

#endif