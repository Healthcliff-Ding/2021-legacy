#include "Simple_window.h"
#include "Graph.h"
#include "AnalogClock.hpp"
#include <time.h>
#include <Windows.h>

using namespace Graph_lib;

int main()
{
    Point tl(150, 100); 
    Simple_window win(tl, 1200, 800, "AnalogClock");
    Point ctl(600, 400);
    
    // draw a central dot
    Dot dot(5, ctl);
    dot.set_color(Color::red);
    dot.set_fill_color(Color::red);
    win.attach(dot);

    // draw the dail
    Clock clock;
    win.attach(clock);

    // add time controller
    __time64_t timep;

    // add the year info
    tm timeInfo;
    _time64(&timep);
    _localtime64_s(&timeInfo, &timep);
    string str;
    str += to_string(timeInfo.tm_year + 1900) + ' ';
    str += to_string(timeInfo.tm_mon + 1) + ' ';
    str += to_string(timeInfo.tm_mday);
    Text t(Point(675, 400), str);
    t.set_color(Color::black);
    win.attach(t);

    // main loop
    while (!win.pushed())
    {
        _time64(&timep);
        tm curTime;
        _localtime64_s(&curTime, &timep);
        clock.set_time(curTime.tm_hour, curTime.tm_min, curTime.tm_sec);
        win.show();
        Fl::wait();
        Sleep(1000);
        win.redraw();
    }
}
