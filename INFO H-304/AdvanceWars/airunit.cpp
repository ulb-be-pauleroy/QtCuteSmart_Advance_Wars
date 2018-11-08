#include "airunit.h"

AirUnit::AirUnit(int x, int y, int type, int tm) : Unit(x,y,type, tm)
{

}

std::string AirUnit::getType() const
{
	return "AirUnit";
}

