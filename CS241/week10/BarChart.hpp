#include "Simple_window.h"
#include "Graph.h"

using namespace Graph_lib;
constexpr auto width = 80, height = 10;

struct Bar : public Shape
{
	int num;
	double val;
	Bar() {}
	Bar(int n, double v) : num(n), val(v) {}
	// 搞颜色会出问题
	void draw_lines() const
	{
		Graph_lib::Rectangle rect(Point(20 + width * num, 500 - height * val), width, int(val * 10));
		rect.set_color(this->color());
		rect.set_fill_color(this->fill_color());
		rect.draw_lines();
	}
};
