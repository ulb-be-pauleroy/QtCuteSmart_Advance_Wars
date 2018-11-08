#ifndef FACTORY_H
#define FACTORY_H

#include "building.h"

class Factory : public Building
{
public:
	Factory(int x, int y, char c);
	virtual void interactWith();
	virtual void buy(int type);
	virtual std::string getType() const;
};

#endif // FACTORY_H
