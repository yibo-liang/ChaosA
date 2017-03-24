#pragma once
#ifndef _ORGANISM
#define _ORGANISM
#include "Shared_Header.hpp"
#include "IObject.hpp"
#include "NeuralNetwork.hpp"
#include "Genome.hpp"

class Organism : public IObject
{
public:
	//runtime status
	floatBase hunger;
	floatBase acceleration; // maximum = maxSpeed / t
	floatBase direction;
	vector<floatBase> perception;
	bool alive;

	Network neuralNetwork;

	//computed properties
	floatBase vision; //vision equals to size * sv_correlation + visionBase
	floatBase maxSpeed; // f(size) = formula(size)
	//encoded properties
	floatBase agility; //max speed at turning around
	//floatBase size; //Already defined in IObject

	int speciesID;

	const Genome & genome;

	Organism(const Organism& o) : genome(o.genome) {
		this->hunger = o.hunger;
		this->acceleration = o.acceleration;
		this->direction = o.direction;
		this->perception = o.perception;
		this->alive = o.alive;

		this->neuralNetwork = Network(o.neuralNetwork);
		this->vision = o.vision;
		this->maxSpeed = o.maxSpeed;
		this->agility = o.agility;

		this->speciesID = o.speciesID
	}

	Organism(const Genome& genome) : genome(genome) {
		this->neuralNetwork = Network(genome.neuralStruct, genome.neuralNetworkEncoding);
		this->size = genome.bodyEncoding(SIZE);
		this->agility = genome.bodyEncoding(FLEXIBILITY);
		this->vision = size * SIZE_VISION_CORRELATION + VISION_BASE;
		this->maxSpeed = speedFormula(size);
	}
	~Organism() {

	}

private:

};

#endif // !ORGANISM
