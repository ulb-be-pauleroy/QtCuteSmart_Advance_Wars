#ifndef VALIDMOVE_H
#define VALIDMOVE_H

#include "gameobject.h"

class ValidMove : public GameObject
{
public:
	ValidMove(int x, int y);
	bool operator==(GameObject &) const;
	std::string getType() const;
	void interactWith();
};

#endif // VALIDMOVE_H
