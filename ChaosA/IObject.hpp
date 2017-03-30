#pragma once

#ifndef _IOBJ
#define _IOBJ
#include "Shared_Header.hpp"
class IObject
{
public:
	virtual ~IObject() {};
	floatBase x;                          
	floatBase y;

	bool exist = true;

	void setSize(floatBase size) {
		this->size = size;
		this->radius = sqrt(size/ 2 / std::_Pi);
	}

	const floatBase getRadius() const {
		return radius;
	}

	floatBase getSize() const {
		return size;
	}

	virtual int getTypeID() const = 0;
	
	IObject() {}

	IObject(const IObject & obj) {
		x = obj.x;
		y = obj.y;
		exist = obj.exist;
		setSize(obj.getSize());
	}

private:
	floatBase size;
	floatBase radius;
};



#endif // IOBJ
