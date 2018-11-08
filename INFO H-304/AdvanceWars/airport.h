#ifndef AIRPORT_H
#define AIRPORT_H

#include "factory.h"

class Airport : public Factory
{
public:
	Airport(int x, int y, char c);
	void interactWith();
	std::string getType() const;
	void buy(int type);
	void healUnit(Unit*);
};

#endif // AIRPORT_H
