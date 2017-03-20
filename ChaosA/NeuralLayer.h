#pragma once
#ifndef _NEURAL_LAYER
#define _NEURAL_LAYER
#include "Shared_Header.h"
#include "NerualNode.h"
class NeuralLayer
{
public:

	vector<NeuralNode> nodes;

	void activate(const NeuralLayer & layer);
	NeuralLayer();
	~NeuralLayer();

private:

};

#endif // !_NEURAL_LAYER
