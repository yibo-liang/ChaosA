#pragma once
#ifndef _GENOME
#define _GENOME
#include "Shared_Header.hpp"
#include "Organism.hpp"



class Genome
{
public:

	Organism toOrganism() {
		return Organism(*this);
		
	};

	floatBase fitness;

	vector<floatBase> bodyEncoding;
	vector<floatBase> neuralNetworkEncoding;
	vector<int> neuralStruct;
	int speciesID;

	Genome(): bodyEncoding(vector<floatBase>(2))
	{
		bodyEncoding[SIZE] = SIZE_BASE;
		bodyEncoding[FLEXIBILITY] = SIZE_BASE;


	};
	Genome(const vector<int>& neuralStruct) {
		Genome();
		this->neuralStruct = neuralStruct; 
	}
	Genome(const Genome& genome) {
		this->bodyEncoding = genome.bodyEncoding;
		this->neuralNetworkEncoding = genome.neuralNetworkEncoding;
		this->neuralStruct = genome.neuralStruct;
	};
	~Genome() {
	};

private:

};
#endif // !GENOME
