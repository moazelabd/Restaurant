#ifndef ORDER_H
#define ORDER_H


#include "enums.h"

// ORDER CLASS
class Order {
private:
    double ArraivalTime;
    OrderType type;
    int OrderSize;
    int remainingSize;
    double OrderPrice;
    int ID;
    double deadline; // D = AT + f(Size, Price)
    double waitingTime;
    double serviceTime;
    double finishTime;
    Orderstatus Status;
    class Cook* assignedCook;
public:
    Order();
    ~Order();
    //setters
    void setArrivalTime(double t);
    void setOrderType(OrderType t);
    void setOrderSize(int size);
    void setOrderPrice(double price);
    void setID(int id);
    void setWaitingTime(double wt);
    void setServiceTime(double st);
    void setFinishTime(double ft);
    void setStatus(Orderstatus s);
	void setDeadline(double d);
	void setRemainingSize(int size);
    //getters
    double getArraivalTime() const;
    OrderType getType() const;
    int getOrderSize() const;
    double getOrderPrice() const;
    int getID() const;
    double getWaitingTime() const;
    double getServiceTime() const;
    double getFinishTime() const;
    Orderstatus getStatus() const;
    double getDeadline() const;
	int getRemainingSize() const;

    // Print order data
    void print() const;
    void setAssignedCook(class Cook* c);
    class Cook* getAssignedCook() const;
};

#endif