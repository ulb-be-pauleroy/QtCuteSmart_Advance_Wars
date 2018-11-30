#include "ai.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h> //for rand

#include "aiworker.h"
#include "airport.h"
#include "airunit.h"
#include "infantry.h"

using namespace std;

const int AI::unitCost[11] = {1000,3000,4000,8000,7000,16000,28000,22000,9000,20000,22000};

AI::AI(char team, const std::vector<Unit *> * units, const std::vector<Building *>& buildings)
{
	this->team = team;
	this->myUnits = units;

	for(unsigned int i=0;i<buildings.size();i++){
		if(Factory* fac = dynamic_cast<Factory*>(buildings[i])){
			this->factories.push_back(fac);
		}
	}
}

void AI::play()
{
	cout<<"AI starting turn"<<endl;
	this->meOnTurn = true;
	this->myMoney = Game::getInstance()->getBalance(this->team);
	this->enemyMoney = Game::getInstance()->getBalance('o'); //TODO setup

	std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > newUnits;
	//for(unsigned int i=0;i<builtUnits.size();i++){
		vector<Unit*> v;
		v.push_back(NULL);
		pair<vector<Unit*>, int> p(v,0);
		std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > purchases = this->makePurchases(this->myMoney,p); //make_pair(v,0));
		newUnits.insert(newUnits.end(),purchases.begin(),purchases.end());
		/*
		std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > >::iterator it;
		for(it = newUnits.begin();it!=newUnits.end();it++){ //TODO very buggy !! also in recursion !!
			if((*it).second.first > this->myMoney) newUnits.erase(it);
		}*/
	//}

	vector<vector<pair<Unit*,int> > > futureTurn;
	vector<vector<int> > attPos;
	// no unit cooperation for now
	for(unsigned int i=0;i<this->myUnits->size();i++){
		Unit* un = this->myUnits->at(i); // [i] doesnt work, idk why
		vector<vector<int> > poss = this->moveUnit(un);
		vector<pair<Unit*,int> > futureUnits;

		//vector<QThread*> threads;
		this->th_cnt =0;
		this->th_done =0;
		for(unsigned int j=0;j<poss.size();j++){
			int rating = poss[j][2]; // rating on this turn
			vector<Unit*> targets = this->targetsFromPos(poss[j][0],poss[j][1]);
			if(!targets.empty()){ //tests attack
				for(unsigned int k=0;k<targets.size();k++){
					rating += this->rateAttack(un,targets[k]);
					vector<int> v;// = {poss[j][0],poss[j][1],targets[k]->getPosX(),targets[k]->getPosY()};
					v.push_back(poss[j][0]);
					v.push_back(poss[j][1]);
					v.push_back(targets[k]->getPosX());
					v.push_back(targets[k]->getPosY());
					attPos.push_back(v);
				}
			}
			futureUnits.push_back(make_pair(this->buildUnit(poss[j][0],poss[j][1],un->getUnitType()),rating));

			QThread* thread = new QThread;
			AIWorker* worker = new AIWorker(futureUnits[j],this->team,this->myMoney,newUnits);
			worker->moveToThread(thread);
			connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
			connect(thread, SIGNAL(started()), worker, SLOT(process()));
			connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
			connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
			connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
			connect(worker, SIGNAL(finished()), this, SLOT(threadEnd()));
			//threads.push_back(thread);
			this->th_cnt++;
			//thread->start();
		}
		// TODO zero or one ?!
		/*
		this->playFutureTurn(0,futureUnits,this->myMoney,newUnits); // TODO create thread (max cca 160 for recon)
		int max = -1;
		int maxI;
		for(unsigned int j=0;j<futureUnits.size();j++){ //TODO use a data structure for max search
			if(futureUnits[j].second > max){
				maxI = j;
				max = futureUnits[j].second;
			}
		}

		this->executeAction(un,futureUnits[maxI].first->getPosX(),futureUnits[maxI].first->getPosY(),attPos);
		for(unsigned int j=0;j<futureUnits.size();j++){
			delete futureUnits[j].first; // no memory leak
		}/*
		usleep(1000 *1e3);
		while(this->th_cnt != this->th_done){
			usleep(10 *1e3);
		}*/
		futureTurn.push_back(futureUnits);
	}

	this->make_sequence(0,futureTurn,this->myMoney +Game::getInstance()->computeIncome(this->team),newUnits);
	for(unsigned int i=0;i<futureTurn.size();i++){
		int max = -1;
		int maxI;
		for(unsigned int j=0;j<futureTurn[i].size();j++){ //TODO use a data structure for max search
			if(futureTurn[i][j].second > max){
				maxI = j;
				max = futureTurn[i][j].second;
			}
		}
		this->executeAction((*myUnits)[i],futureTurn[i][maxI].first->getPosX(),futureTurn[i][maxI].first->getPosY(),attPos);
		for(unsigned int j=0;j<futureTurn[i].size();j++){
			delete futureTurn[i][j].first; // no memory leak
		}
	}

	// unit purchasing
	int imax = -1;
	int imaxI;
	for(unsigned int j=0;j<newUnits.size();j++){ //TODO use a data structure for max search
		if(newUnits[j].second.second > imax){
			imaxI = j;
			imax = newUnits[j].second.second;
		}
	}
	this->buyUnits(newUnits[imaxI].first);

	cout<<"AI ending turn"<<endl;
	Game::getInstance()->endTurn();
}

