#include <iostream>
#include <algorithm>
#include <vector>
#include "game.h"
Game* Game::instance = 0; //needed for singleton
#include "airport.h"
#include "city.h"
//#include "Unit.h"
#include "network.h"
#include "ai.h"
#include "infantry.h"
#include "airunit.h"
#include "terrain.h"
#include "mainwindow.h"

#include "mapbuilder.h"
using namespace std;

Game *Game::getInstance(){

    if(!Game::instance){ // doesnt work yet
		Game::instance = new Game();
    }
    return Game::instance;
}

void Game::setPath(QString path)
{
	//this->intMap = MapBuilder::makeIntMap(path);
	unsigned int x = intMap.size();
	unsigned int y = intMap[0].size();
    size_x = int(x);
    size_y = int(y);
	vector<vector<vector<GameObject*> > > mapPro(x, vector<vector<GameObject*> >(y,vector<GameObject*>()));
	this->map = mapPro;
    MapBuilder::buildTerrainMap(this);
}


Game::Game()
{
	this->network = NULL;
    this->attacking = false;
    this->ai = NULL;
	this->selected_unit = NULL;
	this->selected_factory = NULL;
	this->money_orange = 1000; //TODO
	this->money_blue = 1000;
	this->orange_on_turn = true;
}

void Game::setupGame(int income, bool IA, int IAOption, bool isHost){
	if(isHost){
        this->income = income;
		this->intMap = MapBuilder::makeIntMap(":/Map/Images/Maps/Map.txt");
		this->setPath(":/Map/Images/Maps/Map.txt");
        if(!this->network) this->ai = new AI('b',&this->units_blue, this->buildings);
/*
		Unit* un = new Infantry(5,5,1,'b');
		this->addUnit(un,5,5,'b');
		un = new Infantry(5,8,0,'o');
		this->addUnit(un,5,8,'o');
		un = new Infantry(3,2,1,'o');
		this->addUnit(un,3,2,'o');
		this->selected_unit = this->units_orange[0];*/
		this->selected_x = 1;
		this->selected_y = 1;

		this->setIncome(1000); //TODO

	}else{
		this->ai = new AI('b',&this->units_blue, this->buildings);
    }
}

void Game::setupGame(const bool isHost)
{
    if(isHost){
        this->intMap = MapBuilder::makeIntMap(":/Map/Images/Maps/Map.txt");
        this->setPath(":/Map/Images/Maps/Map.txt");
        if(!this->network) this->ai = new AI('b',&this->units_blue, this->buildings);
        //AI is still buggy, uncomment to set AI
/*
        Unit* un = new Infantry(5,5,1,'b');
        this->addUnit(un,5,5,'b');
        un = new Infantry(5,8,0,'o');
        this->addUnit(un,5,8,'o');
        un = new Infantry(3,2,1,'o');
        this->addUnit(un,3,2,'o');
        this->selected_unit = this->units_orange[0];*/
        this->selected_x = 1;
        this->selected_y = 1;

        this->setIncome(1000); //TODO

    }else{
        this->ai = new AI('b',&this->units_blue, this->buildings);
    }
}

void Game::setIntMap(std::vector<std::vector<int> > & map){
	this->intMap = map;
}

vector<vector<int> > & Game::getIntMap()
{
    return this->intMap;
}

void Game::receiveNetwork(Network *net)
{
	this->network = net;
}

void Game::setIncome(int inc){
	this->income = inc;
}

int Game::getIncome(){
	return this->income;
}

