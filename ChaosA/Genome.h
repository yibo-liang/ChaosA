#pragma once
#ifndef _GENOME
#define _GENOME
#include "Shared_Header.h"
#include "Organism.h"

#define SIZE 0;
#define VISION 1;
#define SPEED 2;
#define ACCELERATION 3;
#define FLEXIBILITY 4;


class Genome
{
public:

	Organism toOrganism();

	vector<floatBase> evolvingChoiceVec;
	vector<floatBase> neuralNetworkEncoding;



	Genome();
	Genome(const Genome& genome);
	~Genome();

private:

};
#endif // !GENOME