void AI::make_sequence(int depth, std::vector<std::vector<std::pair<Unit *, int> > > & units, int money, std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > &builtUnits){
	//save sequence
	if(this->meOnTurn){
		this->myCases[depth] = &units; //TODO copy !!!
	}else{
		this->enemyCases[depth] = &units; //copy problem should be solved
	}
	this->meOnTurn = !this->meOnTurn;
	//launch enemy turn
	if(this->meOnTurn && depth < MAXDEPTH){ // my depth is the same as enemy-s TODO check
		for(unsigned int i=0;i<this->myCases[depth-1]->size();i++){
			this->playFutureTurn(depth,(*this->myCases[depth-1])[i],money,builtUnits);
		}
	}else if(depth < MAXDEPTH -1){
		if(depth == 0){ //first iteration
			std::vector<std::pair<Unit *, int> > dummy;
			this->playFutureTurn(depth+1,dummy,money,builtUnits);
		}else{
			for(unsigned int i=0;i<this->enemyCases[depth]->size();i++){
				this->playFutureTurn(depth+1,(*this->enemyCases[depth])[i],money,builtUnits);
			}
		}
	}
	//TODO unit purchases, doesnt work (or is defensive)
}

void AI::playFutureTurn(int depth, std::vector<std::pair<Unit *, int> > & units, int money, std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > &builtUnits)
{
	// case : list of units, total cost, total rating
	std::vector<std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > > newUnits;
	std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > passUnits;
	for(unsigned int i=0;i<builtUnits.size();i++){
		int balance = money - builtUnits[i].second.first + Game::getInstance()->computeIncome(this->team);
		//if(balance > 0){
			pair<vector<Unit*>,int> p = make_pair(builtUnits[i].first,builtUnits[i].second.second); //int is rating
			std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > purchases;
			purchases = this->makePurchases(balance,p);
			passUnits.insert(passUnits.end(),purchases.begin(),purchases.end());
			newUnits.push_back(purchases);
		//}

	}
	//TODO money math is probably wrong
	//TODO unit building recursion is overkill

	vector<vector<pair<Unit*,int> > > futureTurn;
	for(unsigned int i=0;i<units.size();i++){ // generating possibilities for every unit
		Unit* un = units[i].first;
		vector<vector<int> > poss = this->moveUnit(un);
		//int rating = units[i].second + pos[2]; // rating before this turn + rating on this turn
		//if(depth < MAXDEPTH){
			vector<pair<Unit*,int> > futureUnits;
			for(unsigned int j=0;j<poss.size();j++){
				int rating = units[i].second + poss[j][2]; // rating before this turn + rating on this turn
				vector<Unit*> targets = this->targetsFromPos(poss[j][0],poss[j][1]);
				if(!targets.empty()){ //tests attack
					for(unsigned int k=0;k<targets.size();k++){
						rating += this->rateAttack(un,targets[k]);
					}
				}
				futureUnits.push_back(make_pair(this->buildUnit(poss[j][0],poss[j][1],un->getUnitType()),rating));
			}
			this->choosePossibilitiesToExplore(futureUnits);
			futureTurn.push_back(futureUnits);
			//this->playFutureTurn(depth+1,futureUnits,money +Game::getInstance()->computeIncome(this->team),passUnits);
/*
			// overwrite the ratings when taking into account future moves (choosing the best for each unit)
			int max = -1;
			int maxI;
			for(unsigned int j=0;j<futureUnits.size();j++){ //TODO use a data structure for max search
				if(futureUnits[j].second > max){
					maxI = j;
					max = futureUnits[j].second;
				}
			}
			units[i].second = futureUnits[maxI].second;
			for(unsigned int j=0;j<futureUnits.size();j++){
				delete futureUnits[j].first; // no memory leak
			}
		}else{	// updates the ratings with the best possibility as final depth
			int max = -1;
			int maxI;
			for(unsigned int j=0;j<poss.size();j++){ //TODO use a data structure for max search
				if(poss[j][2] > max){
					maxI = j;
					max = poss[j][2];
				}
			}
			units[i].second += poss[maxI][2];
		}*/

	}
	//modify ratings of variable units

	this->make_sequence(depth,futureTurn,money +Game::getInstance()->computeIncome(this->team),passUnits);
	for(unsigned int i=0;i<futureTurn.size();i++){
		if(depth < MAXDEPTH){ // evaluates possibilities in future moves
	//		this->playFutureTurn(depth+1,futureTurn[i],money +Game::getInstance()->computeIncome(this->team),passUnits);
		}

		// chooses the best possibility
		int max = -1;
		int maxI;
		for(unsigned int j=0;j<futureTurn[i].size();j++){ //TODO use a data structure for max search
			if(futureTurn[i][j].second > max){
				maxI = j;
				max = futureTurn[i][j].second;
			}
		}
		units[i].second = futureTurn[i][maxI].second;
		for(unsigned int j=0;j<futureTurn[i].size();j++){
			delete futureTurn[i][j].first; // no memory leak
		}
	}


	//updates the unit building with the best possibility
	for(unsigned int i=0;i<builtUnits.size();i++){
		int umax = -1;
		int umaxI;
		for(unsigned int j=0;j<newUnits[i].size();j++){ //TODO use a data structure for max search
			if(newUnits[i][j].second.second > umax){
				umaxI = j;
				umax = newUnits[i][j].second.second;
			}
			// + cleanup work
			vector<Unit*>& toClean = newUnits[i][j].first;
			for(unsigned int k=0;k<toClean.size();k++){
				delete toClean[k];
			}
		}
		builtUnits[i].second.second += newUnits[i][umaxI].second.second;
	}
}