void Game::networkAction(string type, int x, int y, int data, int data2)
{
	//cout<<"hello network"<<endl;

	if(type == "move"){
		vector<GameObject*>& tile = this->map[x][y];
		for(unsigned int i=0; i<tile.size();i++){
			if(tile[i]->getType().find("Unit") != string::npos){
				Unit* temp = this->selected_unit;
				this->selected_unit = dynamic_cast<Unit*>(tile[i]);
				int delX = data - x;
				int delY = data2 - y;
				if(delX > 0 && delY == 0){ // right
					this->move(3,true);
				}else if(delX < 0 && delY == 0){ // left
					this->move(2,true);
				}else if(delX == 0 && delY > 0){ //down
					this->move(1,true);
				}else if(delX == 0 && delY < 0){ //up
					this->move(0,true);
				}
				this->selected_unit = temp;
				this->clearValidMoves();
			}
		}
	}else if(type == "newunit"){ //sometimes causes a segfault, idk why
		switch(data2){
			case 1: this->addUnit(this->buildUnit(x,y,data,'o'),x,y,'o',true);break;
		case 2: this->addUnit(this->buildUnit(x,y,data,'b'),x,y,'b',true);break;
		}
	}else if(type == "attack"){
		Unit* attacker = NULL;
		Unit* victim = NULL;
		vector<GameObject*>& tile = this->map[x][y];
		for(unsigned int i=0; i<tile.size();i++){
			if(tile[i]->getType().find("Unit") != string::npos){
				attacker = dynamic_cast<Unit*>(tile[i]);
				break;
			}
		}
		tile = this->map[data][data2];
		for(unsigned int i=0; i<tile.size();i++){
			if(tile[i]->getType().find("Unit") != string::npos){
				victim = dynamic_cast<Unit*>(tile[i]);
				break;
			}
		}
		if(attacker && victim){
			attacker->attack(*victim);
		}
	}else if(type == "endTurn"){
		this->endTurn(true);
	}
}

Unit* Game::buildUnit(int x, int y, int type,char team) // unit differentiator
{
	Unit* un;
	switch(type){
		case 0: case 1:
			un = new Infantry(x,y,type,team);
			break;
		case 2: case 3: case 4: case 5: case 6: case 7:
			un = new Unit(x,y,type,team);
			break;
		case 8: case 9: case 10:
			un = new AirUnit(x,y,type,team);
			break;
		default:
			un = NULL; //error
	}
	return un;
}
/*
vector<GameObject*>* Game::getMapContent(){
	vector<GameObject*>* vec = new vector<GameObject*>;
    for(int i=0;i<size_x;i++){
        for(int j=0;j<size_y;j++){
			vector<GameObject*>& tile = this->map[i][j];
			for(unsigned int k=0;k<tile.size();k++){ //we only collect terrain
				if(!(tile[k]->getType().find("Unit") != string::npos)){
					//cout<<"Network request processing"<<endl;
					vec->push_back(tile[k]);
				}
			}
		}
	}
	return vec;
}
*/
vector<GameObject*>& Game::getObjectsOnPos(int x, int y)// const
{
    if(x>=0 && y>=0 && x<size_x && y<size_y){
			return this->map[x][y];
		}else{
			//vector<GameObject*> tl;// = vector<GameObject*>(); //TODO setup empty vector
			this->test.clear();
			return this->test;
		}
}

int Game::getTerrainMovementModifier(Unit* un, int x, int y)// const doesnt work?
{
	int mov =1;
	vector<GameObject*>& tile = this->map[x][y];
	for(unsigned int i=0;i<tile.size();i++){
		if(tile[i]->getType() == "Terrain"){
			mov = dynamic_cast<Terrain*>(tile[i])->getMovement(un);
		}
		Unit* unit = dynamic_cast<Unit*>(tile[i]); // tests if tile isnt blocked by enemy unit
		if(unit && unit->getTeam() != un->getTeam()){
			return 10000; //sth huge
		}
	}
	return mov;
}

int Game::getTerrainDefenseModifier(Unit& un, int x, int y)// const doesnt work?
{
	if(un.getType() == "AirUnit"){
		return 0;
	}else{
		vector<GameObject*>& tile = this->map[x][y];
		for(unsigned int i=0;i<tile.size();i++){
			if(tile[i]->getType() == "Terrain"){
				return dynamic_cast<Terrain*>(tile[i])->getDefense();
			}
		}
		return 1;
	}
}

