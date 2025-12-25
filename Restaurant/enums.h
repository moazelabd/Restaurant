#ifndef RESTAURANT_ENUMS_H
#define RESTAURANT_ENUMS_H


#include <string>
#include "LinkedQueue.h"
#include "Linkedlist.h"

using namespace std;

// ENUMS
enum OrderType { NORMAL, VEGAN, VIP, FAMILY, EXPRESS};
enum Orderstatus { finished, in_service, waiting };
enum CookStatus { AVAILABLE, ON_BREAK, INJURED, BUSY };
enum EventType { ARRIVAL, CANCELLATION, PROMOTION };

// Program modes for the GUI and simulation
enum PROG_MODE
{
    MODE_INTR,   // interactive (mouse click each timestep)
    MODE_STEP,   // step-by-step (1 second between timesteps)
    MODE_SLNT,   // silent mode (no drawing)
    MODE_CNT
};

// Maximum possible counts used by the GUI
const int MaxPossibleOrdCnt = 200;   // adjust if you want
const int MaxPossibleMcCnt = 50;


#endif // RESTAURANT_ENUMS_H
