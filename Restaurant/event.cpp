#include "event.h"


/*


event class


*/

// Constructor
Event::Event(EventType t, int ts, Order* o, int id, double m)
    : type(t), TimeStep(ts), order(o), OrderID(id), ExtraMoney(m) {
}

// Destructor
Event::~Event() {}

// Setters
void Event::setType(EventType t) { type = t; }
void Event::setTimeStep(int ts) { TimeStep = ts; }
void Event::setOrder(Order* o) { order = o; }
void Event::setOrderID(int id) { OrderID = id; }
void Event::setExtraMoney(double m) { ExtraMoney = m; }

// Getters
EventType Event::getType() const { return type; }
int Event::getTimeStep() const { return TimeStep; }
Order* Event::getOrder() const { return order; }
int Event::getOrderID() const { return OrderID; }
double Event::getExtraMoney() const { return ExtraMoney; }

// Print event details
void Event::print() const {
    cout << "Event Type: " << (type == ARRIVAL ? "Arrival" : type == CANCELLATION ? "Cancellation" : "Promotion")
        << ", TimeStep: " << TimeStep
        << ", OrderID: " << OrderID
        << ", ExtraMoney: " << ExtraMoney
        << endl;
}