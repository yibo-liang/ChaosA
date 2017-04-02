
#include "Visualisation.h"
#include "Pool.hpp"

int main() {


	// create the window
	int foodCount = 80;
	int speciesCount = 3;
	vector<int> neuralNetStruct({ PERCEPTION_NUMBER * 3 + 2, 4, 4, 2 });
	vector<Genome> prototypes(speciesCount, Genome(neuralNetStruct));
	prototypes[0].startGeneration = 0;
	prototypes[0].bodyEncoding[FLEXIBILITY] = PI / 1.25;
	prototypes[1].startGeneration = 0;
	prototypes[1].bodyEncoding[SIZE] = SIZE_BASE * 3.5;
	prototypes[1].bodyEncoding[FLEXIBILITY] = PI / 2;
	prototypes[2].startGeneration = 0;
	prototypes[2].bodyEncoding[SIZE] = SIZE_BASE * 5;
	prototypes[2].bodyEncoding[FLEXIBILITY] = PI / 3;

	std::shared_ptr<Pool> poolPtr = std::make_shared<Pool>(
		speciesCount,
		vector<int>({ 80, 35, 15 }),
		prototypes
	);

	Pool & pool = *(poolPtr.get());

	Visualisation visual(poolPtr);
	while (true) {

		World world;
		world.regenerateFood = 100;
		world.clearFood();
		world.clearOrgs();
		for (auto & s : pool.species) {
			for (auto & genome : s) {
				if (genome.startGeneration <= pool.generation) {
					world.addOrg(Organism(genome));
				}
			}
		}
		world.initGrid();
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
			visual.renderFitness(pool.getFitness(), world);
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