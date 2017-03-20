#pragma once
#ifndef _ORGANISM
#define _ORGANISM
#include "Shared_Header.h"
#include "IObject.h"
#include "NeuralNetwork.h"

class Organism : IObject
{
public:
	
	floatBase hunger;
	floatBase acceleration;
	floatBase direction;
	vector<floatBase> perception;
	bool alive;

	Network neuralNetwork;

	floatBase vision;
	floatBase maxSpeed;
	floatBase agility;

	const Genome & genome;

	Organism(const Genome& genome) : genome(genome) {

	}
	~Organism() {

	}

private:

};

#endif // !ORGANISM
