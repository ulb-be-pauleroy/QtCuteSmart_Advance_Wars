#ifndef CITY_H
#define CITY_H

#include "building.h"
#include "unit.h"

class City : public Building
{
    int health;
public:
	City(int x, int y, char c = '\0');
	//void select();
	void interactWith();
	std::string getType() const;
    void capture(Unit*);
    void abandonCapture();
};

#endif // CITY_H
