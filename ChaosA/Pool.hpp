#pragma once
#ifndef POOL_
#define POOL_
#include "Shared_Header.hpp"
#include "Genome.hpp"
class Pool
{
public:
	
	vector<int> neuralStruct;

	void loadConfigFile(string filename);
	

	vector<Genome> genomes;
	vector<vector<Genome*>> species;

	int speciesCount;
	vector<int> speciesSizes;

	void init(int & speciesCount, vector<int> & speciesSizes, vector<int> & neuralStruct) {
		this->speciesCount = speciesCount;
		this->speciesSizes = speciesSizes;
		this->neuralStruct = neuralStruct;

		for (int i = 0; i < speciesCount; i++) {
			for (int s = 0; s < speciesSizes[i]; s++) {
				Genome g(neuralStruct);
				g.speciesID = i;
				genomes.push_back(g);
				species[i].push_back(&g);
			}
		}

	};
	void run() {
		
	};

	Pool(int & speciesCount, vector<int> & speciesSizes, vector<int> & neuralStruct) {
		init(speciesCount, speciesSizes, neuralStruct);
	}
	Pool() {};
	~Pool() {};



private:
	void evaluate();
	vector<Genome> selection(vector<Genome> genomes);
	Genome crossover(Genome g1, Genome g2);
	Genome mutation(Genome g);


};
#endif // !POOL_
