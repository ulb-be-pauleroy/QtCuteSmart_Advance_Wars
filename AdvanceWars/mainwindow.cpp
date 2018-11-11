#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->blk_size = 40;
	this->game = NULL;
	this->network = NULL;
}



MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::keyPressEvent(QKeyEvent * event)
{
    //if(this->network->getTeam() == game->getTeamOnTurn()){
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
    //}
}

void MainWindow::mousePressEvent(QMouseEvent* event){
    //if(this->network->getTeam() == game->getTeamOnTurn()){
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
    //}
}

void MainWindow::wheelEvent(QWheelEvent * event){
	//std::cout<<event->delta()<<std::endl;
	// +-120 for scrolling up or down
    if(this->network->getTeam() == game->getTeamOnTurn()){
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
    paintMap(painter);
    paintUnits(painter);
    painter.fillRect(this->blk_size*intMap.size(),0,(this->blk_size*intMap[1].size(), "white");

    //if(this->game){
	//int blk_size = 40;
    /*for(int i=0;i<XDIM;i++){
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
				}
			}

		}
    }
    }//std::cout<<"Here5"<<std::endl;*/
}

void MainWindow::receiveGame(Game* gm)
{
    this->game = gm;
    this->intMap = game->getIntMap();
    this->loadImages();
    this->update();
    setFixedSize((this->blk_size*intMap.size())*4/3,this->blk_size*intMap[1].size());
}

void MainWindow::receiveNetwork(Network *net)
{
    this->network = net;
}

void MainWindow::loadImages()
{
    std::vector<std::vector<QImage*> > imageMap(this->intMap.size());
    auto x = this->intMap.size();
    this->imageMap = imageMap;
    //std::vector<std::vector<int> >::iterator it1;
    std::vector<int>::iterator it2;
    for (unsigned int i = 0 ; i != x; i++){
        for (it2 = intMap[i].begin(); it2 != intMap[i].end(); it2++){
            this->imageMap[i].push_back(loadImage(*it2));
        }
    }
    std::map<int,QImage*> unitImages;
    unitImages.insert(std::pair<int, QImage* >(0, new QImage(":/Units/Images/Units/geinfantry.gif")));
    unitImages.insert(std::pair<int, QImage* >(1, new QImage(":/Units/Images/Units/gemech.gif")));
    unitImages.insert(std::pair<int, QImage* >(2, new QImage(":/Units/Images/Units/gerecon.gif")));
    unitImages.insert(std::pair<int, QImage* >(3, new QImage(":/Units/Images/Units/geanti-air.gif")));
    unitImages.insert(std::pair<int, QImage* >(4, new QImage(":/Units/Images/Units/getank.gif")));
    unitImages.insert(std::pair<int, QImage* >(5, new QImage(":/Units/Images/Units/gemd.tank.gif")));
    unitImages.insert(std::pair<int, QImage* >(6, new QImage(":/Units/Images/Units/gemegatank.gif")));
    unitImages.insert(std::pair<int, QImage* >(7, new QImage(":/Units/Images/Units/geneotank.gif")));
    unitImages.insert(std::pair<int, QImage* >(8, new QImage(":/Units/Images/Units/geb-copter.gif")));
    unitImages.insert(std::pair<int, QImage* >(9, new QImage(":/Units/Images/Units/gefighter.gif")));
    unitImages.insert(std::pair<int, QImage* >(10, new QImage(":/Units/Images/Units/gebomber.gif")));
    this->UnitImages = unitImages;
}

QImage *MainWindow::loadImage(int a)
{
    switch (a){
    case 1 : return new QImage(":/Terrains/Images/Terrains/plain.gif");
    case 2 : return new QImage(":/Terrains/Images/Terrains/mountain.gif");
    case 3 : return new QImage(":/Terrains/Images/Terrains/wood.gif");
    case 4 : return new QImage(":/Terrains/Images/Terrains/hriver.gif");
    case 5 : return new QImage(":/Terrains/Images/Terrains/vriver.gif");
    case 6 : return new QImage(":/Terrains/Images/Terrains/criver.gif");
    case 7 : return new QImage(":/Terrains/Images/Terrains/esriver.gif");
    case 8 : return new QImage(":/Terrains/Images/Terrains/swriver.gif");
    case 9 : return new QImage(":/Terrains/Images/Terrains/wnriver.gif");
    case 10 : return new QImage(":/Terrains/Images/Terrains/neriver.gif");
    case 11 : return new QImage(":/Terrains/Images/Terrains/eswriver.gif");
    case 12 : return new QImage(":/Terrains/Images/Terrains/swnriver.gif");
    case 13 : return new QImage(":/Terrains/Images/Terrains/wneriver.gif");
    case 14 : return new QImage(":/Terrains/Images/Terrains/nesriver.gif");
    case 15 : return new QImage(":/Terrains/Images/Terrains/hroad.gif");
    case 16 : return new QImage(":/Terrains/Images/Terrains/vroad.gif");
    case 17 : return new QImage(":/Terrains/Images/Terrains/croad.gif");
    case 18 : return new QImage(":/Terrains/Images/Terrains/esroad.gif");
    case 19 : return new QImage(":/Terrains/Images/Terrains/swroad.gif");
    case 20 : return new QImage(":/Terrains/Images/Terrains/wnroad.gif");
    case 21 : return new QImage(":/Terrains/Images/Terrains/neroad.gif");
    case 22 : return new QImage(":/Terrains/Images/Terrains/eswroad.gif");
    case 23 : return new QImage(":/Terrains/Images/Terrains/swnroad.gif");
    case 24 : return new QImage(":/Terrains/Images/Terrains/wneroad.gif");
    case 25 : return new QImage(":/Terrains/Images/Terrains/nesroad.gif");
    case 26 : return new QImage(":/Terrains/Images/Terrains/hbridge.gif");
    case 27 : return new QImage(":/Terrains/Images/Terrains/vbridge.gif");
    case 28 : return new QImage(":/Terrains/Images/Terrains/sea.gif");
    case 29 : return new QImage(":/Terrains/Images/Terrains/hshoal.gif");
    case 30 : return new QImage(":/Terrains/Images/Terrains/hshoaln.gif");
    case 31 : return new QImage(":/Terrains/Images/Terrains/vshoal.gif");
    case 32 : return new QImage(":/Terrains/Images/Terrains/vshoale.gif");
    case 33 : return new QImage(":/Terrains/Images/Terrains/reef.gif");
    case 34 : return new QImage(":/Terrains/Images/Terrains/neutralcity.gif");
    case 35 : return new QImage(":/Terrains/Images/Terrains/neutralbase.gif");
    case 36 : return new QImage(":/Terrains/Images/Terrains/neutralairport.gif");
    case 37 : return new QImage(":/Terrains/Images/Terrains/neutralcity.gif");
    case 38 : return new QImage(":/Terrains/Images/Terrains/orangestarcity.gif");
    case 39 : return new QImage(":/Terrains/Images/Terrains/orangestarbase.gif");
    case 40 : return new QImage(":/Terrains/Images/Terrains/orangestarairport.gif");
    case 41 ... 42 : return new QImage(":/Terrains/Images/Terrains/orangestarcity.gif");
    case 43 : return new QImage(":/Terrains/Images/Terrains/bluemooncity.gif");
    case 44 : return new QImage(":/Terrains/Images/Terrains/bluemoonbase.gif");
    case 45 : return new QImage(":/Terrains/Images/Terrains/bluemoonairport.gif");
    case 46 ... 47 : return new QImage(":/Terrains/Images/Terrains/bluemooncity.gif");
    case 101 : return new QImage(":/Terrains/Images/Terrains/vpipe.gif");
    case 102 : return new QImage(":/Terrains/Images/Terrains/hpipe.gif");
    case 103 : return new QImage(":/Terrains/Images/Terrains/nepipe.gif");
    case 104 : return new QImage(":/Terrains/Images/Terrains/espipe.gif");
    case 105 : return new QImage(":/Terrains/Images/Terrains/swpipe.gif");
    case 106 : return new QImage(":/Terrains/Images/Terrains/wnpipe.gif");
    case 107 : return new QImage(":/Terrains/Images/Terrains/npipeend.gif");
    case 108 : return new QImage(":/Terrains/Images/Terrains/epipeend.gif");
    case 109 : return new QImage(":/Terrains/Images/Terrains/spipeend.gif");
    case 110 : return new QImage(":/Terrains/Images/Terrains/wpipeend.gif");
    case 111 ... 112 : return new QImage(":/Terrains/Images/Terrains/plain.gif");
    case 113 : return new QImage(":/Terrains/Images/Terrains/hpipeseam.gif");
    case 114 : return new QImage(":/Terrains/Images/Terrains/vpipeseam.gif");
    case 115 ... 116 : return new QImage(":/Terrains/Images/Terrains/plain.gif");
    case 129 :  return new QImage(":/Terrains/Images/Terrains/bluemooncity.gif");
    case 133 : return new QImage(":/Terrains/Images/Terrains/neutralcity.gif");
    case 134 : return new QImage(":/Terrains/Images/Terrains/orangestarcity.gif");
    case 140 : return new QImage(":/Terrains/Images/Terrains/bluemooncity.gif");
    case 145 : return new QImage(":/Terrains/Images/Terrains/neutralcity.gif");
    case 146 : return new QImage(":/Terrains/Images/Terrains/orangestarcity.gif");



    }
    return nullptr;
}

void MainWindow::paintMap(QPainter &painter)
{
    auto x = imageMap.size();

    for (unsigned int i = 0; i!= x;  i++){
        auto y = imageMap[i].size();

        for (unsigned int j = 0; j != y; j++){
            QImage img = imageMap[i][j]->scaledToWidth(blk_size);
            int x = int(i) * blk_size;
            auto r = img.height()-img.width();
            int y = int(j) * blk_size - r;

            painter.drawImage(*new QPoint(x, y), img );
        }
    }
}

void MainWindow::paintUnits(QPainter &painter)
{
    if (this->game->getUnits_blue().size()!=0){
        for (Unit* i : this->game->getUnits_blue() ){
            QImage im = UnitImages[i->getUnitType()]->scaledToWidth(blk_size);
            painter.drawImage(QPoint(i->getPosX()*blk_size, i->getPosY()*blk_size), im);
        }
        qDebug() << "j'ai quitté la boucle 1";
    }
    if (this->game->getUnits_blue().size()!=0){
        for (Unit* i : this->game->getUnits_orange() ){
            QImage im = UnitImages[i->getUnitType()]->scaledToWidth(blk_size);
            painter.drawImage(QPoint(i->getPosX()*blk_size, i->getPosY()*blk_size), im);
        }
        qDebug() << "j'ai quitté la boucle 2";
    }
}



