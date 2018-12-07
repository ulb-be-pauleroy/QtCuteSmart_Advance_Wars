#include "airunit.h"

AirUnit::AirUnit(int x, int y, int type, char tm) : Unit(x,y,type, tm)
{

}

std::string AirUnit::getType() const
{
	return "AirUnit";
}

