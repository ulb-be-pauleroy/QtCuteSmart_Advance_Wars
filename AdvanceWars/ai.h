#ifndef AI_H
#define AI_H

#include <vector>
#include <QThread>
#include "game.h"

#define MAXDEPTH 3

class AI : public QObject
{
	Q_OBJECT

	char team;
	const std::vector<Unit*>* myUnits;
	std::vector<Factory*> factories;

	static const int unitCost[];
	std::vector<std::pair<std::vector<int>, int> > buildCase;

	int myMoney;
	int enemyMoney;

	bool meOnTurn;
	std::vector<std::vector<std::pair<Unit *, int> > >* myCases[MAXDEPTH];
	std::vector<std::vector<std::pair<Unit *, int> > >* enemyCases[MAXDEPTH];

	int th_cnt;
	int th_done;

public:
	AI(char,const std::vector<Unit*>*, const std::vector<Building*>&);
	virtual ~AI() {} //memory leak
	void play();

public slots:
	void errorString(QString);
	void threadEnd();

protected:
	void make_sequence(int depth, std::vector<std::vector<std::pair<Unit *, int> > > &units, int money, std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > &builtUnits);
	void playFutureTurn(int,std::vector<std::pair<Unit*, int> >&,int,std::vector<std::pair<std::vector<Unit*>, std::pair<int,int> > >&);
	std::vector<std::pair<std::vector<Unit*>, std::pair<int, int> > > makePurchases(int,std::pair<std::vector<Unit*>, int>&);
	void generatePurchasePossibilities(int,int,int,std::vector<int>&);
	std::vector<std::vector<int> > moveUnit(Unit*);
	int rateAction(Unit*,ValidMove*);
	int ratePurchase(int,int);
	int rateAttack(Unit*,Unit*);
	std::vector<Unit *> targetsFromPos(int,int);
	std::vector<Unit *> searchForUnits(int,int);
	void executeAction(Unit*,int, int,std::vector<std::vector<int> >);
	void buyUnits(std::vector<Unit*>);
	Unit* buildUnit(int,int,int);
};

#endif // AI_H
