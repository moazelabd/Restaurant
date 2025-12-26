#include <iostream>
#include "enums.h"
#include "GUI.h"
#include "resturant.h"
using namespace std;

int main() {
    Restaurant r;

    GUI* pGUI = new GUI;
    PROG_MODE m = pGUI->getGUIMode();   

    r.setGUI(pGUI, m);

    r.Run();   

    cout << "\nPress Enter to exit...";
    cin.get();

    delete pGUI;
    return 0;
}
