#include "Simple_window.h"
#include "Graph.h"
#include "std_lib_facilities.h"

vector<double> x_axis, y_axis;
Graph_lib::Lines vertical, horizontal;

double a, b, m, n;
void selectPoint(int N)
{
    x_axis.resize(2 * N + 2);
    y_axis.resize(2 * N + 2);
    x_axis[0] = y_axis[N + 1] = - a;
    y_axis[0] = x_axis[N + 1] =  0;
    x_axis[N] = y_axis[2 * N + 2] =  a;
    y_axis[N] = x_axis[2 * N + 2] =  0;

    double stepSize = 2 * a / (N - 1);
    for (int i = 1; i < N; ++i)
    {
        x_axis[i] = x_axis[i - 1] + stepSize;
        y_axis[i] = abs(pow(1 - abs(pow(x_axis[i] / a, m)), 1 / n) * b);
    }
    stepSize = 2 * b / (N - 1);
    for (int i = N + 2; i < 2 * N + 2; ++i)
    {
        y_axis[i] = y_axis[i - 1] + stepSize;
        x_axis[i] = abs(pow(1 - abs(pow(y_axis[i] / b, n)), 1 / m) * a);
    }
}
void generate(int N, int phi)
{
    if (phi == 1)
    {
        for (int i = 0; i < N; ++i)
        {
            if (x_axis[i] != 0)
            {
                vertical.add(Point(x_axis[i] + 2 * a, y_axis[i] + 2 * b), Point(x_axis[i] + 2 * a, -y_axis[i] + 2 * b));
            }
            for (int j = i + 1; j <= N; ++j)
            {
                vertical.add(Point(x_axis[i] + 2 * a, y_axis[i] + 2 * b), Point(x_axis[j] + 2 * a, y_axis[j] + 2 * b));
                if (y_axis[j] != 0)
                {
                    vertical.add(Point(x_axis[i] + 2 * a, y_axis[i] + 2 * b), Point(x_axis[j] + 2 * a, -y_axis[j] + 2 * b));
                }
                if (y_axis[i] != 0)
                {
                    vertical.add(Point(x_axis[i] + 2 * a, -y_axis[i] + 2 * b), Point(x_axis[j] + 2 * a, y_axis[j] + 2 * b));
                    if (y_axis[j] != 0)
                    {
                        vertical.add(Point(x_axis[i] + 2 * a, -y_axis[i] + 2 * b), Point(x_axis[j] + 2 * a, -y_axis[j] + 2 * b));

                    }
                }
            }
        }
    }
    else 
    {
        for (int i = N + 1; i < 2 * N + 2; ++i) 
        {
            if (y_axis[i] != 0)
            {
                horizontal.add(Point(y_axis[i] + 2 * b, x_axis[i] + 2 * a), Point(y_axis[i] + 2 * b, -x_axis[i] + 2 * a));
            }
            for (int j = i + 1; j <= N; ++j)
            {
                horizontal.add(Point(y_axis[i] + 2 * b, x_axis[i] + 2 * a), Point(y_axis[j] + 2 * b, x_axis[j] + 2 * a));
                if (x_axis[j] != 0)
                {
                    horizontal.add(Point(y_axis[i] + 2 * b, x_axis[i] + 2 * a), Point(y_axis[j] + 2 * b, -x_axis[j] + 2 * a));
                }
                if (x_axis[i] != 0)
                {
                    horizontal.add(Point(y_axis[i] + 2 * b, -x_axis[i] + 2 * a), Point(y_axis[j] + 2 * b, x_axis[j] + 2 * a));
                    if (x_axis[j] != 0)
                    {
                        horizontal.add(Point(y_axis[i] + 2 * b, -x_axis[i] + 2 * a), Point(y_axis[j] + 2 * b, -x_axis[j] + 2 * a));
                    }                    
                }
            }
        }
    }
}