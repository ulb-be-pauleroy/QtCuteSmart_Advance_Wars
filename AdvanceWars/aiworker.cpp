#include "aiworker.h"
#include <iostream>

AIWorker::AIWorker(int type, std::pair<Unit *, int> unit_case, char team, int mon, std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > newUn) : AI(type, team,std::vector<Building*>())
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
	this->playFutureTurn(1,dat,this->money,this->units);
	emit finished();
}

