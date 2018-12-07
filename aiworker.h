#ifndef AIWORKER_H
#define AIWORKER_H

#include "ai.h"

class AIWorker : public AI
{
	Q_OBJECT

	std::pair<Unit*,int> work;
	std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > units;
	int money;

public:
	explicit AIWorker(std::pair<Unit*,int>, char,int,std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > >);

public slots:
	void process();

signals:
	void finished();
	void error(QString err);

};

#endif // AIWORKER_H
