#pragma once
#ifndef NEURAL_NET
#define NEURAL_NET
#include "Shared_Header.h"
#include "NeuralLayer.h"

class Network
{
public:

	vector<NeuralLayer> layers;
	void activate(vector<floatBase> perception);

	Network(const Network& network);
	Network();
	~Network();

private:

};
#endif // !NEURAL_NET
