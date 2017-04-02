#pragma once
#ifndef _VIS
#define _VIS

#include <SFML\Graphics.hpp>
#include "World.h"
#include "Pool.hpp"
#include <thread>
#include <strstream>
#include <memory>

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

	sf::Font font;

	floatBase maxFitness = DBL_MIN;
	floatBase minFitness = DBL_MAX;
	std::vector<vector<floatBase>> averageFitnessRecord, maxFitnessRecord;

	std::shared_ptr<Pool> & pool;

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
		for (auto s = 0; s < maxFitnessRecord.size(); s += 1) {

			sf::VertexArray linemax(sf::LinesStrip);
			sf::VertexArray lineavr(sf::LinesStrip);


			linemax.append(sf::Vertex(sf::Vector2f(0, windowHeight)));
			lineavr.append(sf::Vertex(sf::Vector2f(0, windowHeight)));

			auto step = (maxFitnessRecord[s].size() / 500) + 1;
			auto skipRange = maxFitnessRecord[s].size() - maxFitnessRecord[s].size() % step;
			for (auto i = 0; i < maxFitnessRecord[s].size(); i += (i < skipRange ? step : 1)) {
				floatBase maxFit = maxFitnessRecord[s][i];
				floatBase avrFit = averageFitnessRecord[s][i];

				sf::Vector2f maxp(
					(i + 1) / (floatBase)maxFitnessRecord[s].size() * (floatBase)windowWidth,
					windowHeight - (maxFit - minFitness) / dfit * (floatBase)windowHeight
				);
				sf::Vertex vmax(maxp);
				vmax.color = speciesColours[s];
				if (*showAverage)
					vmax.color.a = 85;
				linemax.append(vmax);

				sf::Vector2f avrp(
					(i + 1) / (floatBase)averageFitnessRecord[s].size() * (floatBase)windowWidth,
					windowHeight - (avrFit - minFitness) / dfit * (floatBase)windowHeight
				);
				sf::Vertex vavr(avrp);
				vavr.color = speciesColours[s];
				lineavr.append(vavr);

			}
			if (*showMaximum)
				w->draw(linemax);
			if (*showAverage)
				w->draw(lineavr);
			renderDefaultText();
			renderText(newfitness);
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

		renderDefaultText();
		w->display();
	}

	void renderDefaultText() {

		Window * w = *window.get();
		using std::to_string;
		sf::Text text;
		text.setFont(font);
		text.setString(
			"[M] HIDE/SHOW Maximum Fitness      [N] HIDE/SHOW Average Fitness      [F] Turn ON/OFF Fast Mode      [P] Turn ON/OFF VISION");
		text.setCharacterSize(12);
		text.setFillColor(sf::Color::White);
		text.setPosition(Vector2f(10, 10));

		w->draw(text);

		using std::to_string;
		string tmp = ((*fastMode) ? "ON" : "OFF");
		text.setString(
			"Fast Mode = "
			+ tmp);
		text.setCharacterSize(12);
		text.setFillColor(sf::Color::White);
		text.setPosition(Vector2f(10, 28));
		w->draw(text);
	}

	void renderText(vector<std::pair<floatBase, floatBase>> newfitness) {

		Window * w = *window.get();
		using std::to_string;


		sf::Text text;
		text.setFont(font);
		text.setString(
			"Generation : "
			+ to_string(maxFitnessRecord[0].size()));
		text.setCharacterSize(12);
		text.setFillColor(sf::Color::White);
		text.setPosition(Vector2f(10, 46));

		w->draw(text);

		for (int s = 0; s < newfitness.size(); s++) {

			sf::Text text;

			string star = "";
			if (pool->swap && s == pool->currentEvolveSpecies) {
				star = " *";
			}
			text.setFont(font);
			text.setString(
				"[" + to_string(s) + "] MAX="
				+ to_string(int(newfitness[s].first))
				+ ", AVR="
				+ to_string(int(newfitness[s].second))
				+ star);
			text.setCharacterSize(12);
			text.setFillColor(speciesColours[s]);
			text.setPosition(Vector2f(10, 70 + 18 * s));

			w->draw(text);
		}

	}

	Visualisation(std::shared_ptr<Pool> pool) : pool(pool) {

		if (!font.loadFromFile("font.ttf")) {
			cout << "ERROR : Font file 'font.ttf' not found." << endl;
			throw 404;
		}

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
			else if (abs(ptype - P_SAME_SPECIES) < 0.1) {
				pcolor = sf::Color(0, 50, 250);
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
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;

		window = std::make_shared<Window *>(new sf::RenderWindow(sf::VideoMode(windowWidth, windowHeight), "SFML works!", sf::Style::Default, settings));
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
