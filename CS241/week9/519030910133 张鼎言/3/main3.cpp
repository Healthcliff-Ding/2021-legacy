#include "Simple_window.h"
#include "Graph.h"
#include "Binary_tree.hpp"
#include "TriangleBinary_tree.hpp"
#include <iostream>
using namespace Graph_lib;
constexpr int binaryTree = 1;
constexpr int triAngleBinaryTree = 2;

int main()
{
    int N, alt;
    /*
        When inputing, look at constexpr above!
        N is layer number
        alt ranges either 1 or 2
    */
    cin >> N >> alt;
    
    if (alt == binaryTree)
    {
        Point tl(10, 10); 
        Simple_window win(tl, 600, 400, "Binary_tree");
        Binary_tree poly(N);
        win.attach(poly);
        win.wait_for_button();
    }
    if (alt == triAngleBinaryTree)
    {
        Point tl(10, 10);
        Simple_window win(tl, 600, 400, "TriangleBinary_tree");
        TriangleBinary_tree poly(N);
        win.attach(poly);
        win.wait_for_button();
    }
}

