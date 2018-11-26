#ifndef BUILDING_H
#define BUILDING_H

#include "gameobject.h"
#include "unit.h"

class Building : public GameObject
{
protected:
    char owner;

public:
	Building(int x, int y, char c);
	//virtual void select() =0;
	//void assingOwner(const char);
    char getOwner();
	virtual void healUnit(Unit*);
};

#endif // BUILDING_H
