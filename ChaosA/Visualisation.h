#pragma once
#ifndef _VIS
#define _VIS

#include <SFML\Graphics.hpp>
#include "World.h"
#include <thread>
#include <strstream>
using Window = sf::RenderWindow;
using sf::CircleShape;
using sf::Vector2f;

class Visualisation
{
public:

	int windowWidth = 1600;
	int windowHeight = 960;

	std::shared_ptr<Window*> window;
	std::thread * eventThread;
	std::shared_ptr<bool> windowCreated;
	std::shared_ptr<bool> exit;
	std::shared_ptr<bool> fastMode;
	std::shared_ptr<bool> showPerception;


	std::shared_ptr<bool> showMaximum;
	std::shared_ptr<bool> showAverage;

	sf::Color colorFood = sf::Color(0, 255, 0);
	sf::Color colorEdge = sf::Color(255, 244, 170);

	floatBase maxFitness = DBL_MIN;
	floatBase minFitness = DBL_MAX;
	std::vector<vector<floatBase>> averageFitnessRecord, maxFitnessRecord;

	void renderFitness(vector<std::pair<floatBase, floatBase>> newfitness, World & world) {

		updateScale(world);
		for (int s = 0; s < newfitness.size(); s++) {
			floatBase max = newfitness[s].first;
			floatBase avr = newfitness[s].second;
			if (maxFitnessRecord.size() == 0) {
				maxFitnessRecord = vector<vector<floatBase>>(newfitness.size());
				averageFitnessRecord = vector<vector<floatBase>>(newfitness.size());
			}
			maxFitnessRecord[s].push_back(max);
			averageFitnessRecord[s].push_back(avr);
			if (max > maxFitness) maxFitness = max;
			if (avr < minFitness) minFitness = avr;
		}
		Window * w = *window.get();
		w->clear();

		floatBase dfit = maxFitness - minFitness;
		for (int s = 0; s < maxFitnessRecord.size(); s++) {

			sf::VertexArray linemax(sf::LinesStrip, maxFitnessRecord[s].size() + 1);
			sf::VertexArray lineavr(sf::LinesStrip, maxFitnessRecord[s].size() + 1);


			linemax[0].position = sf::Vector2f(0, windowHeight);
			lineavr[0].position = sf::Vector2f(0, windowHeight);
			for (int i = 0; i < maxFitnessRecord[s].size(); i += 1) {
				floatBase maxFit = maxFitnessRecord[s][i];
				floatBase avrFit = averageFitnessRecord[s][i];

				sf::Vector2f maxp(
					(i + 1) / (floatBase)maxFitnessRecord[s].size() * (floatBase)windowWidth,
					windowHeight - (maxFit - minFitness) / dfit * (floatBase)windowHeight
				);
				linemax[i + 1].position = maxp;
				linemax[i + 1].color = speciesColours[s];

				if (*showAverage)
					linemax[i + 1].color.a = 85;

				sf::Vector2f avrp(
					(i + 1) / (floatBase)averageFitnessRecord[s].size() * (floatBase)windowWidth,
					windowHeight - (avrFit - minFitness) / dfit * (floatBase)windowHeight
				);
				lineavr[i + 1].position = avrp;
				lineavr[i + 1].color = speciesColours[s];

			}
			if (*showMaximum)
				w->draw(linemax);
			if (*showAverage)
				w->draw(lineavr);

		}

		w->display();
	}

	void render(World & world) {
		if (!*windowCreated) return;
		updateScale(world);
		Window * w = *window.get();
		w->clear();

		/*  Drawing of the world here */
		vector<Organism> orgs = world.getOrgs();
		for (int i = 0; i < orgs.size(); i++) {
			if (orgs.at(i).exist)
				drawOrganism(orgs.at(i), w);
		}
		vector<Food> foods = world.getFoods();
		for (int i = 0; i < foods.size(); i++) {
			if (foods.at(i).exist)
				drawFood(foods.at(i), w);
		}



		w->display();
	}

	Visualisation() {

		windowCreated = std::make_shared<bool>(false);
		exit = std::make_shared<bool>(false);
		fastMode = std::make_shared<bool>(false);
		showPerception = std::make_shared<bool>(false);


		showMaximum = std::make_shared<bool>(true);
		showAverage = std::make_shared<bool>(true);

		eventThread = handleEvent();
		speciesColours.push_back(sf::Color(150, 50, 250));
		speciesColours.push_back(sf::Color(100, 190, 150));
		speciesColours.push_back(sf::Color(200, 130, 90));
		speciesColours.push_back(sf::Color(30, 160, 250));
		speciesColours.push_back(sf::Color(0, 180, 150));


	};
	~Visualisation() {
		//delete eventThread;
	};

private:
	Vector2f disposition;

