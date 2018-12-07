#ifndef AI_H
#define AI_H

#include <vector>
#include <QThread>
#include "game.h"

#define MAXDEPTH 8

class AI : public QObject
{
	Q_OBJECT

	char myTeam;
	char enemyTeam;
	const std::vector<Unit*>* myUnits;
	std::vector<Factory*> factories;

	static const int unitCost[];
	static const int dmg_chart[11][11];
	std::vector<std::pair<std::vector<int>, int> > buildCase;
	std::vector<std::pair<int,int> > alreadyMovedTo;

	int myMoney[MAXDEPTH];
	int enemyMoney[MAXDEPTH];

	bool meOnTurn;
	std::vector<std::vector<std::pair<Unit *, int> > > myCases[MAXDEPTH];
	std::vector<std::vector<std::pair<Unit *, int> > > enemyCases[MAXDEPTH];
	std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > myBuildCases[MAXDEPTH];
	std::vector<std::pair<std::vector<Unit *>, std::pair<int, int> > > enemyBuildCases[MAXDEPTH];

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
	void choosePossibilitiesToExplore(std::vector<std::pair<Unit*,int> >&);
	std::vector<std::pair<std::vector<Unit*>, std::pair<int, int> > > makePurchases(int,std::pair<std::vector<Unit*>, int>&);
	void generatePurchasePossibilities(int,int,int,std::vector<int>&);
	std::vector<std::vector<int> > moveUnit(Unit*);
	bool isFree(const int,const int);
	int rateAction(Unit*,ValidMove*);
	int ratePurchase(int,int,const std::vector<int>&);
	int rateAttack(Unit*,Unit*);
	std::vector<Unit *> targetsFromPos(int,int);
	std::vector<Unit *> searchForUnits(int,int);
	void executeAction(Unit*,int, int,std::vector<std::vector<int> >);
	void buyUnits(std::vector<Unit*>);
	Unit* buildUnit(int,int,int);
	char getActiveTeam() const;
	void setTeam(char me);
};

#endif // AI_H