std::vector<Unit*>* Game::getUnits(char pl)// const
{
	if(pl == 'o'){
		return &this->units_orange;
	}else if(pl == 'b'){
		return &this->units_blue;
	}else{
		return NULL; //error
	}
}

int Game::getBalance(char pl) const
{
	if(pl == 'o'){
		return this->money_orange;
	}else if(pl == 'b'){
		return this->money_blue;
	}else{
		return -1; //error
    }
}
/*
vector<Unit *>& Game::getUnits(char pl)
{
	if(pl == 'o'){
		return this->units_orange;
	}else{
		return this->units_blue;
	}
}
*/
/*
vector<Unit *>& Game::getUnits_orange()
{
    return this->units_orange;
}
*/
void Game::pay(int qnt, char player)
{
	if(player == 'o'){
		this->money_orange -= qnt;
	}else if(player == 'b'){
		this->money_blue -= qnt;
	}else{
		cout<<"Error, no such player"<<endl;
	}
}

GameObject* Game::addGameObject(GameObject* go, int x, int y, const char team){
	if(go->getType().find("Unit") != string::npos){
		this->addUnit(dynamic_cast<Unit*>(go),x,y,team);
	}else if(go->getType() == "City"){
		City* ct = dynamic_cast<City*>(go);
		this->map[x][y].push_back(ct);
		this->buildings.push_back(ct);
		this->map[x][y].push_back(new Terrain(x,y,9));
	}else if(go->getType() == "Factory"){
		Factory* fac = dynamic_cast<Factory*>(go);
		this->map[x][y].push_back(fac);
		this->buildings.push_back(fac);
		this->map[x][y].push_back(new Terrain(x,y,10));
	}else if(go->getType() == "Airport"){
		Airport* air = dynamic_cast<Airport*>(go);
		this->map[x][y].push_back(air);
		this->buildings.push_back(air);
		this->map[x][y].push_back(new Terrain(x,y,11));
	}else if(go->getType() == "Terrain"){
		this->map[x][y].push_back(dynamic_cast<Terrain*>(go));
	}else{
		cout<<"Error: Unknown GameObject"<<endl;
	}
	return go;
}

Unit* Game::addUnit(Unit* un, int x, int y, const char team, bool net)
{
	//Unit* un = new Unit(x,y,team);
    if(team == 'o'){
		this->map[x][y].push_back(un);
		this->units_orange.push_back(un);
		if(network && !net) this->network->sendData("newunit",x,y,un->getUnitType(),1);
		//cout<<x<<" "<<y<< "  "<<this->units_orange.end()->getPosX() << " "<<this->units_orange.end()->getPosY()<<endl;
    }else if(team == 'b'){
		this->map[x][y].push_back(un);
		this->units_blue.push_back(un);
		if(network && !net) this->network->sendData("newunit",x,y,un->getUnitType(),2);
    }else{
		cout<< "Error: Team does not exist! "<< team <<endl;
		return NULL;
    }
	return un;
}

