#pragma once
#ifndef FOOD
#define FOOD
#include "Shared_Header.hpp"
#include "IObject.hpp"

class Food : public IObject
{
	using IObject::getTypeID;
public:
	int getTypeID() const {
		//cout << "get food id" << endl;
		return ID_FOOD;
	}
	Food(const Food & food) {
		this->x = food.x;
		this->y = food.y;
		this->exist = food.exist;
		setSize(food.getSize());
	}
	Food() {
		setSize(get_random() * SIZE_BASE * 1);
	};
	~Food() {};

private:

};
#endif // !FOOD
