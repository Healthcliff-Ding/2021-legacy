#include <iostream>
#include "hyperelliptic.hpp"
constexpr auto Vertical = 1;
constexpr auto Horizontal = 2;


int main(){
    int N;
    std::cin >> a >> b >> m >> n >> N;
    a = abs(a), b = abs(b);
    
    selectPoint(N);
    generate(N, Vertical);
    generate(N, Horizontal);
    vertical.set_color(Graph_lib::Color::dark_magenta);
    horizontal.set_color(Graph_lib::Color::dark_magenta);

    Point tl(10, 10);
    Simple_window win(tl, 4 * a, 4 * b, "hyperelliptic");
    win.attach(vertical);
    win.attach(horizontal);
    win.wait_for_button();
}

