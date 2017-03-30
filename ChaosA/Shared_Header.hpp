#pragma once

/*

Cells {
	states:
		hunger
		running speed
		direction // running on direction
		perception{
			36 * {
				(0 - max vision) object distance
				(0 / 1 / 2) object [neutral, same species, or different species]
			}
		}
		alive (true/false)


	property:
		vision
		size
		speed => max speed
		agility => max turing direction angle
		neural network
}

neural network {

}

main {
	init pool{
		load config file => set parameters
		create initial genomes
	}

	run pool{

		evaluate(){
			while(some cell alive && time smaller than ){
				generate foods
				foreach cell{
					if not alive: continue
					run cell(){
						percept
						run neural network()
						act according to output
					}
				}
				foreach cell
					if not alive: continue
					update status{
						hunger -= speed^2 * base
						getfood(cell){
							if (food//smaller different species cell) is nearby,{
								consume food, hunger += value
								if cell, mark : alive = false
							}
						}
					}
				}
				if (visualise){
					visualise()
				}
			}
		}

		selection()
		crossover()
		mutation()

	}



}

*/

#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>
#include <time.h>
#include <iostream>
#include <functional>

#define ID_INTERFACE -1
#define ID_ORGANISM 0
#define ID_FOOD 1

#define FRAME_RATE 30
#define SIZE 0
#define FLEXIBILITY 1

using std::vector;
using std::string;
using uint = uint32_t;
using floatBase = double;
using std::cout;
using std::cin;
using std::endl;

enum CellType
{
	input, output, bias, hidden
};

#define MIN_SIZE 1.0
#define SIZE_BASE 10.0
#define SIZE_VISION_CORRELATION 2.50
#define VISION_BASE 10.0
#define SPEED_BASE 15.0
#define PERCEPTION_NUMBER 18
#define INIT_HUNGER 100.0

#define PI std::_Pi
#ifndef MY_FUNCS
#define MY_FUNCS

inline floatBase speedFormula(floatBase size) {
	return pow(size, (-0.3)) * 2 * SPEED_BASE;
}

inline floatBase get_random()
{
	static std::default_random_engine e;

	static std::uniform_real_distribution<floatBase> dis(0, 1); // rage 0 - 1
	floatBase r = dis(e);
	return r;
}

inline floatBase hungerFormula(floatBase size, floatBase speed, floatBase turning) {
	speed = abs(speed);
	turning = abs(turning);
	floatBase base = (size / SIZE_BASE);
	return  1.5;
}
#endif // !MY_FUNCS

