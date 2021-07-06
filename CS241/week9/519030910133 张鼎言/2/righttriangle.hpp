#include "Simple_window.h"    // get access to our window library
#include "Graph.h"            // get access to our graphics library facilities
#include "std_lib_facilities.h"
using namespace Graph_lib;

const double a = 200;
const double c = 200 * sqrt(2);
class righttriangle : public Shape
{
public:
	righttriangle(int a, int b, int x, int y, int posi)
	{
		// a,b is the length oh edge
		// x,y line righttangle's coordinary
		// posi is righttrangle's posture
		x += 600, y += 400;
		int c = sqrt(a * a + b * b);
		// define the up-most one '1', and the lowest one '5'
		switch (posi)
		{
		case 1:
			add(Point(x, y));
			add(Point(x - 0.5 * c, y + 0.5 * c));
			add(Point(x + 0.5 * c, y + 0.5 * c));
			break;
		case 2:
			add(Point(x, y));
			add(Point(x - a, y));
			add(Point(x, y + b));
			break;
		case 3:
			add(Point(x, y));
			add(Point(x - 0.5 * c, y + 0.5 * c));
			add(Point(x - 0.5 * c, y - 0.5 * c));
			break;
		case 4:
			add(Point(x, y));
			add(Point(x - b, y));
			add(Point(x, y - a));
			break;
		case 5:
			add(Point(x, y));
			add(Point(x - 0.5 * c, y - 0.5 * c));
			add(Point(x + 0.5 * c, y - 0.5 * c));
			break;
		case 6:
			add(Point(x, y));
			add(Point(x + b, y));
			add(Point(x, y - a));
			break;
		case 7:
			add(Point(x, y));
			add(Point(x + 0.5 * c, y + 0.5 * c));
			add(Point(x + 0.5 * c, y - 0.5 * c));
			break;
		case 8:
			add(Point(x, y));
			add(Point(x + b, y));
			add(Point(x, y + a));
			break;
		default:
			break;
		}
	}
	void draw_lines() const
	{
		if (color().visibility() != 0)
		{
			fl_color(color().as_int());
			fl_line(point(1).x, point(1).y, point(0).x, point(0).y);
			fl_line(point(2).x, point(2).y, point(0).x, point(0).y);
			fl_line(point(1).x, point(1).y, point(2).x, point(2).y);
		}
		if (fill_color().visibility() != 0)
		{
			fl_color(fill_color().as_int());
			fl_polygon(point(0).x, point(0).y, point(1).x, point(1).y, point(2).x, point(2).y);
		}
	}

}r1(a, a, 0, a, 1),
r2(a, a, -c / 2, c / 2, 2),
r3(a, a, -a, 0, 3),
r4(a, a, -c / 2, -c / 2, 4),
r5(a, a, 0, -a, 5),
r6(a, a, c / 2, -c / 2, 6),
r7(a, a, a, 0, 7),
r8(a, a, c / 2, c / 2, 8);

void setColor()
{
	r1.set_color(Color::red);
	r2.set_color(Color::red);
	r3.set_color(Color::red);
	r4.set_color(Color::red);
	r5.set_color(Color::red);
	r6.set_color(Color::red);
	r7.set_color(Color::red);
	r8.set_color(Color::dark_blue);

	r1.set_fill_color(Color::red);
	r2.set_fill_color(Color::red);
	r3.set_fill_color(Color::red);
	r4.set_fill_color(Color::red);
	r5.set_fill_color(Color::red);
	r6.set_fill_color(Color::red);
	r7.set_fill_color(Color::red);
	r8.set_fill_color(Color::red);
}