void AI::choosePossibilitiesToExplore(std::vector<std::pair<Unit*,int> >& poss){
	// reduces the size of input
	float reduction = (float)1/4;
	if(poss.size() > 4){ //try different values
		unsigned int newSize = poss.size() * reduction;
		vector<pair<Unit*,int> >::iterator it;
		it = poss.begin();
		while(poss.size() != newSize){
			if(0 == rand()%3){ //maybe try different values
				//delete (*it).first; //TODO memory leak
				poss.erase(it);
			}
			it++;
			if(it == poss.end()){
				it = poss.begin();
			}
		}
	}
}

std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > AI::makePurchases(int money, std::pair<std::vector<Unit *>, int> & buildCase)
{	 //TODO make use of input
	std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > result; //TODO internal variable
	result.push_back(make_pair(vector<Unit*>(),make_pair(money,0))); //can always build nothing

	int fac_cnt =0;
	int air_cnt =0;
	vector<Factory*> fac_lst;
	vector<Airport*> air_lst;
	for(unsigned int i=0;i<this->factories.size();i++){
		if(this->factories[i]->getOwner() == this->team){
			if(Airport* air = dynamic_cast<Airport*>(this->factories[i])){
				air_cnt++;
				air_lst.push_back(air);
			}else{
				fac_cnt++;
				fac_lst.push_back(this->factories[i]);
			}
		}
	}
	vector<int> cs;
	this->buildCase.clear();
	this->generatePurchasePossibilities(money,fac_cnt,air_cnt,cs);
	//TODO misses no builts

	/*
	int newCase[this->factories.size()]; //TODO more cases
	int totalRating;
	for(unsigned int i=0;i<this->factories.size();i++){
		newCase[i] = -1; // default = no build
		int rating = -1;
		if(Airport* air = dynamic_cast<Airport*>(this->factories[i])){
			for(int j=8;j<11;j++){
				int newR = this->ratePurchase(j,money);
				if(newR > rating){
					newCase[i] = j;
					rating = newR;
				}
			}
		}else{
			for(int j=0;j<8;j++){
				int newR = this->ratePurchase(j,money);
				if(newR > rating){
					newCase[i] = j;
					rating = newR;
				}
			}
		}
		totalRating += rating;
	}
	int cost = 0;
	vector<Unit*> units;
	for(unsigned int i=0;i<this->factories.size();i++){
		int x = this->factories[i]->getPosX();
		int y = this->factories[i]->getPosY();
		units.push_back(this->buildUnit(x,y,newCase[i]));
		cost += units[i]->getPrice();
	}
	result.push_back(make_pair(units,make_pair(cost,totalRating)));*/

	for(unsigned int i=0;i<this->buildCase.size();i++){
		if(this->buildCase[i].second <= money){ //TODO do better, directly in generator
			int f_pos = 0;
			int a_pos = 0;
			int money_left = money;
			int rating = 0;
			vector<Unit*> v;
			for(unsigned int j=0;j<this->buildCase[i].first.size();j++){
				int type = this->buildCase[i].first[j];
				rating += this->ratePurchase(type,money_left);
				if(type<8 && f_pos < fac_cnt){ //idk if correct
					int x = fac_lst[f_pos]->getPosX();		// no intelligence for factory choice
					int y = fac_lst[f_pos]->getPosY();
					v.push_back(this->buildUnit(x,y,type));
					f_pos++;
				}else if(a_pos < air_cnt){ //idk if correct
					int x = air_lst[a_pos]->getPosX();
					int y = air_lst[a_pos]->getPosY();
					v.push_back(this->buildUnit(x,y,type));
					a_pos++;
				}
				money_left -= this->unitCost[j];
			}
			result.push_back(make_pair(v,make_pair(this->buildCase[i].second,rating)));
		}
	}

	return result;
}

