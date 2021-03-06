#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <QString>
#include <QTimer>
//#include "network.h"
#include "gameobject.h"
#include "validmove.h"
#include "unit.h"
#include "factory.h"
#include "building.h"
//#include "mapsize.h" // map size preprocessor value defined here
//using namespace std; //this is dangerous
class Network;
class AI;
class MainWindow;

extern int size_x;
extern int size_y;

class Game{


	std::vector<std::vector<int> > intMap;
	std::vector<std::vector<std::vector<GameObject *> > > map;
	int income;
	std::vector<Unit*> units_orange;
	int money_orange;
	std::vector<Unit*> units_blue;
	int money_blue;
	std::vector<Building*> buildings;
    bool orange_on_turn;
	bool unitFusing;
	//int selected_x;
	//int selected_y;
    Unit* selected_unit;
	Factory* selected_factory;
	bool attacking;
	Network* network;
	AI* ai[2];
	static Game* instance;
	std::vector<GameObject*> test;
    MainWindow* wn;

public:
	~Game();

	static Game* getInstance();
	void setPath();
	void setupGame(int income, bool orangeBegins, int AIcnt, int AIOption, const bool isHost=true); //created a polymorphic function, first is called
	//void setupGame(const bool isHost = true);                                  //from starting window, second is called from Network,
    void setIntMap(std::vector<std::vector<int> > & map);                      //will need to be adapted
    std::vector<std::vector<int> > &getIntMap();
    void receiveNetwork(Network* net);
	void setIncome(int inc);
	int getIncome();
	void networkAction(std::string type, int x, int y, int data, int data2);
	//std::vector<GameObject*>* getMapContent(); //network only
	std::vector<GameObject*> & getObjectsOnPos(int x, int y);// const;
	int getTerrainMovementModifier(Unit* un, int x, int y);// const;
	int getTerrainDefenseModifier(Unit& un, int x, int y);// const;
	std::vector<Unit*>* getUnits(char pl);// const;
	int getBalance(char pl) const;
	void pay(int qnt, char player);
	GameObject* addGameObject(GameObject* go, int x, int y, const char team='\0');
	Unit* addUnit(Unit* un, int x, int y, const char team, bool net=false);
	void move(int dir, bool net=false, bool justPassing=false);
	bool moveTo(int x, int y, bool net=false);
	void setAttack();
	void attack(int dir);
	void click(int x, int y);
	void buyUnit(int type);
	void selectUnit(Unit*);
	void cycleUnits(int dir); //-1 and 1 (backwards/forwards)
    void deleteUnit(Unit* un);
	//int getSelectedX();
	//int getSelectedY();
	char getTeamOnTurn() const;
	void endTurn(bool net=false);
    void changeIndex(Building* bl, char initialOwner);
	int computeIncome(char);
	void clearValidMoves();
    void setWn(MainWindow* wn );

	Unit* getSelected_unit();


private:
	Game();
	Unit* buildUnit(int x, int y, int type,char team);
	bool testObstacle(int x, int y);
    void drawPossibleMoves();
	void testCaptureAndHealing(Unit*);
    bool testEndOfGame();
    std::vector<int> currentDirections;
    unsigned int currentI;

};

#endif // GAME_H
