#pragma once
#ifndef POOL_
#define POOL_
#include "Shared_Header.hpp"
#include "Genome.hpp"
#include "World.h"

using Species = vector<Genome>;
class Pool
{
public:

	vector<int> neuralStruct;


	vector<Species> species;

	bool elitism = false;
	int speciesCount;
	int tournamentSize = 2;
	vector<int> speciesSizes;
	floatBase crossoverRate = 0.3;
	floatBase strongMutationRate = 0.05;
	floatBase pertubationRate = 0.3;
	floatBase pertubationRatio = 1.2;

	int generation = 0;
	vector<std::pair<floatBase, floatBase>> getFitness() {
		vector<std::pair<floatBase, floatBase>> result;
		for (int i = 0; i < species.size(); i++) {
			floatBase max = 0;
			floatBase sum = 0;
			for (int k = 0; k < species.at(i).size(); k++) {
				if (species.at(i).at(k).fitness > max) {
					max = species.at(i).at(k).fitness;
				}
				sum += species.at(i).at(k).fitness;
			}
			floatBase avr = sum / (floatBase)species.at(i).size();
			result.push_back(std::pair<floatBase, floatBase>(max, avr));
		}
		return result;
	}

	void init(int speciesCount, vector<int> & speciesSizes, vector<int> & neuralStruct) {
		this->speciesCount = speciesCount;
		this->speciesSizes = speciesSizes;
		this->neuralStruct = neuralStruct;

		for (int i = 0; i < speciesCount; i++) {
			vector<Genome> sp;
			for (int s = 0; s < speciesSizes[i]; s++) {
				Genome g(neuralStruct);
				g.speciesID = i;
				sp.push_back(g);
			}
			species.push_back(sp);
		}

	};
	void nextGeneration() {
		vector<Species> newGen;
		int c = 0; //counter
		cout << "Next Gen:" << endl;
		for (int i = 0; i < species.size(); i++) {
			Species & s = species[i];
			Species newS;
			//cout << "species " << i << endl;
			for (int k = 0; k < s.size(); k++) {
				if (elitism && k == 0) {
					floatBase bestfit = 0;
					int bid;
					for (int b = 0; b < s.size(); b++) {
						if (s[b].fitness > bestfit) {
							bid = b;
							bestfit = s[b].fitness;
						}
					}
					//cout << "push fitness=" << s[bid].fitness << endl;
					newS.push_back(s[bid]);
					continue;
				}
				if (k < s.size() * 0.5) {
					Genome A = tournament(s, tournamentSize);
					Genome C = A;
					if (get_random() < crossoverRate)
					{
						Genome B = tournament(s, tournamentSize);
						C = (crossover(A, B));
					}

					mutation(C);

					//cout << "push fitness=" << C.fitness << endl;
					newS.push_back(C);
				}
				else {
					Genome & A = tournament(s, tournamentSize);

					//cout << "push fitness=" << A.fitness << endl;
					newS.push_back(A);
				}
			}
			newGen.push_back(newS);
		}
		species = newGen;
		generation++;
	};

	Pool(int speciesCount, vector<int> & speciesSizes, vector<int> & neuralStruct) {
		init(speciesCount, speciesSizes, neuralStruct);
	}
	Pool() {};
	~Pool() {};



private:

	Genome tournament(const vector<Genome> & genomes, int k) {
		int id_a = genomes.size()*get_random();
		Genome A = genomes[id_a];
		for (int i = 0; i < k - 1; i++) {
			const Genome & B = genomes[genomes.size()*get_random()];
			if (A.fitness < B.fitness) {
				A = B;
			}
		}
		return A;
	}

	inline floatBase crossover_float(floatBase f1, floatBase f2) {
		/* Cross over between f1 and f2, is a random number between f1-0.1df and f2+0.1df */
		/*floatBase min = f1 < f2 ? f1 : f2;
		floatBase max = f1 > f2 ? f1 : f2;
		cout << "cf:" << min << "," << max << "=>";
		floatBase df = max - min;
		min = min - df * 0.1;
		df = df * 1.2;
		floatBase r = get_random() * df + min;
		cout << r << endl;
		return r;
*/
		return get_random() > 0.5 ? f1 : f2;
	}