	std::shared_ptr<floatBase> scale = std::make_shared<floatBase>(0);

	vector<sf::Color> speciesColours;

	inline void updateScale(World & world) {
		Window * w = *window.get();
		w->clear();
		sf::Vector2u size = w->getSize();
		windowWidth = size.x;
		windowHeight = size.y;

		floatBase vscale = (floatBase)windowHeight / world.height;
		floatBase hscale = (floatBase)windowWidth / world.width;

		*scale = std::min(vscale, hscale);


	}

	inline floatBase getScale() {
		return (*scale);
	}

	void drawOrganism(const Organism org, Window * w) {
		floatBase r = org.getRadius()*getScale();
		floatBase x = (org.x + disposition.x)* getScale();
		floatBase y = (org.y + disposition.y)* getScale();
		CircleShape body(r);
		body.setOutlineColor(speciesColours[org.getSpecies()]);
		body.setFillColor(sf::Color(0, 0, 0, 0));

		body.setOutlineThickness(2);
		Vector2f pos = Vector2f(x - r, y - r);
		body.setPosition(pos);
		w->draw(body);
		sf::VertexArray line(sf::Lines, 2);
		line[0].position = Vector2f(x, y);
		line[1].position = Vector2f(x + r*cos(org.direction), y + r*sin(org.direction));
		w->draw(line);


		if (!*showPerception) return;

		floatBase p_part = (floatBase)1 / (floatBase)PERCEPTION_NUMBER * PERCEPT_RANGE;
		for (int i = 0; i < PERCEPTION_NUMBER; i++) {
			floatBase l = org.perception.at(i) * getScale();
			floatBase rotation = (floatBase)i / (floatBase)PERCEPTION_NUMBER * PERCEPT_RANGE + org.direction - PERCEPT_RANGE / 2;

			Vector2f p1(x + l*cos(rotation), y + l*sin(rotation));
			Vector2f p2(x + l*cos(rotation + p_part), y + l*sin(rotation + p_part));
			sf::ConvexShape convex;
			convex.setPointCount(3);
			convex.setPoint(0, p1);
			convex.setPoint(1, p2);
			convex.setPoint(2, Vector2f(x, y));
			sf::Color pcolor;
			floatBase ptype = org.perception[PERCEPTION_NUMBER + i];
			if (abs(ptype - P_NOTHING) < 0.1) {
				pcolor = sf::Color(0, 0, 0, 0);
			}
			else if (abs(ptype - P_EDGE) < 0.1) {
				pcolor = colorEdge;
			}
			else if (abs(ptype - P_FOOD) < 0.1) {
				pcolor = colorFood;
			}
			else if (abs(ptype - P_ORGANISM_THREAT) < 0.1) {
				pcolor = sf::Color(255, 0, 0);
			}
			else if (abs(ptype - P_NEUTRAL) < 0.1) {
				pcolor = sf::Color(200, 200, 200);
			}
			pcolor.a = 100;

			convex.setFillColor(pcolor);
			w->draw(convex);
		}

	}

	void drawFood(const Food org, Window * w) {
		floatBase r = org.getRadius()*getScale();
		floatBase x = (org.x + disposition.x)*getScale() - r;
		floatBase y = (org.y + disposition.y)*getScale() - r;
		CircleShape body(r);
		body.setFillColor(sf::Color(0, 0, 0, 0));
		body.setOutlineColor(colorFood);
		body.setOutlineThickness(2);

		Vector2f pos = Vector2f(x, y);
		body.setPosition(pos);
		w->draw(body);
	}


	std::thread * handleEvent() {
		return new std::thread([&] {windowEventThread(); });
	}

	void windowEventThread() {
		window = std::make_shared<Window *>(new sf::RenderWindow(sf::VideoMode(windowWidth, windowHeight), "SFML works!", sf::Style::Default));
		Window * w = *window.get();
		w->setFramerateLimit(FRAME_RATE);
		w->setVerticalSyncEnabled(true);
		w->setActive(false);
		*windowCreated = true;
		while (w->isOpen())
		{
			sf::Event event;
			while (w->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					w->close();
				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code)
					{
					case sf::Keyboard::Escape:
						*windowCreated = false;
						*exit = true;
						w->close();
						return;
					case sf::Keyboard::F:
						*fastMode = !*fastMode;
						break;
					case sf::Keyboard::P:
						*showPerception = !*showPerception;
						break;
					case sf::Keyboard::M:
						*showMaximum = !*showMaximum;
						break;
					case sf::Keyboard::N:
						*showAverage = !*showAverage;
						break;

					default:
						break;
					}
				}
				if (event.type == sf::Event::Resized) {
					w->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
				}
				//cout << "event" << endl;
			}

			//w->display();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		return;
	}

};

#endif // !_VIS
