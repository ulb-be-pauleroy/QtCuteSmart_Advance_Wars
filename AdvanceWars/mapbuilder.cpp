#include "mapbuilder.h"

#include "game.h"
#include "terrain.h"
#include "city.h"
#include "factory.h"
#include "airport.h"

#include <QFile>
#include <QTextStream>

using namespace std;

MapBuilder::MapBuilder()
{

}

std::vector<std::vector<int> > MapBuilder::makeIntMap(QString path)
{
	QFile file(path);
	file.open(QIODevice::ReadOnly);

	QString s;
	QTextStream s1(&file);
	s=s1.readAll();
	QStringList list1 = s.split(QRegExp("\n|\r\n|\r"));
	vector<QStringList> list2;
	for(int i=0;i<list1.size();i++){
		QString x = list1[i];
		list2.push_back(x.split(","));
	}
	unsigned int x = list2.size();
	unsigned int y = list2[0].size();
	//qDebug() << x; qDebug() << y ;
	vector<vector<int> > list3(y, vector<int>(x,0));         // watch out here, risk to swap x and y axis
	for (unsigned int i = 0; i < y; i++){ // in order to avoid that, list2[i][j] must become list3[j][i]
		for (unsigned int j = 0; j < x; j++){

			list3[i][j] = list2[j][i].toInt();
			verifyNumber(list3[i][j]);
		}

	}
	file.close();
	return list3;

}

void MapBuilder::verifyNumber(int& index)
{
	if (index >= 48 && index <= 52){ //green to orange
		index -= 10;
	}
	else if (index >= 81 && index <= 85){ //red to orange
		index -= 43;
	}
	else if (index >= 91 && index <= 95){ //black to orange
		index -= 53;
	}
	else if (index >= 117 && index <= 121){ //amber to orange
		index -= 79;
	}
	else if (index >= 53 && index <= 57){ //yellow to blue
		index -= 10;
	}
	else if (index >= 86 && index <= 90){ //brown to blue
		index -= 43;
	}
	else if (index >= 96 && index <= 100){ //grey to blue
		index -= 53;
	}
	else if (index >= 122 && index <= 126){ //jade to blue
		index -= 79;
	}

	if (index  == 37 || index ==  133 || index == 145){ // no port, HQ, Comtower or lab
		index = 34;
	}
	else if (index  == 41 || index == 42 || index == 134 || index == 146){// no port, HQ, Comtower or lab
		index = 38;
	}
	else if (index  == 46 || index == 47 || index == 129 || index == 140){// no port, HQ, Comtower or lab
		index = 43;
	}
	else if (index  == 111 || index == 112 || index == 115 || index == 116){// no missile silo and rubbles
		index = 0;
	}

}

void MapBuilder::buildTerrainMap(Game* game)
{
	vector<vector<int> > intMap = game->getIntMap();
	unsigned int x = intMap.size();
	unsigned int y = intMap[0].size();
	for(unsigned int i = 0; i != x; i++ ){
		for (unsigned int j = 0; j != y; j++){
			switch(intMap[i][j]){
			case 1 : game->addGameObject(new Terrain(i,j,0), i, j) ; break;
			case 2 : game->addGameObject(new Terrain(i,j,1), i, j) ; break;
			case 3 : game->addGameObject(new Terrain(i,j,2), i, j) ; break;
			case 4 ... 14 :  game->addGameObject(new Terrain(i,j,3), i, j) ; break;
			case 15 ... 25:  game->addGameObject(new Terrain(i,j,4), i, j) ; break;
			case 26: case 27:  game->addGameObject(new Terrain(i,j,5), i, j) ; break;
			case 28:  game->addGameObject(new Terrain(i,j,6), i, j) ; break;
			 case 29: case 30: case 31: case 32 :  game->addGameObject(new Terrain(i,j,7), i, j) ; break;
			 case 33 : game->addGameObject(new Terrain(i,j,8), i, j) ; break;
			 case 34 : game->addGameObject(new City(i,j,'\0'), i, j , '\0') ; break;
			 case 35 : game->addGameObject(new Factory(i,j,'\0'), i, j , '\0') ; break;
			 case 36 : game->addGameObject(new Airport(i,j,'\0'), i, j , '\0') ; break;
			 case 37 : game->addGameObject(new City(i,j,'\0'), i, j , '\0') ; break;
			 case 38 : game->addGameObject(new City(i,j,'o'), i, j , 'o' ) ; break;
			 case 39 : game->addGameObject(new Factory(i,j,'o'), i, j , 'o') ; break;
			 case 40 : game->addGameObject(new Airport(i,j,'o'), i, j , 'o') ; break;
			 case 41: case 42 :game->addGameObject(new City(i,j,'o'), i, j , 'o') ; break;
			 case 43 : game->addGameObject(new City(i,j,'b'), i, j , 'b') ; break;
			 case 44 : game->addGameObject(new Factory(i,j,'b'), i, j , 'b') ; break;
			 case 45 : game->addGameObject(new Airport(i,j,'b'), i, j , 'b') ; break;
			 case 46: case 47 : game->addGameObject(new City(i,j,'b'), i, j , 'b') ; break;
			 case 101: case 102: case 103: case 104: case 105:
			 case 106: case 107: case 108: case 109: case 110 :  game->addGameObject(new Terrain(i,j,14), i, j) ; break;
			 case 111: case 112 : game->addGameObject(new Terrain(i,j,0), i, j) ; break;
			 case 113: case 114 : game->addGameObject(new Terrain(i,j,14), i, j) ; break;
			 case 115: case 116 : game->addGameObject(new Terrain(i,j,0), i, j) ; break;
			 case 129 : game->addGameObject(new City(i,j,'b'), i, j , 'b') ; break;
			 case 133 : game->addGameObject(new City(i,j,'\0'), i, j , '\0') ; break;
			 case 134 : game->addGameObject(new City(i,j,'o'), i, j , 'o') ; break;
			 case 140 : game->addGameObject(new City(i,j,'b'), i, j , 'b') ; break;
			 case 145 : game->addGameObject(new City(i,j,'\0'), i, j , '\0') ; break;
			 case 146 : game->addGameObject(new City(i,j,'o'), i, j , 'o') ; break;

			 }
		}
	}

}

