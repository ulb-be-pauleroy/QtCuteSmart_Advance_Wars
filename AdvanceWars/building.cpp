#include "building.h"
#include "game.h"
#include <iostream>
#include <QDebug>

Building::Building(int x, int y, char c) : GameObject(x,y)
{
	this->owner = c;
    this->health = 20;
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

void Building::capture(Unit *un)
{
    if(un->getUnitType() == 0 && un->getTeam() != this->owner){
        this->health -= un->getHealth(); //I think there is a health limit
        un->setCapture(this);
        std::cout<<"capturing "<<this->health<<" health left"<<std::endl;
        if(this->health <=0){
            qDebug() << "this bulding got captured!";
            switch(un->getTeam()){
                case 'o': this->owner ='o'; break;
                case 'b': this->owner ='b'; break;
                // 'o' is char "o" is string
            }
        }
    }
}

void Building::abandonCapture()
{
    this->health = 20;
}

