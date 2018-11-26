#include "infantry.h"
#include <iostream>

Infantry::Infantry(int x, int y, int type, char team) : Unit(x,y, type, team) // TODO
{
	this->capturing = NULL;
}

std::string Infantry::getType() const
{
	return "InfantryUnit";
}

bool Infantry::move(int dir, int terrainMod)
{
    //std::cout<<"Moving "<<this->capturing<<std::endl;
	bool ok = Unit::move(dir, terrainMod);
    if(this->capturing != 0 && ok){
        this->capturing->abandonCapture();
		this->capturing = NULL;
    }
    return ok;
}

void Infantry::setCapture(City * city)
{
    this->capturing = city;
}

bool Infantry::isCapturing()
{
    return this->capturing;
}
