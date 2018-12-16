#include "airport.h"
#include <iostream>
#include "game.h"
#include "airunit.h"

Airport::Airport(int x, int y, char c) : Factory(x,y,c)
{

}

void Airport::interactWith()
{
	std::cout<<"This is an airport, you can build planes here."<<std::endl;
}

std::string Airport::getType() const
{
	return "Airport";
}

void Airport::buy(int type)
{
	std::cout<<"Buying unit"<<std::endl;
	Unit* un = NULL;
	switch(type){
		//9, 10 and 11 for AI
		case 1: case 9:
			un = new AirUnit(this->posX, this->posY, 8, this->owner); //b-copter
			break;
		case 2: case 10:
			un = new AirUnit(this->posX, this->posY, 9, this->owner); //fighter
			break;
		case 3: case 11:
			un = new AirUnit(this->posX, this->posY, 10, this->owner); //bomber
			break;
		default:
			std::cout<<"Error: No such unit."<<std::endl;
	}
	if(un != NULL){
		int price = un->getPrice();
		if(price <= Game::getInstance()->getBalance(this->owner)){
			Game::getInstance()->addUnit(un, this->posX,this->posY,this->owner);
			un->endTurn();
			Game::getInstance()->pay(un->getPrice(), this->owner);
		}else{
			std::cout<<"Not enough money"<<std::endl;
			delete un; //causes segfault, idk why
		}
	}
}

void Airport::healUnit(Unit * un)
{
	if(un->getType() == "AirUnit" && un->getTeam() == this->owner && un->getHealth() != 10){
		Game::getInstance()->pay(un->getPrice()/10, un->getTeam());
		un->heal(2);
	}
}
