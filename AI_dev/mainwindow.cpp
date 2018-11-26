#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->blk_size = 40;
	this->setGeometry(100,100,this->blk_size*XDIM +5,this->blk_size*YDIM +25);//800,800);
	this->game = NULL;
	this->network = NULL;
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::keyPressEvent(QKeyEvent * event)
{
	if(!this->network || (this->network->getTeam() == game->getTeamOnTurn())){
	switch(event->key()){
		case Qt::Key_Up: game->move(0); break;
		case Qt::Key_Down: game->move(1); break;
		case Qt::Key_Left: game->move(2); break;
		case Qt::Key_Right: game->move(3); break;
		//case Qt::Key_N: game->addUnit(1,1,'o'); break;
		case Qt::Key_A: game->setAttack(); break;
		case Qt::Key_Tab: game->cycleUnits(1); break;
		case Qt::Key_Space: game->endTurn(); break;

		case Qt::Key_1: game->buyUnit(1); break;
		case Qt::Key_2: game->buyUnit(2); break;
		case Qt::Key_3: game->buyUnit(3); break;
		case Qt::Key_4: game->buyUnit(4); break;
		case Qt::Key_5: game->buyUnit(5); break;
		case Qt::Key_6: game->buyUnit(6); break;
		case Qt::Key_7: game->buyUnit(7); break;
		case Qt::Key_8: game->buyUnit(8); break;

	}

	this->update();
	}
}

void MainWindow::mousePressEvent(QMouseEvent* event){
	if(!this->network || (this->network->getTeam() == game->getTeamOnTurn())){
	int x = event->x();
	int y = event->y();
	//std::cout<<x<<" "<<y<<" "<< event->button() <<std::endl;
	// 1 for left click, 2 for right click, 4 for middle click
	if(event->button()==1){
		this->game->click(x/this->blk_size,y/this->blk_size);
	}else if(event->button()==2){
		this->game->moveTo(x/this->blk_size,y/this->blk_size);
	}
	this->update();
	}
}

void MainWindow::wheelEvent(QWheelEvent * event){
	//std::cout<<event->delta()<<std::endl;
	// +-120 for scrolling up or down
	if(!this->network || (this->network->getTeam() == game->getTeamOnTurn())){
	if(event->delta() > 0){
		this->game->cycleUnits(-1);
	}else{
		this->game->cycleUnits(1);
	}
	this->update();
	}
}

void MainWindow::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

	if(this->game){
	//int blk_size = 40;
	for(int i=0;i<XDIM;i++){
		for(int j=0;j<YDIM;j++){
			//painter.setBrush(QBrush(Qt::black));
			painter.drawRect(this->blk_size*i,this->blk_size*j,this->blk_size,this->blk_size);
			painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size-2,this->blk_size-2,QBrush(Qt::gray));
		}
	}
	//map refreshing

	painter.setBrush(QBrush(Qt::green));
	painter.drawRect(this->blk_size*game->getSelectedX(),this->blk_size*game->getSelectedY(),this->blk_size,this->blk_size);
   // painter.fillRect(50,10,50,50,QBrush(Qt::red));

	for(int i=0;i<XDIM;i++){
		for(int j=0;j<YDIM;j++){

			std::vector<GameObject*> obj_onPos = this->game->getObjectsOnPos(i,j);
			std::vector<GameObject*>::iterator it;
			for(it=obj_onPos.begin();it!=obj_onPos.end();it++){
				GameObject* go = *it;

				int color = go->getColor();
				if(color == 1){
					painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size-2,this->blk_size-2,QBrush(Qt::yellow));
				}else if(color == 2){
					painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size-2,this->blk_size-2,QBrush(Qt::blue));
				}else if(color == 3){
					painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size-2,this->blk_size-2,QBrush(Qt::cyan));
				}else if(color == 4){
					painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size-2,this->blk_size-2,QBrush(Qt::darkGreen));
				}else if(color == 5){
					painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size-2,this->blk_size-2,QBrush(Qt::red));
				}
			}

		}
	}
	}//std::cout<<"Here5"<<std::endl;
}

void MainWindow::receiveGame(Game* gm)
{
	this->game = gm;
	this->update();
}

void MainWindow::receiveNetwork(Network *net)
{
	this->network = net;
}