void Game::move(int dir, bool net, bool justPassing)
{
	if(this->attacking){
		this->attack(dir);
	}else if(this->selected_unit){
		int x = this->selected_unit->getPosX();
		int y = this->selected_unit->getPosY();
        int newX = 0;
        int newY = 0;
		switch(dir){
			case 0: newX = x; newY = y-1; break; //up
			case 1: newX = x; newY = y+1; break; //down
			case 2: newX = x-1; newY = y; break; //left
			case 3: newX = x+1; newY = y; break; //right
		}
		if(justPassing){
			this->selected_unit->move(dir,this->getTerrainMovementModifier(this->selected_unit, newX, newY));
			vector<GameObject*>::iterator it;
			for(it=this->map[x][y].begin();it!=this->map[x][y].end();it++){
				if(this->selected_unit == (*it)){
					this->map[x][y].erase(it);
					break;
				}
			}
			x = this->selected_unit->getPosX();
			y = this->selected_unit->getPosY();
			this->map[x][y].push_back(this->selected_unit); //possible conflict
			this->selected_x = x;
			this->selected_y = y;
		}else{
            if(newX>= 0 && newY>= 0 && newX<size_x && newY<size_y && !testObstacle(newX,newY)){
				if(this->selected_unit->move(dir,this->getTerrainMovementModifier(this->selected_unit, newX, newY))){

					vector<GameObject*>::iterator it;
					for(it=this->map[x][y].begin();it!=this->map[x][y].end();it++){
						//cout<< "Cycle"<<endl;
						//cout<< this->selected_unit<< " "<<*it<<endl;
						if(this->selected_unit == (*it)){
							this->map[x][y].erase(it);
							//cout<<"Mov erase"<<endl;
							if(this->network && !net){
								this->network->sendData("move",x,y, newX, newY);
							}
							break;
						}
					}
					x = this->selected_unit->getPosX();
					y = this->selected_unit->getPosY();
					//cout<<(*(*it)).getType()<<endl;
					this->map[x][y].push_back(this->selected_unit); //possible conflict
					this->selected_x = x;
					this->selected_y = y;
					this->drawPossibleMoves();
					this->wn->update();
				}
			}
		}
	}
}

void Game::moveTo(int x, int y)
{
	bool ok = false;
	if(this->selected_unit){
		vector<ValidMove*> moves = this->selected_unit->selected();
		for(unsigned int i=0;i<moves.size();i++){
			if(moves[i]->getPosX()==x && moves[i]->getPosY()==y){
				ok=true;
				break;
			}
		}
	}/*
	vector<vector<int> >::iterator it_mv;
	for(it_mv = moves.begin(); it_mv!=moves.end();it_mv++){
		vector<int> coord = *it_mv;
		if(coord[0]==x && coord[1]==y){
			ok=true;
			break;
		}
	}*/
	this->clearValidMoves();
	if(ok){
		//cout<< "moving"<<endl;
		vector<int> directions = this->selected_unit->getDirections(x,y);
		for(unsigned int i=directions.size();i>1;i--){ // parse in reverse
			this->move(directions[i-1],false,true);
		}
		this->move(directions[0],false,false);
		/*
		while(this->selected_unit->getPosX() != x){
			if(this->selected_unit->getPosX() > x){
				this->move(2);
			}else{
				this->move(3);
			}
		}
		while(this->selected_unit->getPosY() != y){
			if(this->selected_unit->getPosY() > y){
				this->move(0);
			}else{
				this->move(1);
			}
		}*/
	}
}

bool Game::testObstacle(int x, int y){
	for(unsigned int i=0;i<this->map[x][y].size();i++){
		if(this->map[x][y][i]->getType().find("Unit") != string::npos){// == "Unit"){
			Unit* un = dynamic_cast<Unit*>(this->map[x][y][i]);
			char team = un->getTeam();
			if(this->selected_unit->getTeam() == team && un->getHealth() != 10){
				//unit fusion here
				vector<Unit*>::iterator it;
				if(team == 'o'){
					it = find(this->units_orange.begin(),this->units_orange.end(),un);
					this->units_orange.erase(it);
				}else if(team == 'b'){
					it = find(this->units_blue.begin(),this->units_blue.end(),un);
					this->units_blue.erase(it);
				}
				this->selected_unit->heal(un->getHealth());
				vector<GameObject*>::iterator itr;
				itr = find(this->map[x][y].begin(),this->map[x][y].end(),un);
				cout<<*itr<<endl;
				this->map[x][y].erase(itr);
				return false;
			}
			cout<<"Obstacle"<<endl;
			return true;
		}
	}
	return false;
}

void Game::setAttack(){
	this->attacking = !this->attacking;
}

