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

void City::capture(Unit *un)
{
	if(un->getType() == "Infantry" && un->getTeam() != this->owner){
		this->health -= un->getHealth(); //I think there is a health limit
        dynamic_cast<Infantry*>(un)->setCapture(this);
        std::cout<<"capturing "<<this->health<<" health left"<<std::endl;
        if(this->health <=0){
            switch(un->getTeam()){
                case 'o': this->owner ='o'; break;
                case 'b': this->owner ='b'; break;
                // 'o' is char "o" is string
            }
        }
    }
}

void City::abandonCapture()
{
    this->health = 20;
}