void AI::generatePurchasePossibilities(int money, int facN, int airN, std::vector<int> & cs)
{
	bool built = false;
	if(facN > 0){
		for(int i=0;i<8;i++){
			if(money < unitCost[i]){
				cs.push_back(i);
				built = true;
				this->generatePurchasePossibilities(money-unitCost[i],facN-1,airN,cs);
			}
		}
		if(!built){
			this->generatePurchasePossibilities(money,0,airN,cs);
		}
	}else if(airN > 0){
		for(int i=8;i<11;i++){
			if(money < unitCost[i]){
				cs.push_back(i);
				built = true;
				this->generatePurchasePossibilities(money-unitCost[i],facN,airN-1,cs);
			}
		}
		if(!built){
			this->generatePurchasePossibilities(money,facN,0,cs);
		}
	}else{
		int cost =0;
		for(unsigned int i=0;i<cs.size();i++){
			cost += unitCost[cs[i]];
		}
		this->buildCase.push_back(make_pair(cs,cost));
	}
}

std::vector<std::vector<int> > AI::moveUnit(Unit * un)
{
	vector<vector<int> > possibilities;
	vector<ValidMove*> moves = un->selected();
	moves.push_back(new ValidMove(un->getPosX(),un->getPosY(),false)); //can also not move
	vector<int> results;
	for(unsigned int i=0;i<moves.size();i++){
		results.push_back(this->rateAction(un,moves[i]));
		vector<int> v;
		v.push_back(moves[i]->getPosX());
		v.push_back(moves[i]->getPosY());
		v.push_back(results[i]);
		possibilities.push_back(v);
		delete moves[i];
	}
	//cout<< "All move possibilities rated "<<moves.size()<<" "<<results.size()<<endl;
	return possibilities;
}

int AI::rateAction(Unit * un, ValidMove * vm)
{
	int rating = 0;
	vector<GameObject*>& tile = Game::getInstance()->getObjectsOnPos(vm->getPosX(),vm->getPosY());
	for(unsigned int i=0;i<tile.size();i++){
		if(City* ct = dynamic_cast<City*>(tile[i])){
			if(ct->getOwner() == un->getTeam()){
				if(un->getHealth() != 10){
					rating += 3;	//unit can heal
				}
			}else{
				if(un->getType() == "InfantryUnit"){
					rating += un->getHealth();	//city capture + also depends on health
				}
			}
			break;
		}
		if(Unit* unit = dynamic_cast<Unit*>(tile[i])){
			if(unit->getTeam() == this->team && un->getHealth() + unit->getHealth() <= 10){
				rating += 3; // unit fusion
			}
		}
	}
	rating += Game::getInstance()->getTerrainDefenseModifier(*un,vm->getPosX(),vm->getPosY());
	if(vm->isLast()){ // only rates completes moves for now
		return rating+1;
	}
	if(un->getTeam() != this->team){
		rating *= -1; //enemy moves are rated negatively
	}
	return rating;
}

