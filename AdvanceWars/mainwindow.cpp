#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    this->blk_size = 50;
	this->game = NULL;
	this->network = NULL;
    //connect(&timer, SIGNAL(), this, SLOT(refresh()));
    //timer.start(50);
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
	//paintMap(painter);
	//paintUnits(painter);
    QColor col1(Qt::cyan);
    QColor col2(Qt::red);
    col1.setAlpha(100);
    col2.setAlpha(100);

    for(int i = 0; i < this->intMap.size() ; i++){
        for(int j = 0; j < this->intMap[i].size() ; j++){
			QImage img = imageMap[i][j]->scaledToWidth(blk_size);
			int x = int(i) * blk_size;
			int r = img.height()-img.width();
			int y = int(j) * blk_size - r;

			painter.drawImage(QPoint(x, y), img );

			std::vector<GameObject*> obj_onPos = this->game->getObjectsOnPos(i,j);
			std::vector<GameObject*>::iterator it;

			for(it=obj_onPos.begin();it!=obj_onPos.end();it++){
				GameObject* go = *it;
				if(dynamic_cast<ValidMove*>(go)){
					if(dynamic_cast<ValidMove*>(go)->isLast()){
						painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size-2,this->blk_size-2,col2);
					}else{
						painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size-2,this->blk_size-2,col1);
					}
				}
				else if(Unit* un = dynamic_cast<Unit*>(go)){
					QImage im = UnitImages[un->getUnitType()]->scaledToWidth(blk_size);
					painter.drawImage(QPoint(i*blk_size, j*blk_size), im);
				}
			}

		}
    }
	//paintUnits(painter);
    //std::cout<<"Here5"<<std::endl;
}

void MainWindow::receiveGame(Game* gm)
{
    this->game = gm;
    this->intMap = game->getIntMap();
    this->loadImages();
    this->update();
    setFixedSize((this->blk_size*intMap.size())*4/3,this->blk_size*intMap[1].size());
    this->ui->horizontalSpacer->changeSize(1000,100);
}

void MainWindow::receiveNetwork(Network *net)
{
    this->network = net;
}

void MainWindow::loadImages()
{
    std::vector<std::vector<QImage*> > imageMap(this->intMap.size());
	unsigned int x = this->intMap.size();
    this->imageMap = imageMap;
    //std::vector<std::vector<int> >::iterator it1;
    std::vector<int>::iterator it2;
    for (unsigned int i = 0 ; i != x; i++){
        for (it2 = intMap[i].begin(); it2 != intMap[i].end(); it2++){
            this->imageMap[i].push_back(ImageLoader::loadImage(*it2));
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

void MainWindow::loadImage(int x, int y)
{
    this->imageMap[x][y] = ImageLoader::loadImage(this->intMap[x][y]);
    this->update();
}

