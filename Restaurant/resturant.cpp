#include "resturant.h"   // او "Restaurant.h" حسب اسم الفايل
#include "GUI.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

Restaurant::Restaurant()
    : AutoPromotionLimit(0), CurrentTimeStep(0),
    totalNormalOrders(0), totalVeganOrders(0),
    totalVIPOrders(0), autoPromotedCount(0),
    pGUI(nullptr), mode(MODE_SLNT)
{
}

void Restaurant::setGUI(GUI* g, PROG_MODE m) {
    pGUI = g;
    mode = m;
}

// بعدها حط: computeDeadline, LoadFile, SimpleSimulator, FullSimulator,
// getFastestAvailableCook, handleAutoPromotion, writeOutputFile
// بالظبط زي ما انت كاتبهم، من غير ما تعيد تعريف الكلاس.

void Restaurant::LoadFile(const std::string& filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "Warning: could not open file '" << filename << "'\n";
        return;
    }

    // Minimal placeholder: consume the file without parsing so program can run.
    std::string line;
    while (std::getline(fin, line)) {
        // ignore content for now
    }

    fin.close();
}

void Restaurant::FullSimulator()
{
    // Minimal placeholder implementation so linker errors are resolved.
    // Replace with the full simulation logic as needed.
    std::cout << "Starting Full Simulator (placeholder)...\n";
    // advance to a final state immediately for now
    std::cout << "Full Simulator finished.\n";
}
