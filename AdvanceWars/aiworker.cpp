#include "aiworker.h"
#include <iostream>

AIWorker::AIWorker(std::pair<Unit *, int> unit_case, char team, int mon, std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > newUn) : AI(team,std::vector<Building*>())
{	// TODO multithreading that works
	this->work = unit_case;
	this->money = mon;
	this->units = newUn;
	emit finished();
}

void AIWorker::process()
{
	std::vector<std::pair<Unit*,int> > dat;
	dat.push_back(this->work);
/*
	std::vector<Unit*>* enemyUnits = Game::getInstance()->getUnits('o'); //TODO parameter
	std::vector<std::pair<Unit*,int> > futureEnemyUnits;
	for(unsigned int i=0;i<enemyUnits->size();i++){
		Unit* en_un = enemyUnits->at(i); //TODO
		futureEnemyUnits.push_back(std::make_pair(this->buildUnit(en_un->getPosX(),en_un->getPosY(),en_un->getUnitType(),en_un->getTeam()),0));
		//vector<vector<int> > en_poss = this->moveUnit(enemyUnits->at(i));
	}
	this->playFutureTurn(1,'o',dat,futureEnemyUnits);
*/
	//std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > builtUnits;
	this->playFutureTurn(1,dat,this->money,this->units);
	//std::cout<<"Jobs done"<<std::endl;
	emit finished();
}