void Game::attack(int dir)
{
	if(this->selected_unit){
		int x = this->selected_unit->getPosX();
		int y = this->selected_unit->getPosY();
		int attX;
		int attY;
		switch(dir){
			case 0: attX = x; attY = y-1; break; //up
			case 1: attX = x; attY = y+1; break; //down
			case 2: attX = x-1; attY = y; break; //left
			case 3: attX = x+1; attY = y; break; //right
		}
		vector<Unit*>::iterator it;
		if(this->orange_on_turn){
			for(it = this->units_blue.begin(); it!=this->units_blue.end();it++){
				if((*it)->getPosX() == attX && (*it)->getPosY() == attY){
					this->selected_unit->attack(**it);
					if(this->network) this->network->sendData("attack",this->selected_unit->getPosX(),this->selected_unit->getPosY(),
											(*it)->getPosX(),(*it)->getPosY());
					break;
				}
			}
		}else{
			for(it = this->units_orange.begin(); it!=this->units_orange.end();it++){
				if((*it)->getPosX() == attX && (*it)->getPosY() == attY){
					this->selected_unit->attack(**it);
					if(this->network) this->network->sendData("attack",this->selected_unit->getPosX(),this->selected_unit->getPosY(),
											(*it)->getPosX(),(*it)->getPosY());
					break;
				}
			}
		}
	}
	this->attacking = false;
}

void Game::click(int x, int y)
{

	if(!this->map[x][y].empty()){
		//for(GameObject& go : this->map[x][y]){ //just dont use range-based, doesnt work, wild pointers
		vector<GameObject*>::iterator itr;
		vector<GameObject*>& tile = this->map[x][y]; //to prevent concurrentModificationException
		for(itr = tile.begin();itr!=tile.end();itr++){
			if(Factory* fac = dynamic_cast<Factory*>(*itr)){
				if((fac->getOwner() == 'o' && this->orange_on_turn)
					|| (fac->getOwner()=='b' && !this->orange_on_turn)){ //tests for selecting enemy fac
						this->selected_factory = fac;
				}
			}
			(*itr)->interactWith();
		/*
		if(go->getType() == "Factory" || go->getType() == "City"){
			go->select();
		}*/
		}
	}
}

void Game::buyUnit(int type)
{
	if(this->selected_factory != NULL){
		this->selected_factory->buy(type);
		this->selected_factory = NULL;
	}
}

void Game::selectUnit(Unit* un){
	if(this->orange_on_turn && find(this->units_orange.begin(),this->units_orange.end(),un) != this->units_orange.end()){
		//find(this->units_orange.begin(),this->units_orange.end(),un);
		this->selected_unit = un;
		this->selected_x = this->selected_unit->getPosX();
		this->selected_y = this->selected_unit->getPosY();
		//cout << this->selected_unit << endl;
		this->drawPossibleMoves();
	}else if(!this->orange_on_turn && find(this->units_blue.begin(),this->units_blue.end(),un) != this->units_blue.end()){
		this->selected_unit = un;
		this->selected_x = this->selected_unit->getPosX();
		this->selected_y = this->selected_unit->getPosY();
		//cout << this->selected_unit << endl;
		this->drawPossibleMoves();
    }else if(this->orange_on_turn && find(this->units_blue.begin(),this->units_blue.end(),un) != this->units_blue.end()){
        //attacking a blue unit
		int x = un->getPosX();
		int y = un->getPosY();
		int delX = x - this->selected_unit->getPosX();
		int delY = y - this->selected_unit->getPosY();
		delX = abs(delX);
		delY = abs(delY);
		if((delX ==0 && delY ==1) || (delX == 1 && delY == 0)){
			cout<<"Ok, units are near to attack"<<endl;
			this->selected_unit->attack(*un);
            if( this->network){
                this->network->sendData("attack",this->selected_unit->getPosX(),this->selected_unit->getPosY(),x,y);
            }

		}/*
        for(unsigned int i=0;i<this->map[x][y].size();i++){
			//program the movement to!!
            if(this->map[x][y][i]->getType() =="ValidMove"){
				this->selected_unit->attack(*un);
                break;
            }
		}*/
    }else if(!this->orange_on_turn && find(this->units_orange.begin(),this->units_orange.end(),un) != this->units_orange.end()){
        //attacking an orange unit
		int x = un->getPosX();
		int y = un->getPosY();
		int delX = abs(x - this->selected_unit->getPosX());
		int delY = abs(y - this->selected_unit->getPosY());
		if((delX ==0 && delY ==1) || (delX == 1 && delY == 0)){
			cout<<"Ok, units are near to attack"<<endl;
			this->selected_unit->attack(*un);
		}/*
        for(unsigned int i=0;i<this->map[x][y].size();i++){
            if(this->map[x][y][i]->getType() =="ValidMove"){
				this->selected_unit->attack(*un);
                break;
            }
		}*/
	}else{
		cout<< "Error when selecting unit, the unit seems to not exist."<<endl;
		//also when not selecting your unit
	}
    this->wn->update();
}

