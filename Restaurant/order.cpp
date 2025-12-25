#include "order.h"


/*


order class


*/
// Constructor
Order::Order() : type(NORMAL), OrderSize(0), OrderPrice(0.0), ID(0),
ArraivalTime(0.0), waitingTime(0.0), serviceTime(0.0),
finishTime(0.0), Status(waiting) {
}


// Destructor
Order::~Order() {}

// Setters
void Order::setArrivalTime(double t) { ArraivalTime = t; }
void Order::setOrderType(OrderType t) { type = t; }
void Order::setOrderSize(int size) { OrderSize = size; }
void Order::setOrderPrice(double price) { OrderPrice = price; }
void Order::setID(int id) { ID = id; }
void Order::setWaitingTime(double wt) { waitingTime = wt; }
void Order::setServiceTime(double st) { serviceTime = st; }
void Order::setFinishTime(double ft) { finishTime = ft; }
void Order::setStatus(Orderstatus s) { Status = s; }
void Order::setDeadline(double d) { deadline = d; }

void Order::setRemainingSize(int size) { remainingSize = size; }

// Getters
double Order::getArraivalTime() const { return ArraivalTime; }
OrderType Order::getType() const { return type; }
int Order::getOrderSize() const { return OrderSize; }
double Order::getOrderPrice() const { return OrderPrice; }
int Order::getID() const { return ID; }
double Order::getWaitingTime() const { return waitingTime; }
double Order::getServiceTime() const { return serviceTime; }
double Order::getFinishTime() const { return finishTime; }
Orderstatus Order::getStatus() const { return Status; }
double Order::getDeadline() const { return deadline; }
int Order::getRemainingSize() const { return remainingSize; }

// Print order data
void Order::print() const {
    cout << "Type: " << (type == NORMAL ? "Normal" : type == VEGAN ? "Vegan" : "VIP")
        << ", Size: " << OrderSize
        << ", Price: " << OrderPrice
        << ", ID: " << ID
        << ", ArrivalTime: " << ArraivalTime
        << ", Waiting: " << waitingTime
        << ", Service: " << serviceTime
        << ", Finish: " << finishTime
        << ", Status: " << Status
        << endl;
}