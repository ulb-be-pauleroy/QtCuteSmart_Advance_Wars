#ifndef UNIT_H
#define UNIT_H
//necessary, otherwise the class will be included 
//each time a sub-class object is created

#include<vector>
#include "edge.h"
#include "validmove.h"
#include "gameobject.h"
#include "mapsize.h"

//#include "game.h"
// doesnt work as it is a cyclic dependency
class Game; //used instead
class Building;

class Unit : public GameObject{


	char team;
	int type;
	int speed;
	int moves_left;
	int health;
	int price;
    Building* capturing;

	//std::vector<Edge> ee[XDIM * YDIM]; // TODO static + still hardcoded
	std::vector<std::vector<Edge> > ee;
	std::vector<int> last;
	//int parent[XDIM*YDIM];
	std::vector<int> parent;


public:
    Unit(int x=0, int y=0, int type =0, char team='\0');
	virtual ~Unit() {}
    int getHealth() const;
    void heal(int qnt);
    int getPrice() const;
    int getUnitType() const;
    char getTeam() const;
	bool sufferDamage(int & dmg);
    void die();
    virtual bool move(int dir, int terrainMod);
    //bool operator== (Unit& un) const;
    //bool operator== (GameObject& obj) const;
    void endTurn();
    void newTurn();
    bool isCapturing();
    void setCapture(Building* building);
	std::vector<ValidMove*> selected();
	std::vector<int> getDirections(int,int);
    virtual std::string getType() const;
    void interactWith();
    void attack(Unit & un, bool counter=false);
    int getMovesLeft();

private:
    int getPosFromCoord(int x, int y);
    int* getCoordFromPos(int pos, int (&cr)[2]);
    std::vector<Edge> findConnected(int pos);
	//std::vector<int> bfs();
    std::vector<int> dijkstra();

    // dmg_chart[attacker][target]
    // 0 = infantry, 1 = mech, 2 = recon, 3 = anti-air
    // 4 = tank, 5 = md. tank, 6 = mega tank, 7 = neotank
    // 8 = b-copter, 9 = fighter, 10 = bomber
    const static int dmg_chart[11][11];
    const static int speed_chart[11];
    const static int price_chart[11];

};

#endif
