#ifndef  COOK_H
#define  COOK_H



#include "enums.h"

// COOK CLASS
class Cook {
private:
    OrderType specialization;
    int breakDuration;
    int baseSpeed;
    int currentSpeed;
    int ordersBeforeBreak;
    CookStatus status;
    int handledNormalOrders;
    int handledVeganOrders;
    int handledVIPOrders;
    int busyTime;
    int idleTime;
    int breakTime;
    int injuryTime;
    int nextAvailableTime;
    int ordersSinceLastBreak;
    int ID;
	int handledfamilyOrders;
	int handledexpressOrders;



public:
    Cook();
    ~Cook();
    //setters
    void setSpecialization(OrderType spec);
    void setBreakDuration(int duration);
    void setBaseSpeed(int speed);
    void setCurrentSpeed(int speed);
    void setOrdersBeforeBreak(int orders);
    void setStatus(CookStatus s);
    void setHandledNormalOrders(int n);
    void setHandledVeganOrders(int v);
    void setHandledVIPOrders(int vip);
	void setHandledfamilyOrders(int fam);
	void setHandledexpressOrders(int exp);
    void setBusyTime(int t);
    void setIdleTime(int t);
    void setBreakTime(int t);
    void setInjuryTime(int t);
	void setNextAvailableTime(int t);
	void incrementOrdersSinceLastBreak();
    void setID(int id);
    void applyFatigue();
    void restoreSpeed();
    void markInjured(int recoveryPeriod);
    void skipNextBreak();
    void resetOrdersSinceLastBreak();

    //getters
    OrderType getSpecialization() const;
    int getBreakDuration() const;
    int getBaseSpeed() const;
    int getCurrentSpeed() const;
    int getOrdersBeforeBreak() const;
    CookStatus getStatus() const;
    int getHandledNormalOrders() const;
    int getHandledVeganOrders() const;
    int getHandledVIPOrders() const;
	int gethandlefamilyOrders() const;
	int gethandleexpressOrders() const;
    int getBusyTime() const;
    int getIdleTime() const;
    int getBreakTime() const;
    int getInjuryTime() const;
	int getNextAvailableTime() const;
    int getID() const;
	int getOrdersSinceLastBreak() const;
};

#endif