	inline void crossover_NN(Genome & result, const Genome & g2) {
		floatBase cLayerRate = 0.01;
		floatBase cNeuronRate = 0.04;
		floatBase cWeightRate = 0.8;
		if (get_random() < cLayerRate) {
			vector<int> & netstruct = result.neuralStruct;
			//select weight matrix between random two adjecent layers
			int l = (netstruct.size() - 1) * get_random(); //layer number, weight matrix between l and l+1 will be copied

			int pointer_pos = 0;
			//skip layer matrix before selected
			for (int i = 0; i < l; i++) {
				int input_n = netstruct[i];
				int output_n = netstruct[i + 1];
				pointer_pos += input_n*output_n;
			}
			int weightMatrix_size = netstruct[l] * netstruct[l + 1];

			for (int i = 0; i < weightMatrix_size; i++) {
				result.neuralNetworkEncoding[pointer_pos + i] = g2.neuralNetworkEncoding[pointer_pos + i];
			}

		}
		if (get_random() < cNeuronRate) {
			vector<int> & netstruct = result.neuralStruct;
			//select weight matrix between random two adjecent layers
			int l = (netstruct.size() - 1) * get_random() + 1;//layer number, start from first non-input layer 
			int n = netstruct[l] * get_random(); //neuron number of the layer


			int pointer_pos = 0;
			//skip layer matrix before this layer
			for (int i = 0; i < l - 1; i++) {
				int input_n = netstruct[i];
				int output_n = netstruct[i + 1];
				pointer_pos += input_n*output_n;
			}
			int input_n = netstruct[l - 1];
			//skip neuron weights before this neuron on the same layer
			for (int i = 0; i < n; i++) {
				pointer_pos += input_n;
			}

			for (int i = 0; i < input_n; i++) {
				result.neuralNetworkEncoding[pointer_pos + i] = g2.neuralNetworkEncoding[pointer_pos + i];
			}


		}
		if (get_random() < cWeightRate) {
			int w = result.neuralNetworkEncoding.size() * get_random();
			result.neuralNetworkEncoding[w] = g2.neuralNetworkEncoding[w];
		}

	}

	Genome crossover(const Genome & g1, const Genome & g2) {
		Genome result(g1);
		for (int i = 0; i < g1.bodyEncoding.size(); i++) {
			result.bodyEncoding[i] = crossover_float(g1.bodyEncoding[i], g2.bodyEncoding[i]);
		}
		crossover_NN(result, g2);
		return result;
	}

	inline floatBase strongMutate(floatBase a) {

		if (get_random() < strongMutationRate) {
			if (get_random() < 0.5) {
				return -a;
			}
			else {
				floatBase min = -abs(a)*1.1;
				floatBase d = abs(a) * 2 * 1.1;
				return min + d*get_random();
			}
		}
		return a;
	}

	inline floatBase pertubate(floatBase a) {

		if (get_random() < pertubationRate) {
			floatBase p = pertubationRatio;
			floatBase min = a - p;
			return min + p * 2 * get_random();
		}
		return a;

	}

	inline floatBase mutate(floatBase f) {
		f = pertubate(f);
		f = strongMutate(f);

		return f;
	}

	void mutation(Genome & g) {
		//g.bodyEncoding[SIZE] = pertubate(g.bodyEncoding[SIZE]);
		//if (g.bodyEncoding[SIZE] < MIN_SIZE) g.bodyEncoding[SIZE] = MIN_SIZE;

		for (int i = 0; i < g.neuralNetworkEncoding.size(); i++) {
			g.neuralNetworkEncoding[i] = mutate(g.neuralNetworkEncoding[i]);
		}
	}


};
#endif // !POOL_
