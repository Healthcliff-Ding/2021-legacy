#include "righttriangle.hpp"

int main()
{
    using namespace Graph_lib;

    Point tl(10, 10);
    Simple_window win(tl, 1200, 800, "hexagon");    // make a simple window

    setColor();
    win.attach(r1);
    win.attach(r2);
    win.attach(r3);
    win.attach(r4);
    win.attach(r5);
    win.attach(r6);
    win.attach(r7);
    win.attach(r8);

    win.wait_for_button();
}
