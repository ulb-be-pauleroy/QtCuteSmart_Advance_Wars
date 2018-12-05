#include <iostream>
#include "edge.h"

Edge::Edge(int t, int d)
{
    //std::cout << "edge c "<< t <<std::endl;
    this->to = t;
	this->dist = d;
}

int Edge::getDist() const
{
	return this->dist;
}

int Edge::getTo() const
{
    //std::cout << "edge "<< this->to <<std::endl;
    return this->to;
}

