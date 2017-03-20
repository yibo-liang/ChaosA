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
	[nodes]{
		[incoming nodes]
		[weights],
		history value
		current value
		type
	}
	encode(){
		
	}
	decode(){

	}
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

using std::vector;
using std::string;
using uint = uint32_t;
using floatBase = float;

enum CellType
{
	input, output, bias, hidden
};