void Game::cycleUnits(int dir)
{   //-1 and 1 (backwards/forwards)
	//segfault when no units
	vector<Unit*>::iterator it;
    //cout << "size " << this->units.size() << " Add "<< &this->units[0] << " " <<&this->units[1]
    //     <<" "<< &this->units[2] << endl;
    if(this->orange_on_turn){
		if(!this->selected_unit){
			if(this->units_orange.size() != 0) this->selected_unit = this->units_orange[0];
		}else{
			for(it = this->units_orange.begin(); it!=this->units_orange.end();it++){
				//cout << "Hello x" << endl;
				if(this->selected_unit == *it){
					if(dir >0){
						it++;
						if(it == this->units_orange.end()){
							it = this->units_orange.begin();
						}
					}else{
						it--;
						if(it == this->units_orange.begin()-1){
							it = this->units_orange.end()-1;
						}
					}
					//cout << "Hello" << endl;
					break;
				}
			}
		}
    }else{
		if(!this->selected_unit){
			if(this->units_blue.size() != 0) this->selected_unit = this->units_blue[0];
		}else{
        for(it = this->units_blue.begin(); it!=this->units_blue.end();it++){
				//cout << "Hello x" << endl;
				if(this->selected_unit == *it){
					if(dir>0){
						it++;
						if(it == this->units_blue.end()){
							it = this->units_blue.begin();
						}
					}else{
						it--;
						if(it == this->units_blue.begin()-1){
							it = this->units_blue.end()-1;
						}
					}
					//cout << "Hello" << endl;
					break;
				}
			}
		}
    }
	if((this->orange_on_turn && this->units_orange.size() != 0) || (!this->orange_on_turn && this->units_blue.size() != 0)){
		this->selected_unit = *it;
		this->selected_x = this->selected_unit->getPosX();
		this->selected_y = this->selected_unit->getPosY();
		//cout << this->selected_unit << endl;
		this->drawPossibleMoves();
	}
}

void Game::deleteUnit(Unit *un)
{
	vector<Unit*>::iterator it;
    if(this->orange_on_turn){
        for(it = this->units_blue.begin(); it!=this->units_blue.end();it++){
			//cout<< "Del"<<endl;
			if(un == *it){
				int x = un->getPosX();
				int y = un->getPosY();
				vector<GameObject*>::iterator itr;
				for(itr = this->map[x][y].begin(); itr != this->map[x][y].end();itr++){
					if(un == *itr){
						this->map[x][y].erase(itr);
						break;
					}
				}
				delete *it;
                this->units_blue.erase(it);
				break;
            }
        }
    }else{
        for(it = this->units_orange.begin(); it!=this->units_orange.end();it++){
			if(un == *it){
				int x = un->getPosX();
				int y = un->getPosY();
				vector<GameObject*>::iterator itr;
				for(itr = this->map[x][y].begin(); itr != this->map[x][y].end();itr++){
					if(un == *itr){
						this->map[x][y].erase(itr);
						break;
					}
				}
				delete *it;
                this->units_orange.erase(it);
				break;
            }
        }
    }
}

