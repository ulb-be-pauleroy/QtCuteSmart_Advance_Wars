#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <QString>
//#include "network.h"
#include "gameobject.h"
#include "validmove.h"
#include "unit.h"
#include "factory.h"
#include "building.h"
#include "mapsize.h" // map size preprocessor value defined here
class Network;

class Game{

    std::vector<std::vector<int> > intMap;
    std::vector<std::vector<std::vector<GameObject *> > > map;
    //std::vector<GameObject*> map[XDIM][YDIM];
	int income;
	std::vector<Unit*> units_orange;
	int money_orange;
	std::vector<Unit*> units_blue;
	int money_blue;
	std::vector<Building*> buildings;
    bool orange_on_turn;
    int selected_x;
    int selected_y;
    Unit* selected_unit;
	Factory* selected_factory;
	bool attacking;
	Network* network;
	static Game* instance;
    Game(bool isHost);

public:

    static Game* getInstance(bool isHost=true);
    void setPath(QString path);
    void makeIntMap(QString path);
    void verifyNumber(int& index);
    std::vector<std::vector<int> > & getIntMap();
	void recieveNetwork(Network* net);
	void setIncome(int inc);
	int getIncome();
	void networkAction(std::string type, int x, int y, int data, int data2);
	std::vector<GameObject*>* getMapContent(); //network only
	std::vector<GameObject*> & getObjectsOnPos(int x, int y);// const;
	int getTerrainMovementModifier(Unit* un, int x, int y);// const;
	int getTerrainDefenseModifier(Unit& un, int x, int y);// const;
	int getBalance(char pl) const;
    std::vector<Unit *>& getUnits_blue();
    std::vector<Unit *> &getUnits_orange();
	void pay(int qnt, char player);
	GameObject* addGameObject(GameObject* go, int x, int y, const char team='\0');
	Unit* addUnit(Unit* un, int x, int y, const char team, bool net=false);
	void move(int dir, bool net=false);
	void moveTo(int x, int y);
	void setAttack();
	void attack(int dir);
	void click(int x, int y);
	void buyUnit(int type);
	void selectUnit(Unit*);
	void cycleUnits(int dir); //-1 and 1 (backwards/forwards)
    void deleteUnit(Unit* un);
    int getSelectedX();
    int getSelectedY();
	char getTeamOnTurn() const;
	void endTurn(bool net=false);
    void buildTerrainMap();

private:
	bool testObstacle(int x, int y);
	void clearValidMoves();
    void drawPossibleMoves();
	void testCaptureAndHealing(Unit*);
	bool testEndOfGame();
};

#endif // GAME_H
