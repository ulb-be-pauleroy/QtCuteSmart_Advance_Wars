#ifndef AIRUNIT_H
#define AIRUNIT_H

#include "unit.h"
#include <string>

class AirUnit : public Unit
{
public:
	AirUnit(int x, int y, int type, int tm);
	std::string getType() const;
};

#endif // AIRUNIT_H
