#ifndef TERRAIN_H
#define TERRAIN_H

#include "gameobject.h"
#include "unit.h"
#include <string>

class Terrain : public GameObject
{
	int type;

	// mov_mod[terrain][unit_type]
	// 0 = infantry, 1 = bazooka, 2 = recon, 3 = anti-air
	// 4 = tank, 5 = md. tank, 6 = mega tank, 7 = neotank
	// 8 = b-copter, 9 = fighter, 10 = bomber
	const static int mov_mod[17][5];
	const static int def_mod[17];

public:
	Terrain(int x, int y, int type);
	std::string getType() const;
	void interactWith();
	int getMovement(Unit* un) const;
	int getDefense() const;
};

#endif // TERRAIN_H
