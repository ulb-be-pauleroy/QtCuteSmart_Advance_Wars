#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QLayoutItem>
#include <QVBoxLayout>
#include <iostream>
#include <string>
#include<QObject>



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    this->blk_size = 50;
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
	//paintMap(painter);
	//paintUnits(painter);
    QColor col1(Qt::cyan);
    QColor col2(Qt::red);
    col1.setAlpha(100);
    col2.setAlpha(100);

    for(int i = 0; i < this->game->getIntMap().size() ; i++){
        for(int j = 0; j < this->game->getIntMap()[i].size() ; j++){
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
                    if(un->getTeam() == 'o'){
                        QImage im = osUnitImages[un->getUnitType()]->scaledToWidth(blk_size);
                        painter.drawImage(QPoint(i*blk_size, j*blk_size), im);
                    }
                    else{
                        QImage im = bmUnitImages[un->getUnitType()]->scaledToWidth(blk_size);
                        painter.drawImage(QPoint(i*blk_size, j*blk_size), im);
                    }

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
    gm->setWn(this);
    this->loadImages();
    this->update();
    setFixedSize((this->blk_size*this->game->getIntMap().size())*4/3,this->blk_size*this->game->getIntMap()[1].size());

    ui->label->setFont(QFont("Comic Sans MS", 14));
    ui->label->setText("Point de vie:");

    ui->label_2->setFont(QFont("Comic Sans MS", 14));
    ui->label_2->setText("crédit Bleu:");

    ui->label_3->setFont(QFont("Comic Sans MS", 14));
    ui->label_3->setText("crédit Orange:");

    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdNumber->display(game->getSelected_unit()->getHealth());

    ui->lcdNumber_4->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdNumber_4->display(game->getMoney_blue());

    ui->lcdNumber_3->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdNumber_3->display(game->getMoney_orange());

    Unit* uniteselec = game->getSelected_unit();

    ui->pushButton->setFont(QFont("Comic Sans MS", 14));
    QObject::connect(ui->pushButton, SIGNAL(clicked()), uniteselec, SLOT(uniteselec.capture()));

    ui->pushButton_2->setFont(QFont("Comic Sans MS", 14));
    QObject::connect(ui->pushButton_2, SIGNAL(clicked()), uniteselec, SLOT(uniteselec->attack()));

    this->update();


}

void MainWindow::receiveNetwork(Network *net)
{
    this->network = net;
}

void MainWindow::loadImages()
{
    std::vector<std::vector<QImage*> > imageMap(this->game->getIntMap().size());
    unsigned int x = this->game->getIntMap().size();
    this->imageMap = imageMap;
    //std::vector<std::vector<int> >::iterator it1;
    std::vector<int>::iterator it2;
    for (unsigned int i = 0 ; i != x; i++){
        for (it2 = this->game->getIntMap()[i].begin(); it2 != this->game->getIntMap()[i].end(); it2++){
            this->imageMap[i].push_back(ImageLoader::loadImage(*it2));
        }
    }

    std::map<int,QImage*> osUnitImages;
    osUnitImages.insert(std::pair<int, QImage* >(0, new QImage(":/Units/Images/Units/osinfantry.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(1, new QImage(":/Units/Images/Units/osmech.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(2, new QImage(":/Units/Images/Units/osrecon.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(3, new QImage(":/Units/Images/Units/osanti-air.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(4, new QImage(":/Units/Images/Units/ostank.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(5, new QImage(":/Units/Images/Units/osmd.tank.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(6, new QImage(":/Units/Images/Units/osmegatank.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(7, new QImage(":/Units/Images/Units/osneotank.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(8, new QImage(":/Units/Images/Units/osb-copter.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(9, new QImage(":/Units/Images/Units/osfighter.gif")));
    osUnitImages.insert(std::pair<int, QImage* >(10, new QImage(":/Units/Images/Units/osbomber.gif")));
    this->osUnitImages = osUnitImages;

    std::map<int,QImage*> bmUnitImages;
    bmUnitImages.insert(std::pair<int, QImage* >(0, new QImage(":/Units/Images/Units/bminfantry.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(1, new QImage(":/Units/Images/Units/bmmech.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(2, new QImage(":/Units/Images/Units/bmrecon.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(3, new QImage(":/Units/Images/Units/bmanti-air.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(4, new QImage(":/Units/Images/Units/bmtank.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(5, new QImage(":/Units/Images/Units/bmmd.tank.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(6, new QImage(":/Units/Images/Units/bmmegatank.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(7, new QImage(":/Units/Images/Units/bmneotank.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(8, new QImage(":/Units/Images/Units/bmb-copter.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(9, new QImage(":/Units/Images/Units/bmfighter.gif")));
    bmUnitImages.insert(std::pair<int, QImage* >(10, new QImage(":/Units/Images/Units/bmbomber.gif")));
    this->bmUnitImages = bmUnitImages;
}

void MainWindow::reloadImage(int x, int y)
{
    this->imageMap[x][y] = ImageLoader::loadImage(this->game->getIntMap()[x][y]);
    qDebug() << this->game->getIntMap()[x][y];
    this->update();
    qDebug() << "j'ai sélectionné une nouvelle image!";
}

