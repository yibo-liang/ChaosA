#include "World.h"



World::World()
{
	worldEdges.push_back(vline(0, 0, width-0, 0)); //top edge
	worldEdges.push_back(vline(0, height - 0, width - 0, height - 0)); //bottom edge
	worldEdges.push_back(vline(0, 0, 0, height - 0)); //left edge
	worldEdges.push_back(vline(width - 0, 0, width - 0, height - 0)); //right edge
}


World::~World()
{
}
