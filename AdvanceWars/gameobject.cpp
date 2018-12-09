#include "gameobject.h"

GameObject::GameObject(int x, int y)
{
	this->posX = x;
	this->posY = y;
}

int GameObject::getPosX()
{
	return this->posX;
}

int GameObject::getPosY()
{
	return this->posY;
}

/*
bool GameObject::operator==(GameObject const&) const
{ //this is a test, should go abstract
	return false;
}
*/
