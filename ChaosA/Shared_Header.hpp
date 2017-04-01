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


#define MIN_SIZE 1.0
#define SIZE_BASE 5
#define SIZE_VISION_CORRELATION 6
#define VISION_BASE 6.0
#define SPEED_BASE 10.0
#define PERCEPTION_NUMBER 9
#define INIT_HUNGER 100.0

#define PI std::_Pi
#define PERCEPT_RANGE std::_Pi * 2
#ifndef MY_FUNCS
#define MY_FUNCS

static std::default_random_engine e;// initialize Mersennes' twister using rd to generate the seed
static std::mt19937 rng(e());

inline floatBase speedFormula(floatBase size) {
	return  size * 0.75 + SPEED_BASE;
}

inline floatBase get_random()
{

	static std::uniform_real_distribution<floatBase> dis(0, 1); // rage 0 - 1
	floatBase r = dis(rng);
	return r;
}

inline floatBase hungerFormula(floatBase size, floatBase speed, floatBase turning) {
	speed = abs(speed);
	turning = abs(turning);
	floatBase base = (size / SIZE_BASE) / 8 + (speed) / (SPEED_BASE) / 30 + 3;
	return  3;
}

#endif // !MY_FUNCS

