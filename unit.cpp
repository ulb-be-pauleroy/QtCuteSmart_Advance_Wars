#include <iostream>
//#include <vector>
#include <queue>
//#include <list>
#include "unit.h"
#include "game.h" //solves the cyclic dependency problem
using namespace std;

// dmg_chart[attacker][target]
// 0 = infantry, 1 = bazooka, 2 = recon, 3 = anti-air
// 4 = tank, 5 = md. tank, 6 = mega tank, 7 = neotank
// 8 = b-copter, 9 = fighter, 10 = bomber
const int Unit::dmg_chart[11][11] = {{55,45,12,5,5,1,1,1,7,0,0},
									{65,55,85,65,55,15,5,15,9,0,0},
									{70,65,35,4,6,1,1,1,12,0,0},
									{105,105,60,45,25,10,1,5,120,65,75},
									{75,70,85,65,55,15,10,15,10,0,0},
									{105,95,105,105,85,55,25,45,12,0,0},
									{135,125,195,195,180,125,65,115,22,0,0},
									{125,115,125,115,105,75,35,55,22,0,0},
									{75,75,55,25,55,25,10,20,65,0,0},
									{0,0,0,0,0,0,0,0,100,55,100},
									{110,110,105,95,105,95,35,90,0,0,0}
									};

const int Unit::speed_chart[11] = {3,2,8,6,6,5,4,6,6,9,7};

const int Unit::price_chart[11] = {1000,3000,4000,8000,7000,16000,28000,22000,9000,20000,22000};


Unit::Unit(int x, int y, int type, char team) : GameObject(x,y){
	this->type = type;
	this->price = Unit::price_chart[type];
    this->health = 10;
	this->speed = Unit::speed_chart[type];
    this->moves_left = this->speed;
	this->team = team;
    this->capturing = NULL;
    if(team != 'o' && team != 'b'){
		cout<< "Error: Team does not exist! "<<team<<endl;
	}
}

void Unit::attack(Unit & un, bool counter){
    // TODO verify the formula
	//Damage = B * A_HP / 10 * (100 - D_TR * D_HP) / 100
	//rounding: 1/3 -> 0	1/2, 3/4, 5/4 -> 1		7/4, 9/4 -> 2
	if(this->moves_left > 0){
		double dmg = (double)Unit::dmg_chart[this->type][un.getUnitType()] *
				this->getHealth()/10 * (100 - un.getHealth() *
				Game::getInstance()->getTerrainDefenseModifier(un, un.getPosX(),un.getPosY()))/100;
		cout << dmg << endl;
		int dm = (int)(dmg/10 +0.5); //should do the rounding
		un.sufferDamage(dm);
		this->endTurn();
		if(!counter){
			un.attack(*this, true); // to prevent infinite loop
		}
	}
}

int Unit::getHealth() const{
	return this->health;
}

void Unit::heal(int qnt)
{
	this->health += qnt;
	if(this->health >10){
		this->health = 10;
	}
}

int Unit::getPrice() const
{
	return this->price;
}

int Unit::getUnitType() const
{
	return this->type;
}

char Unit::getTeam() const
{
	return this->team;
}

void Unit::sufferDamage(int & dmg){
    cout<< this << " got "<< dmg <<" damage!"<<endl;
    this->health -= dmg;
    if(this->health <=0){
		this->die();
    }
}

void Unit::die()
{
    cout<<"Unit "<<this<<" is dead!"<<endl;
	Game::getInstance()->deleteUnit(this);
}

bool Unit::move(int dir, int terrainMod){
	//cout<<"Mov test"<<endl;
	if(this->moves_left - terrainMod >= 0){
        switch(dir){
            case 0: this->posY--; break; //up
            case 1: this->posY++; break; //down
            case 2: this->posX--; break; //left
            case 3: this->posX++; break; //right
        }
        if (this->getUnitType() == 0 && this->capturing != 0){
            this->capturing->abandonCapture();
            this->capturing = NULL;
        }
		this->moves_left -= terrainMod;
        return true;
    }
    return false;
}
/*
bool Unit::operator==(Unit & un) const
{
    //cout << this << " " << &un << endl;
	return this == &un;
}

bool Unit::operator==(GameObject &obj) const
{
	Unit* un = dynamic_cast<Unit*>(&obj); // use pointers
	if(un != 0){ // downcast successfull
		return this->operator==(*un);
	}else{
		return false;
	}
}
*/
void Unit::endTurn()
{
	this->moves_left = 0;
	Game::getInstance()->selectUnit(this); // kinda complicated, but works
}

