
#include "Visualisation.h"
#include "Pool.hpp"

int main() {


	// create the window
	int foodCount = 20;
	Visualisation visual;
	Pool pool(
		1,
		vector<int>({ 40,40 }),
		vector<int>({ PERCEPTION_NUMBER * 3 + 1, 5, 5, 2 })
	);

	while (true) {

		World world;
		world.clearFood();
		world.clearOrgs();
		for (auto & s : pool.species) {
			for (auto & genome : s) {
				world.addOrg(Organism(genome));
			}
		}
		for (int i = 0; i < foodCount; i++) {
			world.addFood();
		}
		while (world.step()) {
			if (*visual.fastMode) {

				cout << "rendering" << endl;
				visual.render(world);

			}
			if (visual.exit != NULL && *visual.exit) break;
		}

		cout << "Generateion:" << pool.generation << endl;
		for (auto & fitness : pool.getFitness()) {
			cout << "f max:" << fitness.first << "\t\tf avr:" << fitness.second<<",\t";
		}
		cout << endl;
		pool.nextGeneration();


		if (visual.exit != NULL && *visual.exit) break;
	}
	return 0;
}