#include "ai.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h> //for rand

#include "aiworker.h"
#include "airport.h"
#include "airunit.h"
#include "infantry.h"
#include "city.h"

using namespace std;

// dmg_chart[attacker][target]
// 0 = infantry, 1 = bazooka, 2 = recon, 3 = anti-air
// 4 = tank, 5 = md. tank, 6 = mega tank, 7 = neotank
// 8 = b-copter, 9 = fighter, 10 = bomber
const int AI::dmg_chart[11][11] = {{55,45,12,5,5,1,1,1,7,0,0},
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

const int AI::unitCost[11] = {1000,3000,4000,8000,7000,16000,28000,22000,9000,20000,22000};

AI::AI(int type, char team, const std::vector<Building *>& buildings)
{
	this->setTeam(team);
    this->type = type;
	//this->myUnits = units;


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
	this->myMoney[0] = Game::getInstance()->getBalance(this->myTeam);

    if(this->type != 0){
        std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > newUnits;
        //for(unsigned int i=0;i<builtUnits.size();i++){
        vector<Unit*> v;
        v.push_back(NULL);
        pair<vector<Unit*>, int> p(v,0);
        std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > purchases = this->makePurchases(this->myMoney[0],p); //make_pair(v,0));
        newUnits.insert(newUnits.end(),purchases.begin(),purchases.end());
        //}

        vector<Unit*>& myUnits = * Game::getInstance()->getUnits(this->myTeam);
        vector<vector<pair<Unit*,int> > > futureTurn;
        vector<vector<int> > attPos;
        // no unit cooperation for now
        for(unsigned int i=0;i<myUnits.size();i++){
            Unit* un = myUnits[i];
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
                AIWorker* worker = new AIWorker(this->type,futureUnits[j],this->myTeam,this->myMoney[0],newUnits); //TODO multithreading
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

        this->make_sequence(0,futureTurn,this->myMoney[0],newUnits);
        this->alreadyMovedTo.clear();
        for(unsigned int i=0;i<futureTurn.size();i++){
            int max = -1;
            int maxI;
            for(unsigned int j=0;j<futureTurn[i].size();j++){ //TODO use a data structure for max search
                if(futureTurn[i][j].second > max && isFree(futureTurn[i][j].first->getPosX(),futureTurn[i][j].first->getPosY())){
                    maxI = j;
                    max = futureTurn[i][j].second;
                }
            }
            if(max != -1) this->executeAction(myUnits[i],futureTurn[i][maxI].first->getPosX(),futureTurn[i][maxI].first->getPosY(),attPos);
            // it can happen, that a unit only has one valid move that is already taken
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
        for(unsigned int j=0;j<newUnits.size();j++){
            // + cleanup work
            vector<Unit*>& toClean = newUnits[j].first;
            for(unsigned int k=0;k<toClean.size();k++){
                delete toClean[k];
            }
        }
    }
    cout<<"AI ending turn"<<endl;
    Game::getInstance()->endTurn();
}

void AI::make_sequence(int depth, std::vector<std::vector<std::pair<Unit *, int> > > & units, int money, std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > &builtUnits){
	//save sequence
	if(this->meOnTurn){
		this->myCases[depth] = units; //we want a copy !!!
		this->myBuildCases[depth] = builtUnits;
		this->myMoney[depth] = money;
	}else{
		this->enemyCases[depth] = units; //copy problem should be solved
		this->enemyBuildCases[depth] = builtUnits;
		this->enemyMoney[depth] = money;
	}
	this->meOnTurn = !this->meOnTurn;
	//launch enemy turn
	if(this->meOnTurn && depth < MAXDEPTH){ // my depth is the same as enemy-s
		for(unsigned int i=0;i<this->myCases[depth-1].size();i++){
			this->playFutureTurn(depth,(this->myCases[depth-1])[i],this->myMoney[depth]+Game::getInstance()->computeIncome(this->myTeam),this->myBuildCases[depth]);
		}
	}else if(depth < MAXDEPTH -1){
		if(depth == 0){ //first iteration
			std::vector<std::pair<Unit *, int> > dummy;
			std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > buildDummy;
			this->playFutureTurn(depth+1,dummy,Game::getInstance()->getBalance(this->enemyTeam),buildDummy);
		}else{
			for(unsigned int i=0;i<this->enemyCases[depth].size();i++){
				this->playFutureTurn(depth+1,(this->enemyCases[depth])[i],this->enemyMoney[depth]+Game::getInstance()->computeIncome(this->enemyTeam),this->enemyBuildCases[depth]);
			}
		}
	}
	//decrement rating based on opponent
	this->meOnTurn = !this->meOnTurn;
	if(this->meOnTurn && depth < MAXDEPTH -1){ // enemy didnt play next turn at depth end
		int enemyRating;
		for(unsigned int i=0;i<enemyCases[depth+1].size();i++){
			int imax = -1;
			int imaxI;
			for(unsigned int j=0;j<(enemyCases[depth+1])[i].size();j++){ //TODO use a data structure for max search
				if((enemyCases[depth+1])[i][j].second > imax){
					imaxI = j;
					imax = (enemyCases[depth+1])[i][j].second;
				}
			}
			//TODO check for memory leak
			enemyRating += imax;
		}
		for(unsigned int i=0;i<myCases[depth].size();i++){
			for(unsigned int j=0;j<myCases[depth][i].size();j++){
				(myCases[depth])[i][j].second -= enemyRating;
			}
		}
	}else if(!this->meOnTurn && depth > 0){
		int enemyRating;
		for(unsigned int i=0;i<myCases[depth].size();i++){
			int imax = -1;
			int imaxI;
			for(unsigned int j=0;j<(myCases[depth])[i].size();j++){ //TODO use a data structure for max search
				if((myCases[depth])[i][j].second > imax){
					imaxI = j;
					imax = (myCases[depth])[i][j].second;
				}
			}
			//TODO check for memory leak
			enemyRating += imax;
		}
		for(unsigned int i=0;i<enemyCases[depth].size();i++){
			for(unsigned int j=0;j<(enemyCases[depth])[i].size();j++){
				(enemyCases[depth])[i][j].second -= enemyRating;
			}
		}
    }

}

void AI::playFutureTurn(int depth, std::vector<std::pair<Unit *, int> > & units, int money, std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > &builtUnits)
{
	// case : list of units, total cost, total rating
	std::vector<std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > > newUnits;
	std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > passUnits;
	for(unsigned int i=0;i<builtUnits.size();i++){
		int balance = money - builtUnits[i].second.first;
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

	char team = this->getActiveTeam();
	this->make_sequence(depth,futureTurn,money +Game::getInstance()->computeIncome(team),passUnits);
	for(unsigned int i=0;i<futureTurn.size();i++){
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
                delete toClean[k]; //leaves the base class in memory
			}
		}
		builtUnits[i].second.second += newUnits[i][umaxI].second.second;
	}
}

void AI::choosePossibilitiesToExplore(std::vector<std::pair<Unit*,int> >& poss){
	// reduces the size of input
	float reduction = (float)1/4;
	if(poss.size() > 12){ //try different values
		unsigned int newSize = poss.size() * reduction;

		int maxVal[3]; maxVal[0] = -1; maxVal[1] = -1; maxVal[2] = -1;
		//int maxValCnt[3];
		unsigned int maxI[3];
		int minI = 0;
		for(unsigned int i=0;i<poss.size();i++){
			if(poss[i].second > maxVal[minI]){
				maxVal[minI] = poss[i].second;
				//maxValCnt[minI] =1;
				maxI[minI] = i;
				int min = 10000;
				for(int j=0;j<3;j++){
					if(maxVal[j] < min){
						min = maxVal[j];
						minI = j;
					}
				}
			}else{/*
				for(int j=0;j<3;j++){
					if(poss[i].second = maxVal[j]){
						maxValCnt[j]++;
					}
				}*/
			}
		}

		vector<pair<Unit*,int> >::iterator it;
		it = poss.begin();
		unsigned int pos = 0;
		while(poss.size() != newSize){
			if(0 == rand()%3){ //maybe try different values
				if(pos != maxI[0] && pos != maxI[1] && pos != maxI[2]){
					vector<pair<Unit*,int> >::iterator itr = poss.begin() + pos;
					delete poss[pos].first; //no memory leak
					poss.erase(itr);
				}
				/*
				bool notMax = true;
				for(unsigned int i=0;i<3;i++){
					if((*it).first->getPosX() == poss[maxI[i]].first->getPosX() && (*it).first->getPosY() == poss[maxI[i]].first->getPosY()){
						notMax = false; break;
					}
				}
				if(notMax){
					//delete (*it).first; //TODO memory leak
					poss.erase(it);
				//}*/
			}
			pos++;
			if(pos >= poss.size()) pos = 0;
			/*
			it++;
			if(it == poss.end()){
				it = poss.begin();
			}*/
		}
	}
}

std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > AI::makePurchases(int money, std::pair<std::vector<Unit *>, int> & buildCase)
{	 //TODO make use of input
	std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > result;
	result.push_back(make_pair(vector<Unit*>(),make_pair(money,0))); //can always build nothing

	char team = this->getActiveTeam();

	int fac_cnt =0;
	int air_cnt =0;
	vector<Factory*> fac_lst;
	vector<Airport*> air_lst;
	for(unsigned int i=0;i<this->factories.size();i++){
		if(this->factories[i]->getOwner() == team){
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

	char enemy;
	if(!this->meOnTurn){
		enemy = this->myTeam;
	}else{
		enemy = this->enemyTeam;
	}
	vector<Unit*>* enemyUnits = Game::getInstance()->getUnits(enemy);
	vector<int> enemyUnitsTypes;
	for(unsigned int i=0;i<enemyUnits->size();i++){
		enemyUnitsTypes.push_back((*enemyUnits)[i]->getUnitType());
	}

	for(unsigned int i=0;i<this->buildCase.size();i++){
		if(this->buildCase[i].second <= money){ //TODO do better, directly in generator
			int f_pos = 0;
			int a_pos = 0;
			int money_left = money;
			int rating = 0;
			vector<Unit*> v;
			for(unsigned int j=0;j<this->buildCase[i].first.size();j++){
				int type = this->buildCase[i].first[j];
				rating += this->ratePurchase(type,money_left,enemyUnitsTypes);
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
            if(money > unitCost[i]){
				cs.push_back(i);
				built = true;
				this->generatePurchasePossibilities(money-unitCost[i],facN-1,airN,cs);
				vector<int>::iterator it = cs.end()-1; // roll back the choice
				cs.erase(it);
			}
		}
		if(!built){
			this->generatePurchasePossibilities(money,0,airN,cs);
		}
	}else if(airN > 0){
		for(int i=8;i<11;i++){
            if(money > unitCost[i]){
				cs.push_back(i);
				built = true;
				this->generatePurchasePossibilities(money-unitCost[i],facN,airN-1,cs);
				vector<int>::iterator it = cs.end()-1; // roll back the choice
				cs.erase(it);
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

bool AI::isFree(const int x, const int y){
	for(unsigned int i=0;i<this->alreadyMovedTo.size();i++){
		if(this->alreadyMovedTo[i].first == x && this->alreadyMovedTo[i].second == y) return false;
	}
	return true;
}

int AI::rateAction(Unit * un, ValidMove * vm)
{
	char team = this->getActiveTeam();

	int rating = 0;
	vector<GameObject*>& tile = Game::getInstance()->getObjectsOnPos(vm->getPosX(),vm->getPosY());
	for(unsigned int i=0;i<tile.size();i++){
		if(Building* bld = dynamic_cast<Building*>(tile[i])){
            rating += 20;
			if(bld->getOwner() == un->getTeam()){
				if(un->getHealth() != 10){
                    rating += 50;	//unit can heal
				}
			}else{
				if(un->getType() == "InfantryUnit"){
					rating += 5*un->getHealth();	//building capture + also depends on health
					if(un->getPosX() == vm->getPosX() && un->getPosY() == vm->getPosY()){
                        rating += 30; // continuing capture
					}
					if(bld->getOwner() != '\0') rating += 20; //capturing enemy building
				}
			}
			break;
		}
		if(Unit* unit = dynamic_cast<Unit*>(tile[i])){
			if(unit->getTeam() == team && un->getHealth() + unit->getHealth() <= 10){
				rating += 3; // unit fusion
			}
		}
	}
	rating += Game::getInstance()->getTerrainDefenseModifier(*un,vm->getPosX(),vm->getPosY());
	if(vm->isLast()){ // only rates completes moves for now
		return rating+1;
	}
	return rating;
}

int AI::ratePurchase(int type, int money, const std::vector<int> & enemyUnits)
{
	//Unit* un = this->buildUnit(0,0,type);
	if(this->unitCost[type] > money){
		return 0;
	}else if(enemyUnits.size() == 0){ //when AI opening game
		const int ratings[11] = {3,1,5,4,6,5,4,5,6,2,2}; // random numbers
		return ratings[type];
	}else{
		int rating =0;
		for(unsigned int i=0;i<enemyUnits.size();i++){
			rating += this->dmg_chart[type][enemyUnits[i]];
		}
		return rating;
	}
}

int AI::rateAttack(Unit * un, Unit * target)
{
    int rating = this->dmg_chart[un->getUnitType()][target->getUnitType()] /1; //TODO try different values
	if(un->getUnitType() >= target->getUnitType()){
		rating += 5;
	}
	return rating;
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
	char team = this->getActiveTeam();

	vector<Unit*> units;
	vector<GameObject*>& tile = Game::getInstance()->getObjectsOnPos(x,y);
	for(unsigned int i=0;i<tile.size();i++){
		if(Unit* unit = dynamic_cast<Unit*>(tile[i])){ //doesnt work in one line
			if(unit->getTeam() != team){
				units.push_back(unit);
			}
		}
	}
	return units;
}

void AI::executeAction(Unit * un, int x, int y, std::vector<std::vector<int> > attacks)
{
	cout<<"Chosen the best from "<<un->getPosX()<<" "<<un->getPosY()<<" to "<<x<<" "<<y<<", executing"<<endl;
	this->alreadyMovedTo.push_back(make_pair(x,y));
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
        Game::getInstance()->buyUnit(units[i]->getUnitType()+1); // setup to use keyboard
	}
}

Unit* AI::buildUnit(int x, int y, int type) // unit differentiator
{
	char team = this->getActiveTeam();

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

char AI::getActiveTeam() const{
	if(this->meOnTurn){
		return this->myTeam;
	}else{
		return this->enemyTeam;
	}
}

void AI::setTeam(char me){
	if(me == 'o'){
		this->myTeam = 'o';
		this->enemyTeam = 'b';
	}else{
		this->myTeam = 'b';
		this->enemyTeam = 'o';
	}
}

void AI::errorString(QString str)
{
	cout<<"Error: "<<str.toStdString()<<endl;
}

void AI::threadEnd()
{
	this->th_done++;
}

