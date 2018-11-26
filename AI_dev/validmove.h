#ifndef VALIDMOVE_H
#define VALIDMOVE_H

#include "gameobject.h"

class ValidMove : public GameObject
{
	bool is_last_move;
public:
	ValidMove(int x, int y, bool last);
	bool isLast();
	bool operator==(GameObject &) const;
	std::string getType() const;
	void interactWith();
	int getColor();
};

#endif // VALIDMOVE_H
