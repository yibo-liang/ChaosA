#pragma once
#ifndef CELL_WORLD
#define CELL_WORLD
#include "IObject.hpp"
#include "Food.h"
#include "Organism.hpp"

struct point {
	floatBase x;
	floatBase y;
	point() {};
	point(floatBase x, floatBase y) {
		this->x = x;
		this->y = y;
	};
};

struct vline
{
	point start;
	point end;
	vline() {};
	vline(floatBase x1, floatBase y1, floatBase x2, floatBase y2) {
		start.x = x1;
		start.y = y1;
		end.x = x2;
		end.y = y2;
	}
};

class World
{
public:


	floatBase width = 800;
	floatBase height = 600;

	vector<IObject> objs;

	vector<IObject *> orgs;
	vector<IObject *> foods;


	void step();

	void addFood() {
		Food food;
		objs.push_back(food);
		foods.push_back(&objs[objs.size() - 1]);

	};

	void addOrg(Organism org) {

		objs.push_back(org);
		orgs.push_back(&objs[objs.size() - 1]);
	}

	World();
	~World();
private:



	//Eucludean
	inline floatBase distance(floatBase x1, floatBase y1, floatBase x2, floatBase y2) {
		floatBase dx = x1 - x2;
		floatBase dy = y1 - y2;
	}

	inline floatBase distance(const IObject & a, const IObject & b) {
		floatBase dx = a.x - b.x;
		floatBase dy = a.y - b.y;
		return sqrt(dx*dx + dy*dy);
	}

	inline bool isColliding(IObject & a, IObject & b) {
		floatBase ds = distance(a, b);
		return ds < a.getRadius() + b.getRadius();
	}

	inline bool on_line(point & p, vline & line) {
		floatBase xmin = std::min(line.start.x, line.end.x);
		floatBase xmax = std::max(line.start.x, line.end.x);

		floatBase ymin = std::min(line.start.y, line.end.y);
		floatBase ymax = std::max(line.start.y, line.end.y);
		return (p.x >= xmin && p.x <= xmax && p.y >= ymin && p.y <= ymax);

	}

	inline vector<point> tangentObjLine(IObject obj, vline line) {

		vector<point> result;
		//solve equation 
		// y=kx+l;
		// x^2+y^2 = r^2
		floatBase r = obj.getRadius();
		floatBase k, l;
		k = (line.end.y - line.start.y) / (line.end.x - line.start.x);
		l = line.start.y - k * line.start.x;

		//floatBase r2 = r * r;
		floatBase a, b, c; // solve r^2 = x^2 + (kx+b)^2;

		a = (1 + k*k);
		b = (2 * k*l);
		c = l*l - r*r;

		//possible solution x and y
		floatBase x1, x2, y1, y2;

		floatBase b2_4ac = b*b - 4 * a*c;

		vector<point> temp;
		if (b2_4ac > 0) {
			x1 = (-b + sqrt(b2_4ac)) / (2 * a);
			x2 = (-b - sqrt(b2_4ac)) / (2 * a);
			y1 = k*x1 + l;
			y2 = k*x2 + l;
			temp.push_back(point(x1, y1));
			temp.push_back(point(x2, y2));
		}
		else if (b2_4ac == 0) {
			x1 = (-b) / (2 * a);
			y1 = k*x1 + l;
			temp.push_back(point(x1, y1));
		}

		for (auto & p : temp) {
			if (on_line(p, line)) {
				result.push_back(p);
			}
		}

		return result;

	}


};


#endif // !CELL_WORLD