void Unit::newTurn()
{
    this->moves_left = this->speed;
}

bool Unit::isCapturing()
{
    return this->capturing;
}

void Unit::setCapture(Building *building)
{
    this->capturing = building;
}


std::vector<ValidMove *> Unit::selected(){ //int[] (int* should do)
    //cout << "A unit is selected, returning possible moves." << endl;
    //cout<<this->posX<<" "<<this->posY<<endl;
    //vector<int> input = this->bfs();
    vector<int> input = this->dijkstra();
	vector<ValidMove*> res;
    vector<int>::iterator it;
    for(it = input.begin(); it!=input.end();it++) {
        int& i = *it;
        int tb[2];
        int* p = this->getCoordFromPos(i, tb);
		bool lst = false;
		for(unsigned int j=0;j<this->last.size();j++){
			if(this->last[j] == i){
				lst =true;
				break;
			}
		}
		if(lst){
			res.push_back(new ValidMove(*p,*(p+1),true));
		}else{
			res.push_back(new ValidMove(*p,*(p+1),false));
		}
		/*
        v.push_back(*p);
        v.push_back(*(p+1));
		res.push_back(v);*/
    }
    return res;
}

vector<int> Unit::getDirections(int x, int y){
	this->dijkstra();
	vector<int> directions;
	int me = this->getPosFromCoord(this->posX,this->posY);
	int target = this->getPosFromCoord(x,y);
	while(target != me){
		int prt = this->parent[target];
		int tb[2];
		int* nw = this->getCoordFromPos(target,tb);
		int tb2[2];
		int* old = this->getCoordFromPos(prt,tb2);
		int delX = *nw - *old;
		int delY = *(nw+1) - *(old+1);
		if(delX == 0){
			if(delY > 0){
				directions.push_back(1); //up
			}else{
				directions.push_back(0); //down
			}
		}else{
			if(delX > 0){
				directions.push_back(3); //right
			}else{
				directions.push_back(2); //left
			}
		}
		target = prt;
	}
	return directions;
}

string Unit::getType() const
{
	//cout<< "un"<<endl;
	return "Unit";
}

void Unit::interactWith()
{
	Game::getInstance()->selectUnit(this);
}

int Unit::getPosFromCoord(int x, int y) { //STATIC
	int sy = YDIM; // for now
	if(x<0 || y<0){
        cout<< x<<" "<<y <<endl;
	}
    return sy*x+y;
}

int* Unit::getCoordFromPos(int pos, int (&cr)[2]) { //static + int[]
	int sy = YDIM; //for now
    cr[0] = pos/sy;
    cr[1] = pos%sy;
    return cr;
}

vector<Edge> Unit::findConnected(int pos){ // linkedlist
        //cout << "Hello" << endl;
        int tb[2];
        int* coord = this->getCoordFromPos(pos, tb);
        int nextTo[4];
        //cout << "Hello2 " << coord<< " "<< coord << endl;
		if(coord[0] != XDIM-1){
			nextTo[0] = this->getPosFromCoord(coord[0]+1, coord[1]);
		}else{
			nextTo[0] = -1;
		}
        //to prevent valid moves on map edges
        if(coord[0] !=0){
            nextTo[1] = this->getPosFromCoord(coord[0]-1, coord[1]);
        }else{
            nextTo[1] = -1;
        }
		if(coord[1] != YDIM-1){
			nextTo[2] = this->getPosFromCoord(coord[0], coord[1]+1);
		}else{
			nextTo[2] = -1;
		}
        if(coord[1] !=0){
            nextTo[3] = this->getPosFromCoord(coord[0], coord[1]-1);
        }else{
            nextTo[3] = -1;
        }
        vector<Edge> res;// = new LinkedList<Edge>();
        for(int i =0;i<4;i++) {
            if(nextTo[i] != -1) { // no obstacles yet
				int* cr = this->getCoordFromPos(nextTo[i], tb);
				res.push_back(Edge(nextTo[i],Game::getInstance()->getTerrainMovementModifier(this,cr[0],cr[1])));
            }
        }
        //cout<< "hello2.5 "<<res[0].getTo()<<endl;
        return res;
    }

