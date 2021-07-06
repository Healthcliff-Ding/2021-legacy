#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Simple_window.h"
#include "Graph.h"
#include "BarChart.hpp"
using namespace Graph_lib;
constexpr int MONTH[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int main()
{
	// file read in
	ifstream inAustin("Austin.csv", ios::in);
	ifstream inNewYork("NewYork.csv", ios::in);
	string lineStr;
	vector<string> date1, date2;
	vector<string> temperature1, temperature2;
	getline(inAustin, lineStr);
	while (getline(inAustin, lineStr))
	{
		stringstream ss(lineStr);
		string str;
		getline(ss, str, ',');
		date1.push_back(str);
		getline(ss, str, ',');
		temperature1.push_back(str);
	}
	getline(inNewYork, lineStr);
	while (getline(inNewYork, lineStr))
	{
		stringstream ss(lineStr);
		string str;
		getline(ss, str, ',');
		date2.push_back(str);
		getline(ss, str, ',');
		temperature2.push_back(str);
	}
	// calculate average temperature
	int cnt = 0;
	vector<double> avrTem1, avrTem2;
	avrTem1.resize(12), avrTem2.resize(12);
	for (int mon = 0; mon < 12; ++mon)
	{
		double tot1 = 0, tot2 = 0;
		for (int day = 0; day < MONTH[mon]; ++day, ++cnt)
		{
			tot1 += stod(temperature1[cnt]);
			tot2 += stod(temperature2[cnt]);
		}
		avrTem1[mon] = tot1 / MONTH[mon];
		avrTem2[mon] = tot2 / MONTH[mon];
	}
	
	// draw coordinary axis
	Point tl(150, 100);
	Simple_window win(tl, 1200, 800, "BarChart");
	Point origin(100, 500);

	Axis xa(Axis::x, origin, width * 12, 12, "Month");
	xa.set_color(Color::black);
	Axis ya(Axis::y, origin, 400, 40, "Average Temperature");
	ya.set_color(Color::black);

	win.attach(xa);
	win.attach(ya);

	Text *xb[12], *yb[4];
	for (int i = 0; i < 12; ++i)
	{
		xb[i] = new Text(Point(140 + i * 80, 550), to_string(i + 1));
		xb[i]->set_color(Color::black);
		win.attach(*xb[i]);
	}
	for (int i = 0; i < 4; ++i)
	{
		yb[i] = new Text(Point(50, 500 - 100 * (i + 1)), to_string(10 * (i + 1)));
		yb[i]->set_color(Color::black);
		win.attach(*yb[i]);
	}
	
	// draw bar chart
	Bar *Austin[12], *NewYork[12];
	for (int i = 0; i < 12; ++i)
	{
		Austin[i] = new Bar(i + 1, avrTem1[i]);
		Austin[i]->set_color(Color::black);
		Austin[i]->set_fill_color(Color::dark_red);
		NewYork[i] = new Bar(i + 1, avrTem2[i]);
		NewYork[i]->set_color(Color::black);
		NewYork[i]->set_fill_color(Color::red);

		win.attach(*Austin[i]);
		win.attach(*NewYork[i]);
	}

	win.wait_for_button();
}