void Game::clearValidMoves(){
	//cout<<"Cleaning time"<<endl;
    for(int i = 0;i<size_x;i++){
        for(int j=0;j<size_y;j++){
			vector<GameObject*>::iterator it;
			for(it=this->map[i][j].begin();it!=this->map[i][j].end();it++){
				//cout<<"Here cycle "<<i<<" "<<j<<endl;
				//cout<<(ValidMove*)(&(*it))<<endl; //dyn cast needs RTTI
				//cout<< (*it)->getType()<<endl;
				if((*it)->getType() == "ValidMove"){
						//(ValidMove*)(&(*it))){//dynamic_cast<ValidMove*>(&(*it))){//ValidMove(i,j) == *it){
					//cout<< "ValidMove "<<*it<<" found "<<i<<" "<<j<<endl;
					delete (*it);
					this->map[i][j].erase(it); // memory leak?
					break;
					//it = this->map[i][j].begin();
				}
			}
		}
    }
}

void Game::setWn(MainWindow *wn)
{
    this->wn = wn;
}

void Game::drawPossibleMoves(){
	//cout<<"Here"<<endl;
	//this->clearValidMoves();
	//cout<<"Here2"<<endl;
	vector<ValidMove*> moves = this->selected_unit->selected();
	this->clearValidMoves();
	//cout<<"Here3"<<endl;
	/*
	vector<vector<int> >::iterator it_mv;
	for(it_mv = moves.begin(); it_mv!=moves.end();it_mv++){
		vector<int> coord = *it_mv;
		this->map[coord[0]][coord[1]].push_back(new ValidMove(coord[0],coord[1],false));
		//possible memory leak here
	}*/
	for(unsigned int i=0;i<moves.size();i++){
		ValidMove* vm = moves[i];
		this->map[vm->getPosX()][vm->getPosY()].push_back(vm);
	}
	//cout<<"Here4"<<endl;
}

int Game::getSelectedX(){
    return this->selected_x;
}

int Game::getSelectedY(){
    return this->selected_y;
}

char Game::getTeamOnTurn() const
{
	if(this->orange_on_turn){
		return 'o';
	}else{
		return 'b';
	}
}

void Game::endTurn(bool net)
{
    cout<< "End turn"<<endl;
	this->selected_unit = NULL;
	if(network && !net) this->network->sendData("endTurn");
	vector<Unit*>::iterator it;
    if(this->orange_on_turn){
        for(it = this->units_orange.begin(); it!=this->units_orange.end();it++){
			(*it)->newTurn();
			this->testCaptureAndHealing(*it);
        }
		if(!testEndOfGame()){
			this->orange_on_turn = false;
			this->money_blue += this->computeIncome('b');
			/*
			if(!this->units_blue.empty()) this->selected_unit = this->units_blue[0];
			for(unsigned int i=0;i<this->buildings.size();i++){ //TODO rewrite airport
				if(this->buildings[i]->getType() != "Airport" && this->buildings[i]->getOwner() =='b'){
					this->money_blue += 1000;
				}
			}*/
			cout<<"Blue money: "<<this->money_blue<<endl;
            if (this->ai != NULL){
                this->ai->play();
			}else{
				//this->selectUnit(this->units_blue[0]); //TODO segfault
			}

		}else{
			cout<<"The game had ended: Orange wins!"<<endl;
		}
    }else{
        for(it = this->units_blue.begin(); it!=this->units_blue.end();it++){
			(*it)->newTurn();
			this->testCaptureAndHealing(*it);
        }
		if(!testEndOfGame()){
			this->orange_on_turn = true;
			this->money_orange += this->computeIncome('o');
			/*
			if(!this->units_orange.empty()) this->selected_unit = this->units_orange[0];
			for(unsigned int i=0;i<this->buildings.size();i++){
				if(this->buildings[i]->getType() != "Airport" && this->buildings[i]->getOwner() =='o'){
					this->money_orange += 1000;
				}
            }*/

			//this->selectUnit(this->units_orange[0]);
			cout<<"Orange money: "<<this->money_orange<<endl;
		}else{
			cout<<"The game had ended: Blue wins!"<<endl;
		}

    }
}

