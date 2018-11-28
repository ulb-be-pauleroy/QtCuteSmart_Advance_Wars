#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include <QString>
#include <vector>
//#include "gameobject.h"
class Game;

class MapBuilder
{
	//static std::vector<std::vector<int> > intMap;


public:
	MapBuilder();

	static std::vector<std::vector<int> > makeIntMap(QString path);
	static void verifyNumber(int& index);
	static void buildTerrainMap(Game *game);
};

#endif // MAPBUILDER_H
