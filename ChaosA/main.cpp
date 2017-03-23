#include "Shared_Header.h"
#include "NeuralNetwork.hpp"
int main() {

	//parameters
	vector<int> networkStructure = { 36,16,16,4 };
	Network n(networkStructure);
	n.activate(vector<floatBase>(36, 1));
	std::cout << "haha" << std::endl;

}