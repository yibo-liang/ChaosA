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

	std::shared_ptr<Window*> window;
	std::thread * eventThread;
	std::shared_ptr<bool> windowCreated;
	std::shared_ptr<bool> exit;
	std::shared_ptr<bool> fastMode;



	void render(const World & world) {
		if (!*windowCreated) return;
		Window * w = *window.get();
		w->clear();
		/*  Drawing of the world here */
		const vector<Organism> & orgs = world.getOrgs();
		for (int i = 0; i < orgs.size(); i++) {
			if (orgs.at(i).exist)
				drawOrganism(orgs.at(i), w, false);
		}
		const vector<Food> & foods = world.getFoods();
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
		eventThread = handleEvent();
		speciesColours.push_back(sf::Color(150, 50, 250));
		speciesColours.push_back(sf::Color(100, 190, 150));
		speciesColours.push_back(sf::Color(200, 130, 90));


	};
	~Visualisation() {
		//delete eventThread;
	};

private:
	Vector2f disposition;

	floatBase scale = 10;

	vector<sf::Color> speciesColours;


	void drawOrganism(const Organism & org, Window * w, bool showPerception) {
		floatBase r = org.getRadius()*scale;
		floatBase x = (org.x + disposition.x )*scale ;
		floatBase y = (org.y + disposition.y )*scale ;
		CircleShape body(r);
		body.setOutlineColor(speciesColours[org.getSpecies()]);
		body.setFillColor(sf::Color(0, 0, 0, 0));

		body.setOutlineThickness(2);	
		Vector2f pos = Vector2f(x-r, y-r);
		body.setPosition(pos);
		w->draw(body);


		if (!showPerception) return;

		for (int i = 0; i < PERCEPTION_NUMBER; i++) {
			floatBase l = org.perception[i]*scale;
			floatBase rotation = (floatBase)i / (floatBase)PERCEPTION_NUMBER * 2 * PI + org.direction;
			Vector2f p(x + l*cos(rotation), y+l*sin(rotation));
			sf::VertexArray line(sf::Lines, 2);
			line[0].position = Vector2f(x,y);
			line[1].position = p;
			if (i == 0) {
				line[0].color = sf::Color(150, 50, 250);
			}
			w->draw(line);
		}

	}

	void drawFood(const Food & org, Window * w) {
		floatBase r = org.getRadius()*scale;
		floatBase x = (org.x + disposition.x)*scale - r;
		floatBase y = (org.y + disposition.y)*scale - r;
		CircleShape body(r);
		body.setFillColor(sf::Color(0, 0, 0, 0));
		body.setOutlineColor(sf::Color(50, 250, 250));
		body.setOutlineThickness(2);

		Vector2f pos = Vector2f(x, y);
		body.setPosition(pos);
		w->draw(body);
	}


	std::thread * handleEvent() {
		return new std::thread([&] {windowEventThread(); });
	}

	void windowEventThread() {
		window = std::make_shared<Window *>(new sf::RenderWindow(sf::VideoMode(1600, 960), "SFML works!", sf::Style::Titlebar));
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
					default:
						break;
					}
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
