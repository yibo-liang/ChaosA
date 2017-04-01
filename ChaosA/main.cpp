
#include "Visualisation.h"
#include "Pool.hpp"

int main() {


	// create the window
	int foodCount = 50;
	Visualisation visual;
	int speciesCount = 3;
	vector<int> neuralNetStruct({ PERCEPTION_NUMBER * 3 + 2, 5, 5, 2 });
	vector<Genome> prototypes(speciesCount, Genome(neuralNetStruct));
	prototypes[0].startGeneration = 0;
	prototypes[0].bodyEncoding[FLEXIBILITY] = PI / 3;
	prototypes[1].startGeneration = 1500;
	prototypes[1].bodyEncoding[SIZE] = SIZE_BASE * 1.75;
	prototypes[1].bodyEncoding[FLEXIBILITY] = PI / 4;
	prototypes[2].startGeneration = 3000;
	prototypes[2].bodyEncoding[SIZE] = SIZE_BASE * 2.5;
	prototypes[2].bodyEncoding[FLEXIBILITY] = PI / 5.5;

	Pool pool(
		speciesCount,
		vector<int>({ 70, 50, 30 }),
		prototypes
	);

	while (true) {

		World world;
		world.clearFood();
		world.clearOrgs();
		for (auto & s : pool.species) {
			for (auto & genome : s) {
				if (genome.startGeneration <= pool.generation) {
					world.addOrg(Organism(genome));
				}
			}
		}
		for (int i = 0; i < foodCount; i++) {
			world.addFood();
		}
		while (world.step(!*visual.fastMode)) {
			if (!*visual.fastMode) {
				visual.render(world);
			}
			if (visual.exit != NULL && *visual.exit) break;
		}
		if (*visual.fastMode) {
			visual.renderFitness(pool.getFitness(),world);
		}
		cout << "Generateion:" << pool.generation << endl;
		for (auto & fitness : pool.getFitness()) {
			cout << "f max:" << fitness.first << ", f avr:" << fitness.second << ",\t";
		}

		cout << endl;
		pool.nextGeneration();


		if (visual.exit != NULL && *visual.exit) break;
	}
	return 0;
}