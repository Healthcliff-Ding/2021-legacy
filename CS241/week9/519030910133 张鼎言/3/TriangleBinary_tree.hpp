#include "Simple_window.h"    // get access to our window library
#include "Graph.h"            // get access to our graphics library facilities
#include "std_lib_facilities.h"
#include <queue>
struct tri :public Shape
{
public:
	tri(Point pp) {
		add(Point(pp.x, pp.y - 6));
		add(Point(pp.x - 5, pp.y + 3));
		add(Point(pp.x + 5, pp.y + 3));
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
};
class TriangleBinary_tree : public Binary_tree
{
private:
	vector<tri*> triag;
	queue<Point> _points;
	int tot = 0;
public:
	TriangleBinary_tree() {}
	TriangleBinary_tree(int N)
	{
		set_color(Color::black);
		N = abs(N);
		int space = 600;
		int h = 100;
		if (N > 0)
		{
			triag.resize(N);
			Point rt(600, 100);
			node root(6, rt);
			triag[tot++] = new tri(rt);
			_points.push(rt);
			for (int i = 2; i <= N; ++i, space /= 2)
			{
				int sz = _points.size();
				for (int j = 1; j <= sz; ++j)
				{
					Point tmp = _points.front();
					_points.pop();
					Point lc(tmp.x - 0.5 * space, tmp.y + h);
					Point rc(tmp.x + 0.5 * space, tmp.y + h);
					branch.add(tmp, lc);
					branch.add(tmp, rc);
					tri ll(lc), rr(rc);
					triag[tot++] = new tri(lc);
					triag[tot++] = new tri(rc);
					_points.push(lc);
					_points.push(rc);
				}
			}
		}
	}
	virtual void draw_lines() const
	{
		if (color().visibility())
			branch.draw_lines();
		for (int i = 0; i < tot; ++i)
		{
			triag[i]->set_color(Color::dark_cyan);
			triag[i]->set_fill_color(Color::cyan);
			triag[i]->draw_lines();
		}
	}
};
