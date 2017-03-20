#pragma once
#ifndef POOL_
#define POOL_
#include "Shared_Header.h"
#include "Genome.h"
class Poll
{
public:
	Poll();
	~Poll();

	void loadConfigFile(string filename);
	void init();
	void run();


private:
	void evaluate();
	vector<Genome> selection(vector<Genome> genomes);
	Genome crossover(Genome g1, Genome g2);
	Genome mutation(Genome g);
};
#endif // !POOL_
