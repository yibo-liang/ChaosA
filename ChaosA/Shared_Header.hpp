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
#include <iostream>
#include <functional>

#define SIZE 0
#define FLEXIBILITY 1

using std::vector;
using std::string;
using uint = uint32_t;
using floatBase = float;
using std::cout;
using std::cin;
using std::endl;

enum CellType
{
	input, output, bias, hidden
};

#define FRAME_RATE 30
#define SIZE_BASE 10
#define SIZE_VISION_CORRELATION 5
#define VISION_BASE 10
#define SPEED_BASE 15

inline floatBase speedFormula(floatBase size) {
	return pow(size, (-0.3)) * 2 * SPEED_BASE;
}

inline floatBase get_random()
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<floatBase> dis(0, 1); // rage 0 - 1
	return dis(e);
}