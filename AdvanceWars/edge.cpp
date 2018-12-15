#include <iostream>
#include "edge.h"

Edge::Edge(int t, int d)
{
    this->to = t;
	this->dist = d;
}

int Edge::getDist() const
{
	return this->dist;
}

int Edge::getTo() const
{
    return this->to;
}

