#include "terrain.h"

// mov_mod[terrain][unit_type]
// 0 = plain, 1 = mountain, 2 = forest
// 3 = water1, 4 = road, 5 = bridge, 6 = water2
// 7 = coast?, 8 = coast2?, 9 = city, 10 = factory, 11 = airport
// 12+ = unknown
const int Terrain::mov_mod[17][5] = {{1,1,1,2,1},
								   {2,1,10000,10000,1},
								   {1,1,2,3,1},
								   {2,1,10000,10000,1},
								   {1,1,1,1,1},
								   {1,1,1,1,1},
								   {10000,10000,10000,10000,1},
								   {1,1,1,1,1},
								   {1,1,1,1,1},
								   {1,1,1,1,1},
								   {1,1,1,1,1},
								   {1,1,1,1,1},
								   {1,1,1,1,1},
								   {1,1,1,1,1},
								   {10000,10000,10000,10000,10000}
								  }; // 10000 = sth huge
			//TODO check for not implemented buildings

const int Terrain::def_mod[17] = {1,4,2,0,0,0,0,0,1,3,3,3,3,4,3,3,3};

Terrain::Terrain(int x, int y, int type) : GameObject(x,y)
{
	this->type = type;
}

std::string Terrain::getType() const
{
	return "Terrain";
}

void Terrain::interactWith()
{

}

int Terrain::getMovement(Unit* un) const
{
	// 0 = infantry, 1 = bazooka, 2 = recon, 3 = anti-air
	// 4 = tank, 5 = md. tank, 6 = mega tank, 7 = neotank
	// 8 = b-copter, 9 = fighter, 10 = bomber
	switch(un->getUnitType()){
		case 0: return Terrain::mov_mod[this->type][0];
		case 1: return Terrain::mov_mod[this->type][1];
		case 2: return Terrain::mov_mod[this->type][3];
		case 3: case 4: case 5: case 6: case 7: return Terrain::mov_mod[this->type][2];
		case 8: case 9: case 10: return Terrain::mov_mod[this->type][4];
		default: return 1; //for compiler hapiness
	}
}

int Terrain::getDefense() const
{
	return Terrain::def_mod[this->type];
}

