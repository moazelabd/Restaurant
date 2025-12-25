#include "cook.h"



/*


cooks class


*/
// Constructor
Cook::Cook()
    : specialization(NORMAL), baseSpeed(0), currentSpeed(0), ordersBeforeBreak(0), breakDuration(0),
    status(AVAILABLE), handledNormalOrders(0), handledVeganOrders(0), handledVIPOrders(0),
    busyTime(0), idleTime(0), breakTime(0), injuryTime(0) {
}

// Destructor
Cook::~Cook() {}

// Setters
void Cook::setSpecialization(OrderType spec) { specialization = spec; }
void Cook::setBreakDuration(int duration) { breakDuration = duration; }
void Cook::setBaseSpeed(int speed) { baseSpeed = speed; }
void Cook::setCurrentSpeed(int speed) { currentSpeed = speed; }
void Cook::setOrdersBeforeBreak(int orders) { ordersBeforeBreak = orders; }
void Cook::setStatus(CookStatus s) { status = s; }
void Cook::setHandledNormalOrders(int n) { handledNormalOrders = n; }
void Cook::setHandledVeganOrders(int v) { handledVeganOrders = v; }
void Cook::setHandledVIPOrders(int vip) { handledVIPOrders = vip; }
void Cook::setBusyTime(int t) { busyTime = t; }
void Cook::setIdleTime(int t) { idleTime = t; }
void Cook::setBreakTime(int t) { breakTime = t; }
void Cook::setInjuryTime(int t) { injuryTime = t; }
void Cook::setNextAvailableTime(int t) { nextAvailableTime = t; }
void Cook::incrementOrdersSinceLastBreak() { ordersSinceLastBreak++; }
void Cook::setID(int id) { ID = id; }


// Getters
OrderType Cook::getSpecialization() const { return specialization; }
int Cook::getBreakDuration() const { return breakDuration; }
int Cook::getBaseSpeed() const { return baseSpeed; }
int Cook::getCurrentSpeed() const { return currentSpeed; }
int Cook::getOrdersBeforeBreak() const { return ordersBeforeBreak; }
CookStatus Cook::getStatus() const { return status; }
int Cook::getHandledNormalOrders() const { return handledNormalOrders; }
int Cook::getHandledVeganOrders() const { return handledVeganOrders; }
int Cook::getHandledVIPOrders() const { return handledVIPOrders; }
int Cook::getBusyTime() const { return busyTime; }
int Cook::getIdleTime() const { return idleTime; }
int Cook::getBreakTime() const { return breakTime; }
int Cook::getInjuryTime() const { return injuryTime; }
int Cook::getNextAvailableTime() const { return nextAvailableTime; }
int Cook::getOrdersSinceLastBreak() const {  return ordersSinceLastBreak; }
int Cook::getID() const { return ID; }



