#include "World.h"



World::World()
{
	int padding = 0;
	worldEdges.push_back(vline(padding, padding, width-padding, padding)); //top edge
	worldEdges.push_back(vline(padding, height - padding, width - padding, height - padding)); //bottom edge
	worldEdges.push_back(vline(padding, padding, padding, height - padding)); //left edge
	worldEdges.push_back(vline(width - padding, padding, width - padding, height - padding)); //right edge
}


World::~World()
{
}
