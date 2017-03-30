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
	floatBase lifespan = 0;
	floatBase foodGet = 0;

	floatBase hunger = INIT_HUNGER;
	floatBase acceleration; // maximum = maxSpeed / t
	floatBase direction;
	floatBase speed = 0;
	vector<floatBase> perception;

	Network neuralNetwork;

	//computed properties
	floatBase vision; //vision equals to size * sv_correlation + visionBase
	floatBase maxSpeed; // f(size) = formula(size)
	//encoded properties
	floatBase agility; //max speed at turning around
	//floatBase size; //Already defined in IObject


	Genome & genome;

	int getTypeID() const {
		return ID_ORGANISM;
	}

	int getSpecies() const{
		return genome.speciesID;
	}

	Organism(const Organism& o) : genome(o.genome) {
		this->lifespan = o.lifespan;
		this->hunger = o.hunger;
		this->acceleration = o.acceleration;
		this->direction = o.direction;
		this->perception = o.perception;
		this->exist = o.exist;

		this->neuralNetwork = Network(o.neuralNetwork);
		this->vision = o.vision;
		this->maxSpeed = o.maxSpeed;
		this->agility = o.agility;

		this->x = o.x;
		this->y = o.y;
		setSize(o.getSize());
	}

	Organism(Genome& g) : genome(g) {
		this->neuralNetwork = Network(genome.neuralStruct, genome.neuralNetworkEncoding);
		setSize( genome.bodyEncoding[SIZE]);
		this->agility = genome.bodyEncoding[FLEXIBILITY];
		this->vision =getRadius() * SIZE_VISION_CORRELATION + VISION_BASE;
		this->maxSpeed = speedFormula(getSize());

	}
	~Organism() {

	}

private:

};

#endif // !ORGANISM
