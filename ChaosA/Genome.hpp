#pragma once
#ifndef _GENOME
#define _GENOME
#include "Shared_Header.hpp"
#include "Organism.hpp"

class Genome
{
public:


	floatBase fitness;

	vector<floatBase> bodyEncoding;
	vector<floatBase> neuralNetworkEncoding;
	vector<int> neuralStruct;
	int speciesID;

	Genome()
	{
		bodyEncoding = (vector<floatBase>(2));
		bodyEncoding[SIZE] = SIZE_BASE;
		bodyEncoding[FLEXIBILITY] = PI / 4;
	};
	Genome(const vector<int>& neuralStruct) {
		bodyEncoding = (vector<floatBase>(2));
		bodyEncoding[SIZE] = SIZE_BASE;
		bodyEncoding[FLEXIBILITY] = PI / 4;
		this->neuralStruct = neuralStruct;
		init(neuralStruct);
	}
	Genome(const Genome& genome) {
		this->fitness = genome.fitness;
		this->bodyEncoding = genome.bodyEncoding;
		this->neuralNetworkEncoding = genome.neuralNetworkEncoding;
		this->neuralStruct = genome.neuralStruct;
		this->speciesID = genome.speciesID;
	};
	~Genome() {
	};

private:
	inline void init(const vector<int> & networkStruct) {

		using namespace std;
		int size = 0;
		for (int i = 0; i < networkStruct.size() - 1; i++) {
			int input_n = networkStruct[i];
			int output_n = networkStruct[i + 1];
			int matrixSize = input_n*output_n;
			size += matrixSize;
		}
		neuralNetworkEncoding = vector<floatBase>(size, 0);

		// First create an instance of an engine.
		random_device rnd_device;
		// Specify the engine and distribution.
		mt19937 mersenne_engine(rnd_device());
		int in = networkStruct[0];
		int out = networkStruct[networkStruct.size() - 1];
		uniform_real_distribution<floatBase> dist(-2 / (in + out), 2*(in + out));
		auto gen = std::bind(dist, mersenne_engine);

		generate(begin(neuralNetworkEncoding), end(neuralNetworkEncoding), gen);
	}
};
#endif // !GENOME
