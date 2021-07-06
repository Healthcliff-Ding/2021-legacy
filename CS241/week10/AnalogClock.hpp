// 先画一个表盘
// 再画一个指针，并且预留数据接口
// 最后把clock弄进来，每秒改数据
#include "Simple_window.h"
#include "Graph.h"
#include <cmath>
#include <vector>
#include <memory>
#include <iostream>
#include <string>
constexpr auto PI = 3.1415926;

using namespace Graph_lib;

inline double dtor(int degree)
{
	return PI * degree / 180;
}

struct Dot : public Shape
{
public:
	int r;
	Point p;
	Dot() {}
	Dot(int rr, Point pp) : r(rr), p(pp) // rr is radius
	{
		add(Point(p.x - r, p.y - r));
	}
	Dot(int rr, int xx, int yy) : r(rr)
	{
		p = Point(xx, yy);
		add(Point(p.x - r, p.y - r));
	}
	Dot(const Dot& other)
	{
		r = other.r;
		p = other.p;
		add(Point(p.x - r, p.y - r));
	}
	void draw_lines() const
	{
		fl_color(color().as_int());
		fl_arc(point(0).x, point(0).y, 2 * r, 2 * r, 0, 360);

		fl_color(fill_color().as_int());
		fl_pie(point(0).x, point(0).y, 2 * r, 2 * r, 0, 360);
	}
};
class Bar : public Shape
{
	int ll, ss;
	double theta;
public:
	Bar():ss(30) {}
	Bar(int l, double phi, int s = 30):ss(s), ll(l), theta(phi * PI / 180) {}
	Bar(const Bar& other)
	{
		ll = other.ll, ss = other.ss;
		theta = other.theta;
	}
	void set_angel(double phi)
	{
		theta = dtor(phi);
	}
	void draw_lines() const
	{
		Line ptr(Point(600 - ss * cos(theta), 400 - ss * sin(theta)), Point(600 + ll * cos(theta), 400 + ll * sin(theta)));
		ptr.set_color(this->color());
		ptr.draw();
	}
};

class Clock : public Shape
{
private:
	// this time the center is (600, 400)
	int r;
	vector<Dot> Dots;
	vector<Text*> Nums;
	int sec, minute, hour;
	shared_ptr<Bar> Sec, Min, Hour;
	void draw_dail() const
	{
		// first draw the outer dail
		fl_color(Color::black);
		fl_arc(600 - r, 400 - r, 2 * r, 2 * r, 0, 360);
		// second draw 12 Dots
		for (int i = 0; i < 12; ++i)
		{
			// Dots[i].draw_lines();
			Nums[i]->draw_lines();
		}
	}
	void draw_ptr() const
	{
		Hour->set_color(Color::black);
		Hour->draw_lines();
		Min->set_color(Color::blue);
		Min->draw_lines();
		Sec->set_color(Color::red);
		Sec->draw_lines();
	}
public:
	Clock(): r(200)
	{
		for (int i = 0; i < 12; i++)
		{
			double arc = dtor(30 * i);
			int xx = 600 + (r - 25) * cos(arc), yy = 400 + (r - 25) * sin(arc);
			Point tmp(xx, yy);
			Dots.push_back(Dot(5, tmp));
			Dots.back().set_color(Color::black);
			Dots.back().set_fill_color(Color::black);
			xx = 600 + (r - 25) * cos(arc), yy = 400 + (r - 25) * sin(arc);
			int qwq = (i + 3) % 12;
			qwq = (qwq == 0) ? 12 : qwq;
			Nums.push_back(new Text(tmp, to_string(qwq)));
		}
	}
	void set_time(int h, int m, int s)
	{
		sec = s, minute = m, hour = h;
		Sec = make_shared<Bar>(Bar(150, sec * 6 - 90));
		Min = make_shared<Bar>(Bar(120, minute * 6 + sec / 10 - 90));
		Hour = make_shared<Bar>(Bar(75, hour * 30 + minute / 2 - 90, 20));
	}
	void draw_lines() const
	{
		draw_dail();
		draw_ptr();
	}
	// debug functions
	void check() const
	{
		cout << "Dots.size() is " << Dots.size() << endl;
		for (int i = 0; i < Dots.size(); ++i)
		{
			cout << Dots[i].p.x << ' ' << Dots[i].p.y << endl;
		}
	}
};