#pragma once
#ifndef _NEURAL_NODE
#define _NEURAL_NODE
#include "Shared_Header.h"
class NeuralNode
{
public:

	uint id;
	vector<floatBase> weights;
	floatBase historyValue;
	floatBase currentValue;

	CellType type;

	void activate(const vector<NeuralNode>& nodes);

	NeuralNode(const NeuralNode& node);
	NeuralNode();
	~NeuralNode();

private:

};

#endif // !NEURAL_NODE
