#pragma once
#ifndef NEURAL_NET
#define NEURAL_NET
#include "Shared_Header.hpp"
struct matrix
{
	floatBase * dataPtr; //size I*O, access dataPtr[o row][i col],  is weight of (i input node) to (o output node)
	matrix(floatBase * start, int I, int O) {
		dataPtr = start;
		this->I = I;
		this->O = O;
	}
	int I;//intput size
	int O;//output size
};


inline floatBase readMatrix(matrix m, int row, int col) {
	return m.dataPtr[col + row*m.O];
}

inline void writeMatrix(matrix m, floatBase val, int row, int col) {
	m.dataPtr[col + row*m.O] = val;
}

class Network
{
public:

	vector<floatBase> dataVec;
	vector<matrix> weightMatrices;
	vector<int> networkStruct;

	const vector<floatBase>& activate(const vector<floatBase>& input) {
		vector<floatBase> result = input;
		for (int i = 0; i < this->weightMatrices.size(); i++) {
			result = activateLayer(weightMatrices[i], result);
		}
		activationResult = result;
		return activationResult;
	};
	void init() {};


	Network(const Network& network) {
		dataVec = network.dataVec;
		networkStruct = network.networkStruct;
		int ptr_offset = 0;
		weightMatrices.push_back(matrix(&dataVec[ptr_offset], networkStruct[0], networkStruct[1]));
		for (int i = 1; i < networkStruct.size(); i++) {
			int input_n = networkStruct[i];
			int output_n = networkStruct[i + 1];
			int matrixSize = input_n*output_n;
			ptr_offset += matrixSize;
			weightMatrices.push_back(matrix(&dataVec[ptr_offset], input_n, output_n));
		}
	};
	Network(const vector<int>& networkStruct) {
		//init data vector with randome
		using namespace std;

		int size = 0;
		for (int i = 0; i < networkStruct.size() - 1; i++) {
			int input_n = networkStruct[i];
			int output_n = networkStruct[i + 1];
			int matrixSize = input_n*output_n;
			size += matrixSize;
		}


		// First create an instance of an engine.
		random_device rnd_device;
		// Specify the engine and distribution.
		mt19937 mersenne_engine(rnd_device());
		uniform_real_distribution<floatBase> dist(-2, 2);
		auto gen = std::bind(dist, mersenne_engine);

		dataVec = vector<floatBase>(size, 0);
		generate(begin(dataVec), end(dataVec), gen);

		//init weight matrices
		this->networkStruct = networkStruct;
		int ptr_offset = 0;
		for (int i = 0; i < networkStruct.size() - 1; i++) {
			int input_n = networkStruct[i];
			int output_n = networkStruct[i + 1];
			weightMatrices.push_back(matrix(&dataVec[ptr_offset], input_n, output_n));

			int matrixSize = input_n*output_n;
			ptr_offset += matrixSize;
		}
	};

	Network(const vector<int>& networkStruct, const vector<floatBase>& dataVec) {
		this->dataVec = dataVec;
		//init weight matrices
		this->networkStruct = networkStruct;
		int ptr_offset = 0;
		for (int i = 0; i < networkStruct.size() - 1; i++) {
			int input_n = networkStruct[i];
			int output_n = networkStruct[i + 1];
			weightMatrices.push_back(matrix(&this->dataVec[ptr_offset], input_n, output_n));

			int matrixSize = input_n*output_n;
			ptr_offset += matrixSize;
		}
	}

	Network() {};
	~Network() {};

private:

	inline floatBase activationFunction(floatBase x) {
		return tanh(x);
	}

	vector<floatBase> activationResult;
	inline vector<floatBase> activateLayer(matrix m, vector<floatBase> vals) {
		/*if (vals.size() != m.I) {
			cout << "ERROR :: Input number does not match layer neuron number." << endl;
			throw 20;
		}*/
		vector<floatBase> result(m.O);
		for (int o = 0; o < m.O; o++) {
			floatBase sum = 0;
			for (int i = 0; i < vals.size(); i++) {
				floatBase w = readMatrix(m, o, i);
				sum += vals[i] * w;
			}
			result[o] = activationFunction(sum);
		}
		return result;
	}

};
#endif // !NEURAL_NET
