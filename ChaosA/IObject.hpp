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
	floatBase size;
	floatBase getRadius() {
		return sqrt(size/ 2 / std::_Pi);
	}
	
private:

};



#endif // IOBJ
