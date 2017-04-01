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
	int startGeneration = 0;

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
		this->startGeneration = genome.startGeneration;
		this->fitness = genome.fitness;
		this->bodyEncoding = genome.bodyEncoding;
		this->neuralNetworkEncoding = genome.neuralNetworkEncoding;
		this->neuralStruct = genome.neuralStruct;
		this->speciesID = genome.speciesID;
	};
	~Genome() {
	};

	void randomizeNeuralEncoding() {

		using namespace std;
		// First create an instance of an engine.
		// Specify the engine and distribution.
		int in = neuralStruct[0];
		int out = neuralStruct[neuralStruct.size() - 1];
		uniform_real_distribution<floatBase> dist(-2 / (in + out), 2 * (in + out));
		auto gen = std::bind(dist, rng);

		generate(begin(neuralNetworkEncoding), end(neuralNetworkEncoding), gen);
	}
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

		randomizeNeuralEncoding();
	}


};
#endif // !GENOME
