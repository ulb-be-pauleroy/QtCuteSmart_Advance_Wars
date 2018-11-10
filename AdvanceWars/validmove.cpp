#include "validmove.h"
#include <iostream>

ValidMove::ValidMove(int x, int y) : GameObject(x,y)
{
	this->color = 3;
}

bool ValidMove::operator==(GameObject & obj) const
{
	//std::cout<<"VM"<<std::endl;
	ValidMove* vm = dynamic_cast<ValidMove*>(&obj); // use pointers
	if(vm != 0){ // downcast successfull
		std::cout<<"VM "<<posX<<" "<<vm->getPosX()<<" "<<posY<<" "<<vm->getPosY()<<std::endl;
		return this->posX == vm->getPosX() && this->posY == vm->getPosY();
	}else{
		return false;
	}
}

std::string ValidMove::getType() const
{
	return "ValidMove";
}

void ValidMove::interactWith()
{
	//no action, yet
}

