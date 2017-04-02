#pragma once
#ifndef CELL_WORLD
#define CELL_WORLD
#include "IObject.hpp"
#include "Food.h"
#include "Organism.hpp"
#include <math.h>

#define P_NOTHING -500
#define P_FOOD -100
#define P_NEUTRAL 100
#define P_SAME_SPECIES 300
#define P_ORGANISM_THREAT 500
#define P_EDGE 700

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

using orgGrid = vector<Organism *>;
using foodGrid = vector<Food *>;

using orgMatrix = vector<vector<orgGrid>>;
using foodMatrix = vector<vector<foodGrid>>;

class World
{
public:
	bool foodChain = true;
	bool perceptNeutral = false;
	int regenerateFood = 240;
	int stepCount = 0;

	floatBase width = 480;
	floatBase height = 270;

	orgMatrix orgMat;
	foodMatrix foodMat;

	int collisionMatrixWidth;
	int collisionMatrixHeight;


	floatBase gridSize;

	const vector<Organism> & getOrgs() const {
		return orgs;
	}

	const vector<Food> & getFoods() const {
		return foods;
	}
	//step the world for one tick, return false if all organism are dead

	bool step(bool visualising) {
		if (!visualising) {
#pragma omp parallel for
			for (int i = 0; i < orgs.size(); i++) {
				organismPercept(i);
			}
#pragma omp parallel for
			for (int i = 0; i < orgs.size(); i++) {
				organismStep(i);
			}
		}
		else {
			for (int i = 0; i < orgs.size(); i++) {
				organismPercept(i);
			}
			for (int i = 0; i < orgs.size(); i++) {
				organismStep(i);
			}
		}
		for (int i = 0; i < orgs.size(); i++) {
			organismInteract(i);
		}
		vector<Food> foodNew;
		int food_count = 0;
		for (int i = 0; i < foods.size(); i++) {
			if (foods[i].exist) {
				foodNew.push_back(foods[i]);
				food_count++;
			}
			else if (regenerateFood > 0) {
				Food f;
				f.x = get_random()*width;
				f.y = get_random()*height;
				foodNew.push_back(f);
				regenerateFood--;

			}

		}
		foods = foodNew;
		clearGrid(foodMat);
		addAllFoodsToGrid(foods, foodMat);
		stepCount++;

		//cout << "food n=" << food_count << endl;
		if (stepCount < FRAME_RATE * 10000)
			for (int i = 0; i < orgs.size(); i++) {
				//cout << orgs[i].exist << endl;
				if (orgs[i].exist) return true;

			}
		//all orgs are dead, copy lifespan as fitness
		//cout << "All dead" << endl;

		for (int i = 0; i < orgs.size(); i++) {
			orgs[i].genome.fitness = org_Fitness(orgs[i]);
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
		food.id = 100000 + foods.size();
		foods.push_back(food);

	};

	void addOrg(Organism & org) {
		org.x = width*get_random();
		org.y = height*get_random();
		org.direction = std::_Pi * 2 * get_random();
		org.genome.fitness = 0;
		org.id = orgs.size();
		orgs.push_back(org);
	}

	void initGrid() {
		//find out max perception + radius 

		floatBase gridSize = 0;
		for (int i = 0; i < orgs.size(); i++) {
			Organism & org = orgs[i];
			if (org.getRadius() + org.vision > gridSize) {
				gridSize = org.getRadius() + org.vision;
			}
		}

		int gridWidth = ceil(width / gridSize);
		int gridHeight = ceil(height / gridSize);

		collisionMatrixHeight = gridHeight;
		collisionMatrixWidth = gridWidth;

		foodMat = foodMatrix(gridHeight, vector<foodGrid>(gridWidth));
		orgMat = orgMatrix(gridHeight, vector<orgGrid>(gridWidth));

		this->gridSize = gridSize;

		addAllOrgsToGrid(orgs, orgMat);
		addAllFoodsToGrid(foods, foodMat);

	}

	World();
	~World();
private:

	vector<Organism> orgs;
	vector<Food> foods;
	vector<vline> worldEdges;

	inline void addOrgToGrid(orgMatrix & cmat, Organism & org, int col, int row) {

		cmat[row][col].push_back(&org);
	}

	inline void removeOrgFromGrid(orgMatrix & cmat, Organism & org, int col, int row) {
		orgGrid & g = cmat[row][col];
		orgGrid new_g;
		for (int i = 0; i < g.size(); i++) {
			Organism * orgPtr = (g[i]);
			if (orgPtr->id != org.id) {
				new_g.push_back(orgPtr);
			}
		}
		g = new_g;
	}

	inline void addFoodToGrid(foodMatrix & cmat, Food & org, int col, int row) {

		cmat[row][col].push_back(&org);
	}

	inline void removeFoodFromGrid(foodMatrix & cmat, Food & org, int col, int row) {
		foodGrid & g = cmat[row][col];
		foodGrid new_g;
		for (int i = 0; i < g.size(); i++) {
			Food * orgPtr = (g[i]);
			if (orgPtr->id != org.id) {
				new_g.push_back(orgPtr);
			}
		}
		g = new_g;
	}

	inline void clearGrid(foodMatrix & cmat) {
		for (int row = 0; row < cmat.size(); row++) {
			for (int col = 0; col < cmat[row].size(); col++) {
				cmat[row][col].clear();
			}
		}
	}

	inline void  addAllOrgsToGrid(vector<Organism> & objs, orgMatrix & cmat) {
		for (int i = 0; i < objs.size(); i++) {
			Organism & obj = objs[i];
			int grid_col = obj.x / gridSize;
			int grid_row = obj.y / gridSize;
			addOrgToGrid(cmat, obj, grid_col, grid_row);
		}
	}
	inline void  addAllFoodsToGrid(vector<Food> & objs, foodMatrix & cmat) {
		for (int i = 0; i < objs.size(); i++) {
			Food & obj = objs[i];
			int grid_col = obj.x / gridSize;
			int grid_row = obj.y / gridSize;
			addFoodToGrid(cmat, obj, grid_col, grid_row);
		}
	}

	inline floatBase org_Fitness(Organism & org) {
		return org.lifespan / FRAME_RATE;
	}

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
		point va = point(line.start.x - line.end.x, line.start.y - line.end.y);
		point vb = point(p.x - line.end.x, p.y - line.end.y);
		floatBase area = va.x * vb.y - va.y *vb.x;
		if (abs(area) < 0.001) {
			return p.x >= std::min(line.start.x, line.end.x) && p.x <= std::max(line.start.x, line.end.x)
				&& p.y >= std::min(line.start.y, line.end.y) && p.y <= std::max(line.start.y, line.end.y);
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

		point p(x, y);
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

	inline floatBase normalise_angle(floatBase r) {
		r = fmod(r, PI * 2);
		r = r > PI ? r - 2 * PI : r;
		r = r < -PI ? r + 2 * PI : r;
		return r;
	}
	//absolute difference in angle r1 , r2
	inline floatBase inc_angle(floatBase r1, floatBase r2) {

		r1 = fmod(r1, PI * 2);
		r2 = fmod(r2, PI * 2);

		floatBase dr = r2 - r1;

		return normalise_angle(dr);
	}

	//absolute difference in angle r1 , r2 clockwise
	inline floatBase inc_angle_clockwise(floatBase r1, floatBase r2) {

		r1 = normalise_angle(r1);
		r2 = normalise_angle(r2);

		floatBase dr = r2 - r1;
		dr = dr < 0 ? dr + 2 * PI : dr;
		return dr;
	}



	inline void organismPerceptObject(Organism & org, IObject & other_obj, vector<floatBase> & perceptions, int type) {

		floatBase vision = org.vision + org.getRadius();
		floatBase cx = org.x, cy = org.y;
		floatBase dist = distance(org, other_obj) - other_obj.getRadius() - org.getRadius();
		if (dist > org.vision) return;
		if (!perceptNeutral && type == P_NEUTRAL) return;

		floatBase ox = other_obj.x;
		floatBase oy = other_obj.y;

		floatBase dx = ox - cx;
		floatBase dy = oy - cy;

		floatBase p_part = (floatBase)1 / (floatBase)PERCEPTION_NUMBER * PERCEPT_RANGE;
		floatBase p_angle = atan2(dy, dx);
		floatBase start_p_angle = org.direction - PERCEPT_RANGE / 2;
		floatBase p_relative_angle = inc_angle(p_angle, org.direction);
		if (abs(p_relative_angle) > PERCEPT_RANGE / 2) return;
		floatBase tmp = (inc_angle_clockwise(start_p_angle, p_angle));
		int k = tmp / abs(p_part);
		//if (k < 0) throw 1010;
		floatBase vdist = distance(cx, cy, ox, oy);
		perceptions[k] = vdist - other_obj.getRadius();
		perceptions[PERCEPTION_NUMBER + k] = type;
		perceptions[PERCEPTION_NUMBER * 2 + k] = (other_obj.getSize() - org.getSize()) * 100;

	}

	inline void organismPerceptEdges(Organism & org, vector<floatBase> & perceptions) {


		floatBase vision = org.vision + org.getRadius();
		floatBase cx = org.x, cy = org.y;
		for (int p = 0; p < PERCEPTION_NUMBER; p++) {
			floatBase r, lx, ly;
			r = ((floatBase)p + 0.5) / (floatBase)PERCEPTION_NUMBER * PERCEPT_RANGE + org.direction - PERCEPT_RANGE / 2;
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
		vector<floatBase> perceptions(PERCEPTION_NUMBER * 3 + 2, P_NOTHING);

		perceptions[perceptions.size() - 1] = org.getSize();
		perceptions[perceptions.size() - 2] = org.speed;

		for (int i = 0; i < PERCEPTION_NUMBER; i++) {
			perceptions[i] = org.vision;
		}
		// perception other food or organism
		using std::min;
		using std::max;

		int grid_col = org.x / gridSize;
		int grid_row = org.y / gridSize;
		int startCol = max(grid_col - 1, 0);
		int startRow = max(grid_row - 1, 0);
		int endCol = min(grid_col + 1, collisionMatrixWidth - 1);
		int endRow = min(grid_row + 1, collisionMatrixHeight - 1);
		for (int row = startRow; row <= endRow; row++) {
			for (int col = startCol; col <= endCol; col++) {
				//Organism
				orgGrid & g = orgMat[row][col];
				for (int i = 0; i < g.size(); i++) {
					Organism * other = (g[i]);

					if (other->id == org.id) continue;
					if (!other->exist) continue;

					int perceptType = P_NEUTRAL;
					if (foodChain) {
						if (org.getSpecies() - other->getSpecies() == 1) {
							perceptType = P_FOOD;
						}
						else if (other->getSpecies() - org.getSpecies() == 1) {
							perceptType = P_ORGANISM_THREAT;
						}
						else if (other->getSpecies() == org.getSpecies()) {
							perceptType = P_SAME_SPECIES;
						}
					}
					else {
						if (other->getSpecies() != org.getSpecies()) {
							if (other->getSize() > org.getSize()) {
								perceptType = P_ORGANISM_THREAT;
							}
							else {
								perceptType = P_NEUTRAL;
							}
						}
						else {
							perceptType = P_SAME_SPECIES;
						}
					}
					organismPerceptObject(org, *other, perceptions, perceptType);
				}
				//Food
				foodGrid & g2 = foodMat[row][col];
				for (int i = 0; i < g2.size(); i++) {
					Food * food = (g2[i]);
					if (!food->exist) continue;
					if (foodChain) {
						if (org.getSpecies() == 0) {
							organismPerceptObject(org, *food, perceptions, P_FOOD);
						}
						else {
							organismPerceptObject(org, *food, perceptions, P_NEUTRAL);
						}
					}
					else {
						organismPerceptObject(org, *food, perceptions, P_FOOD);
					}
				}

			}
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


		int grid_col = org.x / gridSize;
		int grid_row = org.y / gridSize;

		//org.acceleration = action[ACTION_ACCELERATE] * timeScale;
		org.direction += action[ACTION_TURN] * org.agility * timeScale;
		org.direction = org.direction > PI ? (org.direction - PI * 2) : org.direction;
		org.direction = org.direction < -PI ? (org.direction + PI * 2) : org.direction;

		org.speed += action[ACTION_ACCELERATE] * (org.maxSpeed * FRAME_RATE);
		//if (std::isnan(org.speed)) {
		//	cout << "ERROR" << endl;
		//	//throw 500;

		//}
		org.speed = std::max(std::min(org.speed, org.maxSpeed), (floatBase)0.0);

		floatBase r = org.getRadius();
		float dx = cos(org.direction) * org.speed * timeScale;
		float dy = sin(org.direction) * org.speed * timeScale;
		org.x += dx;
		org.y += dy;
		//cout << "mx=" << dx << ", my=" << dy << ", speed=" << org.speed << endl;
		//cout << "dir=" << org.direction << endl;


		int grid_col2 = org.x / gridSize;
		int grid_row2 = org.y / gridSize;

#pragma omp critical
		{
			if (grid_col != grid_col2 || grid_row != grid_row2) {
				removeOrgFromGrid(orgMat, org, grid_col, grid_row);
				addOrgToGrid(orgMat, org, grid_col2, grid_row2);
			}
		}
		if (org.x < r) {
			org.x = r; //org.hunger -= timeScale * 2;
		}
		if (org.x > width - r) {
			org.x = width - r;// org.hunger -= timeScale * 2;
		}
		if (org.y < r) {
			org.y = r; //org.hunger -= timeScale * 2;
		}
		if (org.y > height - r) {
			org.y = height - r;// org.hunger -= timeScale * 2;
		}
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
				if (foodChain) {
					if (org.getSpecies() == 0) {
						other.exist = false;
						org.foodGet++;
						org.hunger += pow(SIZE_BASE, 2); // e=1/2mv^2, where v is however related to  :)
					}
				}
				else if (org.getSize() > other.getSize()*0.95) { //eat the other organism if size is much larger
					other.exist = false;
					org.foodGet++;
					org.hunger += pow(SIZE_BASE, 2); // e=1/2mv^2, where v is however related to  :)
				}

			}
			else if (other.getTypeID() == ID_ORGANISM) {
				if (foodChain) {

					Organism * orgPtr = dynamic_cast<Organism *>(&other);

					if (org.getSpecies() - orgPtr->getSpecies() == 1) {
						orgPtr->exist = false;
						org.foodGet++;
						org.hunger += pow(SIZE_BASE, 2); // e=1/2mv^2, where v is however related to  :)
					}
				}
				else  if (org.getSize() > other.getSize()*1.25) { //eat the other organism if size is much larger
					other.exist = false;
					org.foodGet++;
					org.hunger += pow(SIZE_BASE, 2); // e=1/2mv^2, where v is however related to  :)
				}
			}
		}
	}

	inline void organismInteract(int k) {

		Organism & org = orgs[k];

		using std::max;
		using std::min;

		int grid_col = org.x / gridSize;
		int grid_row = org.y / gridSize;
		int startCol = max(grid_col - 1, 0);
		int startRow = max(grid_row - 1, 0);
		int endCol = min(grid_col + 1, collisionMatrixWidth - 1);
		int endRow = min(grid_row + 1, collisionMatrixHeight - 1);
		for (int row = startRow; row <= endRow; row++) {
			for (int col = startCol; col <= endCol; col++) {
				//Organism
				orgGrid & g = orgMat[row][col];
				for (int i = 0; i < g.size(); i++) {
					Organism * other = (g[i]);

					if (other->id == org.id) continue;
					if (!other->exist) continue;

					if (other->getSpecies() != org.getSpecies())
						_interact(org, *other);
				}
				//Food
				if (org.getSpecies() > 0 && foodChain) { continue; }
				foodGrid & g2 = foodMat[row][col];
				for (int i = 0; i < g2.size(); i++) {
					Food * food = (g2[i]);
					if (!food->exist) continue;
					_interact(org, *food);
				}

			}
		}


	}

};


#endif // !CELL_WORLD