int AI::ratePurchase(int type, int money)
{
	const int ratings[11] = {3,1,3,4,6,5,4,5,6,2,2};
	//very stupid for now
	//Unit* un = this->buildUnit(0,0,type);
	if(this->unitCost[type] > money){
		return -10;
	}else{
		return ratings[type];
	}
}

int AI::rateAttack(Unit * un, Unit * target)
{
	if(un->getUnitType() >= target->getUnitType()){ //TODO very stupid logic
		return 5;
	}else{
		return 2;
	}

}

vector<Unit*> AI::targetsFromPos(int x, int y)
{
	vector<Unit*> targets;
	vector<Unit*> v1 = this->searchForUnits(x+1,y);
	vector<Unit*> v2 = this->searchForUnits(x-1,y);
	vector<Unit*> v3= this->searchForUnits(x,y+1);
	vector<Unit*> v4 = this->searchForUnits(x,y-1);
	targets.insert(targets.end(),v1.begin(),v1.end());
	targets.insert(targets.end(),v2.begin(),v2.end());
	targets.insert(targets.end(),v3.begin(),v3.end());
	targets.insert(targets.end(),v4.begin(),v4.end());
	return targets;
}

std::vector<Unit *> AI::searchForUnits(int x, int y)
{
	vector<Unit*> units;
	vector<GameObject*>& tile = Game::getInstance()->getObjectsOnPos(x,y);
	for(unsigned int i=0;i<tile.size();i++){
		if(Unit* unit = dynamic_cast<Unit*>(tile[i])){ //doesnt work in one line
			if(unit->getTeam() != this->team){
				units.push_back(unit);
			}
		}
	}
	return units;
}

void AI::executeAction(Unit * un, int x, int y, std::vector<std::vector<int> > attacks)
{
	cout<<"Chosen the best from "<<un->getPosX()<<" "<<un->getPosY()<<" to "<<x<<" "<<y<<", executing"<<endl;
	Game::getInstance()->selectUnit(un);
	//cout<<"Selected"<<endl;
	Game::getInstance()->moveTo(x,y); //second unit causes a segfault in last-move size = 0
			// infantry commented in game
			// calling getType on Infantry calls Unit::interactWith ?????
	for(unsigned int i=0;i<attacks.size();i++){
		if(attacks[i][0] == x && attacks[i][1] == y){ // TODO choose best if more units
			int delX = x - attacks[i][2];
			int delY = y - attacks[i][3];
			if(delX == 0){
				if(delY > 0){
					Game::getInstance()->attack(0);
				}else{
					Game::getInstance()->attack(1);
				}
			}else if(delX > 0){
				Game::getInstance()->attack(2);
			}else{
				Game::getInstance()->attack(3);
			}
		}
	}
	Game::getInstance()->clearValidMoves();
}

void AI::buyUnits(std::vector<Unit *> units)
{
	for(unsigned int i=0;i<units.size();i++){
		Game::getInstance()->click(units[i]->getPosX(),units[i]->getPosY());
		Game::getInstance()->buyUnit(units[i]->getUnitType());
	}
}

Unit* AI::buildUnit(int x, int y, int type) // unit differentiator
{
	Unit* un;
	switch(type){
		case 0: case 1:
			un = new Infantry(x,y,type,this->team);
			break;
		case 2: case 3: case 4: case 5: case 6: case 7:
			un = new Unit(x,y,type,this->team);
			break;
		case 8: case 9: case 10:
			un = new AirUnit(x,y,type,this->team);
			break;
		default:
			un = NULL; //error
	}
	return un;
}

void AI::errorString(QString str)
{
	cout<<"Error: "<<str.toStdString()<<endl;
}

void AI::threadEnd()
{
	this->th_done++;
}