void Game::changeIndex(Building *bl, char initialOwner) //changes index of building when owner changes
{
    if(bl->getOwner() == 'b' && initialOwner == 'o' ){
        this->intMap[bl->getPosX()][bl->getPosY()] += 5;
    }
    else if (bl->getOwner() == 'o' && initialOwner == 'b' ){
        this->intMap[bl->getPosX()][bl->getPosY()] -= 5;
    }
    else if (bl->getOwner() == 'b' && initialOwner == '\0' ){
        this->intMap[bl->getPosX()][bl->getPosY()] += 9;
    }
    else if (bl->getOwner() == 'o' && initialOwner == '\0' ){
        this->intMap[bl->getPosX()][bl->getPosY()] += 4;
    }
    qDebug() << this->intMap[bl->getPosX()][bl->getPosY()];
}

int Game::computeIncome(char pl){
	int income = 0;
	if(pl =='o'){
		for(unsigned int i=0;i<this->buildings.size();i++){
			if(this->buildings[i]->getType() != "Airport" && this->buildings[i]->getOwner() =='o'){
				income += this->income;
			}
		}
	}else if(pl =='b'){
		for(unsigned int i=0;i<this->buildings.size();i++){ //TODO rewrite airport
			if(this->buildings[i]->getType() != "Airport" && this->buildings[i]->getOwner() =='b'){
				income += this->income;
			}
		}
	}
	return income;
}

void Game::testCaptureAndHealing(Unit* un)
{
	int x = un->getPosX();
	int y = un->getPosY();
	vector<GameObject*> tl = this->map[x][y];
	vector<GameObject*>::iterator itr;
	for(itr = tl.begin();itr!=tl.end();itr++){
		GameObject* go = *itr;
        Building* bl = dynamic_cast<Building*>(go);
        //cout << city << endl;
        if(bl != NULL) { // go->getType() == "building")
            char initialOwner = bl->getOwner();
            bl->capture(un);
            if(bl->getOwner() == un->getTeam()){
                bl->healUnit(un);
                if (bl->getOwner() != initialOwner){
                    this->changeIndex(bl, initialOwner);
                    this->wn->reloadImage(bl->getPosX(),bl->getPosY());
                }
            }
		}
	}
}

bool Game::testEndOfGame()
{
	if(this->orange_on_turn){
		if(this->units_blue.empty()){
			bool ended = true;
			for(unsigned int i =0; i<this->buildings.size();i++){
				Building* bld = this->buildings[i];
				if(bld->getType() == "Factory" && bld->getOwner() == 'b'){
					ended = false;
					vector<GameObject*>& tile = this->map[bld->getPosX()][bld->getPosY()];
					for(unsigned int j=0;j<tile.size();j++){
						if(tile[j]->getType().find("Unit") != string::npos){// == "Unit"){
							ended = true;
						}
					}
				}
			}
			return ended;
		}
	}else{
		if(this->units_orange.empty()){
			bool ended = true;
			for(unsigned int i =0; i<this->buildings.size();i++){
				Building* bld = this->buildings[i];
				if(bld->getType() == "Factory" && bld->getOwner() == 'o'){
					ended = false;
					vector<GameObject*>& tile = this->map[bld->getPosX()][bld->getPosY()];
					for(unsigned int j=0;j<tile.size();j++){
						if(tile[j]->getType().find("Unit") != string::npos){// == "Unit"){
							ended = true;
						}
					}
				}
			}
			return ended;
		}
	}
	return false;
}

Unit* Game:: getSelected_unit(){
    return selected_unit;
}
/*
int Game:: getMoney_orange(){
    return money_orange;
}

int Game::getMoney_blue(){
    return money_blue;
}*/