vector<int> Unit::dijkstra(){
	int sx = XDIM;
	int sy = YDIM;
	int dist[sx*sy];// = new int[sx*sy];
	for(int i=0;i<sx*sy;i++){ //Arrays.fill(dist, Integer.MAX_VALUE);
		dist[i] = 10000; //=sth huge
		this->parent[i] = -2; //default value, no meaning
	}
	priority_queue<int, vector<int>, greater<int> > pq;// = new LinkedList<Integer>();
	vector<int> ok;
	this->last.clear();
	int me = this->getPosFromCoord(this->posX, this->posY);
	pq.push(me);
	dist[me] = 0;
	this->parent[me] = -1; //root

	while(!pq.empty()) {
		int a = pq.top();
		pq.pop();
		this->ee[a] = findConnected(a);
		//cout<< a <<" "<<endl;// this->ee[a] << endl;
		//cout<< "hello3 "<< this->ee[a].end() - this->ee[a].begin() <<" "
		//    << this->ee[a][0].getTo() <<endl;
		vector<Edge>::iterator it;
		for(it = this->ee[a].begin(); it!=this->ee[a].end();it++/*Edge& e : this->ee[a]*/) {
			Edge& e = *it;
			int distOfEnd = dist[a]+e.getDist();
			//cout<< "hello3.5" <<endl;
			if(distOfEnd < dist[e.getTo()]) { //same huge thing
				//cout<< "hello3.65" <<endl;
				dist[e.getTo()] = distOfEnd;
				this->parent[e.getTo()] = a; // we found a shorter path, the parent is now a
				//cout<< "hello3.75" <<endl;
				if(dist[e.getTo()] <= this->moves_left){ // = I can move there this turn <= ?
					pq.push(e.getTo());
					//cout<< "hello4" <<endl;
					vector<int>::iterator yet;
					yet = find(ok.begin(),ok.end(),e.getTo());
					if(yet == ok.end()){ // tests if we didnt add it already
						ok.push_back(e.getTo()); // we collect this
					}

					//ok.push_back(e.getTo()); // we collect this

					if(dist[e.getTo()] - this->moves_left >= 0){
						//cout << "This is a last move."<<endl;
						this->last.push_back(e.getTo());
					}else{
						vector<int>::iterator notLast;
						notLast = find(this->last.begin(),this->last.end(),e.getTo());
						if(notLast != this->last.end()){ // we found a shorter path, no longer a last valid move
							this->last.erase(notLast);
						}
					}
				}
			}
			//cout<< "loop end" <<endl;
		}
	}
	return ok;
}
/*
vector<int> Unit::bfs(){
	int sx = XDIM;
	int sy = YDIM;
    int dist[sx*sy];// = new int[sx*sy];
    for(int i=0;i<sx*sy;i++){ //Arrays.fill(dist, Integer.MAX_VALUE);
        dist[i] = 10000; //=sth huge
    }
    queue<int> q;// = new LinkedList<Integer>();
    vector<int> ok;
    int me = this->getPosFromCoord(this->posX, this->posY);
    q.push(me);
	dist[me] = 0;
    //cout<<"in bfs"<<endl;

    while(!q.empty()) {
        int a = q.front();
        q.pop();
        this->ee[a] = findConnected(a);
        //cout<< a <<" "<<endl;// this->ee[a] << endl;
        //cout<< "hello3 "<< this->ee[a].end() - this->ee[a].begin() <<" "
        //    << this->ee[a][0].getTo() <<endl;
        vector<Edge>::iterator it;
		for(it = this->ee[a].begin(); it!=this->ee[a].end();it++) {
            Edge& e = *it;
            //cout<< "hello3.5" <<endl;
            if(dist[e.getTo()] == 10000) { //same huge thing
                //cout<< "hello3.65" <<endl;
				dist[e.getTo()] = dist[a] + 1;
                //cout<< "hello3.75" <<endl;
                if(dist[e.getTo()] <= this->moves_left){ // = I can move there this turn <= ?
                    q.push(e.getTo());
                    //cout<< "hello4" <<endl;
                    ok.push_back(e.getTo()); // we collect this
				}
			}
            //cout<< "loop end" <<endl;
		}
	}
    //cout<< "hello5" <<endl;
	return ok;
}
			*/
