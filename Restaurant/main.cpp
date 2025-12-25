#include "resturant.h"
#include <iostream>


int main() {
    Restaurant r;
    r.LoadFile("test.txt"); // file name
    r.SimpleSimulator();
   // r.FullSimulator();
    cout << "\nPress Enter to exit...";
    cin.get();
    return 0;
}
