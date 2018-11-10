#include "building.h"
#include "game.h"

Building::Building(int x, int y, char c) : GameObject(x,y)
{
    this->color = 4;
	this->owner = c;
}
/*
void Building::assingOwner(const char c)
{
    this->owner = c;
}
*/
char Building::getOwner()
{
	return this->owner;
}

void Building::healUnit(Unit * un)
{
	if(un->getType() != "AirUnit" && un->getTeam() == this->owner && un->getHealth() != 10){
		//cost = un->getPrice()/10;
		Game::getInstance()->pay(un->getPrice()/10, un->getTeam());
		un->heal(2);
	}
}

