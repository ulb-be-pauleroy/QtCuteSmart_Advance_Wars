#include "city.h"
#include "infantry.h"
#include <iostream>

City::City(int x, int y, char c) : Building(x,y,c)
{
    this->health = 20;
}

void City::interactWith()
{
	std::cout<< "This is a city, leaving a unit here will capture it."<<std::endl;
}

std::string City::getType() const
{
    return "City";
}


