#pragma once
#ifndef FOOD
#define FOOD
#include "Shared_Header.hpp"
#include "IObject.hpp"

class Food : public IObject
{
public:
	Food() {
		size = get_random() * SIZE_BASE;
	};
	~Food() {};

private:

};
#endif // !FOOD
