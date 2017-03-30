#pragma once
#ifndef CELL_WORLD
#define CELL_WORLD
#include "IObject.hpp"
#include "Food.h"
#include "Organism.hpp"

#define P_NOTHING -1
#define P_ORGANISM_SAME_SPECIES 0
#define P_ORGANISM_OTHER_SPECIES 1
#define P_FOOD 3
#define P_EDGE 4

#define ACTION_ACCELERATE 0
#define ACTION_TURN 1


struct point {
	bool valid = true;
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
	bool regenerateFood = true;
	int stepCount = 0;

	floatBase width = 160;
	floatBase height = 90;

	const vector<Organism> & getOrgs() const {
		return orgs;
	}

	const vector<Food> & getFoods() const {
		return foods;
	}
	//step the world for one tick, return false if all organism are dead
	bool step() {
		int threadn = 8;
#pragma omp parallel for num_threads(threadn)
		for (int i = 0; i < orgs.size(); i++) {
			organismPercept(i);
		}
#pragma omp parallel for num_threads(threadn)
		for (int i = 0; i < orgs.size(); i++) {
			organismStep(i);
		}
		for (int i = 0; i < orgs.size(); i++) {
			organismInteract(i);
		}
		if (regenerateFood) {

			vector<Food> foodNew;
			int food_count = 0;
			for (int i = 0; i < foods.size(); i++) {
				if (foods[i].exist) {
					foodNew.push_back(foods[i]);
					food_count++;
				}
				else {
					Food f;
					f.x = get_random()*width;
					f.y = get_random()*height;
					foodNew.push_back(f);
				}
			}
			foods = foodNew;

		}
		stepCount++;

		//cout << "food n=" << food_count << endl;

		for (int i = 0; i < orgs.size(); i++) {
			//cout << orgs[i].exist << endl;
			if (orgs[i].exist) return true;

		}
		//all orgs are dead, copy lifespan as fitness
		//cout << "All dead" << endl;
		for (int i = 0; i < orgs.size(); i++) {
			orgs[i].genome.fitness = (floatBase)orgs[i].lifespan / (floatBase)FRAME_RATE * 0.25;
		}



		return false;
	};

	void clearOrgs() {
		orgs.clear();
	}

	void clearFood() {
		foods.clear();
	}

	void addFood() {
		Food food;
		food.x = width*get_random();
		food.y = height*get_random();
		foods.push_back(food);

	};

	void addOrg(Organism & org) {
		org.x = width*get_random();
		org.y = height*get_random();
		org.direction = std::_Pi * 2 * get_random();
		orgs.push_back(org);
	}

	World();
	~World();
private:

	vector<Organism> orgs;
	vector<Food> foods;
	vector<vline> worldEdges;
	//Eucludean
	inline floatBase distance(floatBase x1, floatBase y1, floatBase x2, floatBase y2) {
		floatBase dx = x1 - x2;
		floatBase dy = y1 - y2;
		return sqrt(dx*dx + dy*dy);
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

	inline bool on_line(point & p, vline line) {
		floatBase k1, b1;

		if (line.end.x == line.start.x) {
			line.end.x += 0.000001;
		}
		k1 = (line.end.y - line.start.y) / (line.end.x - line.start.x);
		b1 = line.start.y - line.start.x*k1;
		if (p.x >= std::min(line.start.x, line.end.x) - 0.01
			&& p.x <= std::max(line.start.x, line.end.x) + 0.01) {
			floatBase y = k1*p.x + b1;
			floatBase dy = abs(y - p.y);
			if (dy <= 0.1) {
				return true;
			}
		}
		return false;

	}

	inline point intersectLines(vline l1, vline l2) {
		//solve equation for k & b //for both l1, l2
		// y_1=k*x_1+b
		// y_2=k*x_2+b

		floatBase k1, k2, b1, b2;
		if (l1.end.x == l1.start.x) {
			l1.end.x += 0.000001;
		}
		k1 = (l1.end.y - l1.start.y) / (l1.end.x - l1.start.x);
		b1 = l1.start.y - l1.start.x*k1;

		if (l2.end.x == l2.start.x) {
			l2.end.x += 0.000001;
		}
		k2 = (l2.end.y - l2.start.y) / (l2.end.x - l2.start.x);
		b2 = l2.start.y - l2.start.x*k2;

		floatBase x, y;
		floatBase tx, ty;

		if (k1 != k2) {
		}
		else {
			k1 += 0.000001;
		}
		x = -(b2 - b1) / (k2 - k1);
		y = k1 * x + b1;

		point p;
		bool o1 = on_line(p, l1);
		bool o2 = on_line(p, l2);

		p.valid = o1 && o2;

		return p;
	}

	inline point tangentObjLine(IObject & obj, vline line) {

		point result;
		//solve equation for variable k & l, with known r
		// y=kx+l;
		// (x-m)^2+(y-n)^2 = r^2
		//x^2+y^2 -2mx -2ny + (m^2+n^2-r^2)
		//
		floatBase r = obj.getRadius();
		floatBase m, c;
		floatBase p, q;
		p = obj.x;
		q = obj.y;

		if (line.end.x == line.start.x) {
			line.end.x += 0.000001;
		}
		m = (line.end.y - line.start.y) / (line.end.x - line.start.x);
		c = line.start.y - m * line.start.x;

		//floatBase r2 = r * r;
		floatBase A, B, C; // solve r^2 = x^2 + (kx+b)^2;

		A = (1 + m*m);
		B = 2 * (m*c - m*q - p);
		C = q*q - r*r + p*p - 2 * c*q + c*c;

		//possible solution x and y
		floatBase x1, x2, y1, y2;

		floatBase b2_4ac = B*B - 4 * A*C;


		if (b2_4ac > 0) {
			x1 = (-B + sqrt(b2_4ac)) / (2 * A);
			x2 = (-B - sqrt(b2_4ac)) / (2 * A);
			y1 = m*x1 + c;
			y2 = m*x2 + c;
			result =
				(
					distance(x1, y1, obj.x, obj.y) < distance(x2, y2, obj.x, obj.y)
					? point(x1, y1)
					: point(x2, y2)
					);

			result.valid = on_line(result, line);
		}
		else if (b2_4ac == 0) {
			x1 = (-B) / (2 * A);
			y1 = m*x1 + c;
			result = (point(x1, y1));
			result.valid = on_line(result, line);
		}
		else
			result.valid = false;


		return result;

	}

	inline void organismPerceptObject(Organism & org, IObject & other_obj, vector<floatBase> & perceptions, int type) {

		floatBase vision = org.vision + org.getRadius();
		floatBase cx = org.x, cy = org.y;
		floatBase dist = distance(org, other_obj) - other_obj.getRadius() - org.getRadius();
		if (dist > org.vision) return;
		for (int p = 0; p < PERCEPTION_NUMBER; p++) {
			floatBase r, lx, ly;
			r = (floatBase)p / (floatBase)PERCEPTION_NUMBER * 2 * PI + org.direction;
			lx = cx + cos(r)*vision;
			ly = cy + sin(r)*vision;
			vline visionLine(cx, cy, lx, ly); //virtual vision line, anything intersect with it will be seen as percepted
			point vpoint = tangentObjLine(other_obj, visionLine);

			if (vpoint.valid) {
				floatBase vdist = distance(org.x, org.y, vpoint.x, vpoint.y);
				if (perceptions[p] > vdist) {
					if (vdist < org.getRadius()) vdist = org.getRadius();
					perceptions[p] = vdist;
					perceptions[PERCEPTION_NUMBER + p] = type;
					perceptions[PERCEPTION_NUMBER * 2 + p] = other_obj.getSize();
				}
			}

		}
	}

	inline void organismPerceptEdges(Organism & org, vector<floatBase> & perceptions) {
		floatBase vision = org.vision + org.getRadius();
		floatBase cx = org.x, cy = org.y;
		for (int p = 0; p < PERCEPTION_NUMBER; p++) {
			floatBase r, lx, ly;
			r = (floatBase)p / (floatBase)PERCEPTION_NUMBER * 2 * PI + org.direction;
			lx = cx + cos(r)*vision;
			ly = cy + sin(r)*vision;
			vline visionLine(cx, cy, lx, ly); //virtual vision line, anything intersect with it will be seen as percepted
			for (int e = 0; e < worldEdges.size(); e++) {
				point ipoint = intersectLines(visionLine, worldEdges[e]);
				if (ipoint.valid) {
					floatBase vdist = distance(org.x, org.y, ipoint.x, ipoint.y);
					if (perceptions[p] > vdist) {
						if (vdist < org.getRadius()) vdist = org.getRadius();
						perceptions[p] = vdist;
						perceptions[PERCEPTION_NUMBER + p] = P_EDGE;
						perceptions[PERCEPTION_NUMBER * 2 + p] = 0;
					}
				}
			}

		}
	}

	inline void organismPercept(int k) {
		//compute the visual input for i th organism, for its neural network
		Organism & org = orgs[k];
		if (!org.exist) return;
		vector<floatBase> perceptions(PERCEPTION_NUMBER * 3 + 1, P_NOTHING);
		perceptions[perceptions.size() - 1] = org.getSize();
		for (int i = 0; i < PERCEPTION_NUMBER; i++) {
			perceptions[i] = org.vision;
		}
		// perception other oganisms
		for (int i = 0; i < orgs.size(); i++) {
			if (i != k) {
				Organism & other = orgs[i];
				int species = P_ORGANISM_SAME_SPECIES;
				if (other.getTypeID() == org.getTypeID()) {
					species = P_ORGANISM_OTHER_SPECIES;
				}
				organismPerceptObject(org, other, perceptions, species);
			}
		}
		for (int i = 0; i < foods.size(); i++) {
			Food & other = foods[i];
			organismPerceptObject(org, other, perceptions, P_FOOD);
		}
		organismPerceptEdges(org, perceptions);
		org.perception = perceptions;
	}

	inline void organismStep(int k) {
		Organism & org = orgs[k];

		if (!org.exist) {
			return;
		}
		vector<floatBase> action = org.neuralNetwork.activate(org.perception);

		floatBase timeScale = 1 / (floatBase)FRAME_RATE;


		//org.acceleration = action[ACTION_ACCELERATE] * timeScale;
		org.direction += action[ACTION_TURN] * org.agility * timeScale;
		org.speed = action[ACTION_ACCELERATE] * org.maxSpeed;
		//if (std::isnan(org.speed)) {
		//	cout << "ERROR" << endl;
		//	//throw 500;

		//}
		//org.speed = std::max(std::min(org.speed, org.maxSpeed), (floatBase)0.0);

		floatBase r = org.getRadius();
		float dx = cos(org.direction) * org.speed * timeScale;
		float dy = sin(org.direction) * org.speed * timeScale;
		org.x += dx;
		org.y += dy;
		//cout << "mx=" << dx << ", my=" << dy << ", speed=" << org.speed << endl;
		//cout << "dir=" << org.direction << endl;

		if (org.x < r) org.x = r;
		if (org.x > width - r) org.x = width - r;
		if (org.y < r)org.y = r;
		if (org.y > height - r) org.y = height - r;
		floatBase consumption = hungerFormula(org.getSize(), org.speed, action[ACTION_TURN]);
		//cout << consumption * timeScale<<", "<< org.hunger<< endl;
		if (consumption < 0) {
			cout << "org.getSize()=" << org.getSize() << ", org.speed=" << org.speed << ", " << action[ACTION_TURN] << endl;
			throw 202;
		}
		org.hunger -= consumption* timeScale;
		//cout <<"h: "<< org.hunger << endl;

		if (org.hunger <= 0) org.exist = false;
		org.lifespan += 1;
	}

	inline void _interact(Organism & org, IObject & other) {
		if (isColliding(org, other)) {

			//if collide, if food, eat directly and add hunger value, if both organism, check size and eat at condition;
			if (other.getTypeID() == ID_FOOD) {
				if (org.getSize() > other.getSize()*0.95) { //eat the other organism if size is much larger
					other.exist = false;
					org.foodGet++;
					org.hunger += pow(other.getSize(), 2); // e=1/2mv^2, where v is however related to  :)
				}

			}
			else if (other.getTypeID() == ID_ORGANISM) {
				if (org.getSize() > other.getSize()*1.25) { //eat the other organism if size is much larger
					other.exist = false;
					org.foodGet++;
					org.hunger += pow(other.getSize(), 2); // e=1/2mv^2, where v is however related to  :)
				}
			}
		}
	}

	inline void organismInteract(int k) {

		Organism & org = orgs[k];
		for (int i = 0; i < orgs.size(); i++) {
			if (i != k) {
				Organism & other = orgs[i];
				if (other.exist && other.getSpecies()!=org.getSpecies())
					_interact(org, other);
			}
		}
		for (int i = 0; i < foods.size(); i++) {
			Food & food = foods[i];
			if (food.exist)
				_interact(org, food);
		}

	}


};


#endif // !CELL_WORLD
