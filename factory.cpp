#include "factory.h"
#include <iostream>
#include "game.h"
#include "infantry.h"

Factory::Factory(int x, int y, char c) : Building(x,y,c)
{

}

void Factory::interactWith()
{
	std::cout<<"This is a factory, you can build units here."<<std::endl;
	//Unit* un = Game::getInstance()->addUnit(this->posX,this->posY,this->owner);
	//Game::getInstance()->pay(un->getPrice(), this->owner);

	//std::cout<<"Creating unit"<<std::endl;

}

void Factory::buy(int type)
{
	std::cout<<"Buying unit"<<std::endl;
	bool can_build = true;
	std::vector<GameObject*>& tile = Game::getInstance()->getObjectsOnPos(this->posX,this->posY);
	for(unsigned int i=0; i<tile.size();i++){
		if(tile[i]->getType().find("Unit") != std::string::npos){
			can_build = false;
			break;
		}
	}
	if(can_build){
		Unit* un = NULL;
		switch(type){
			case 1:
				un = new Infantry(this->posX, this->posY, 0, this->owner); //infantry
				break;
			case 2:
				un = new Infantry(this->posX, this->posY, 1, this->owner); //bazooka
				break;
			case 3:
				un = new Unit(this->posX, this->posY, 2, this->owner); //recon
				break;
			case 4:
				un = new Unit(this->posX, this->posY, 3, this->owner); //anti-air
				break;
			case 5:
				un = new Unit(this->posX, this->posY, 4, this->owner); //tank
				break;
			case 6:
				un = new Unit(this->posX, this->posY, 5, this->owner); //md. tank
				break;
			case 7:
				un = new Unit(this->posX, this->posY, 6, this->owner); //mega tank
				break;
			case 8:
				un = new Unit(this->posX, this->posY, 7, this->owner); //neotank
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
				//delete un; //causes segfault, idk why
			}
		}
	}
}

std::string Factory::getType() const
{
	return "Factory";
}

