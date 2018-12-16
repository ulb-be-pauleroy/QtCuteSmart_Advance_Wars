#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QTimer>

MainWindow::MainWindow(bool isAzerty, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    this->isAzerty = isAzerty;
    this->blk_size = 50;
	this->game = NULL;
	this->network = NULL;
	this->isAnimating = false;
    setButtons();
    ui->UnitCostLayout->hide();
	this->setWindowTitle("Advanced Wars (aka the return of the tank)");
}



MainWindow::~MainWindow()
{
	qDebug() << "Destroying MW";
    delete ui;
	for(unsigned int i=0;i<this->imageMap.size();i++){
		for(unsigned int j=0;j<this->imageMap[i].size();j++){
			delete this->imageMap[i][j];
		}
	}
	for(int i=0;i<11;i++){
		delete this->osUnitImages[i];
		delete this->bmUnitImages[i];
	}
}

void MainWindow::drawGamePannel(QPainter painter)
{

}


void MainWindow::keyPressEvent(QKeyEvent * event)
{
	if(game &&(!this->network || (this->network->getTeam() == game->getTeamOnTurn()))){
		if(isAzerty){
			switch(event->key()){
				case Qt::Key_Z: game->move(0); break;
				case Qt::Key_Q: game->move(2); break;
				case Qt::Key_S: game->move(1); break;
				case Qt::Key_D: game->move(3); break;
			}
		}else{
			switch(event->key()){
				case Qt::Key_W: game->move(0); break;
				case Qt::Key_A: game->move(2); break;
				case Qt::Key_S: game->move(1); break;
				case Qt::Key_D: game->move(3); break;
			}
		}

        switch(event->key()){
//<<<<<<< HEAD
        case Qt::Key_F: game->setAttack(); break;
//=======
		//?? WASD anybody?

		//case Qt::Key_A: game->setAttack(); break;
//>>>>>>> 96d020c447514bb52921663691bc891dabe1cf80
		case Qt::Key_C: game->cycleUnits(1); break;
			//Tab, as well as Enter and Space are blocked by buttons

        case Qt::Key_T: game->endTurn(); break;

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
	if(game && (!this->network || this->network->getTeam() == game->getTeamOnTurn())) {
        if(! isAnimating) {
            int x = event->x();
            int y = event->y();
            if(event->button() == 1) {
                this->game->click(x/this->blk_size, y/this->blk_size);
            } else if(event->button() == 2) {
                this->game->moveTo(x/this->blk_size, y/this->blk_size);
            }
            this->update();
        }
	}
}

void MainWindow::wheelEvent(QWheelEvent * event){

	if(game && (!this->network || (this->network->getTeam() == game->getTeamOnTurn()))){
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
    QColor col1(Qt::cyan);
    QColor col2(Qt::red);
    col1.setAlpha(100);
    col2.setAlpha(100);

    if(this->game){
        for(unsigned int i = 0; i < this->game->getIntMap().size() ; i++){
            for(unsigned int j = 0; j < this->game->getIntMap()[i].size() ; j++){
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
                        if(un->getHealth() != 10){
                            painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size-2,this->blk_size/10,QColor(Qt::red));
                            painter.fillRect(this->blk_size*i,this->blk_size*j,this->blk_size*(float)un->getHealth()/10,this->blk_size/10,QColor(Qt::green));
                        }

                    }
                }
            }
        }
        displayUnitInfo();
        ui->OrangeMoneyCounter->display(game->getBalance('o'));
        ui->BlueMoneyCounter->display(game->getBalance('b'));
        if (game->getTeamOnTurn() == 'b'){
            ui->TurnLabel->setStyleSheet("QLabel { color : blue; }");
            ui->TurnLabel->setText("BLUE MOON") ;
        }
        else if(game->getTeamOnTurn() == 'o') {
            ui->TurnLabel->setStyleSheet("QLabel { color : orange; }");
            ui->TurnLabel->setText("Orange STAR");
        }
    }
}

void MainWindow::receiveGame(Game* gm)
{
    this->game = gm;
    gm->setWn(this);
    this->loadImages();
    this->update();
    unsigned int x = this->blk_size*this->game->getIntMap().size();
    unsigned int y = this->blk_size*this->game->getIntMap()[1].size();
    setFixedSize(x+500, y);
    ui->InfoLayout->setGeometry(x, 0, 500, y/8);
    ui->UnitInfoLayout->setGeometry(x, y/8, 500, 3*y/8);
    ui->UnitCostLayout_2->setGeometry(x, y/2, 500, y/2);
    ui->HealthPointsCounter->display(0);
    ui->BlueMoneyCounter->display(game->getBalance('b'));
    ui->OrangeMoneyCounter->display(game->getBalance('o'));

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
    osUnitImages.insert(std::pair<int, QImage* >(5, new QImage(":/Units/Images/Units/osmd-tank.gif")));
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
    bmUnitImages.insert(std::pair<int, QImage* >(5, new QImage(":/Units/Images/Units/bmmd-tank.gif")));
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

void MainWindow::setButtons()
{
    connect(ui->endTurnButton, SIGNAL(clicked()), this, SLOT(endTurn()));
    QSignalMapper* mapper = new QSignalMapper(this);
    connect(ui->InfantryButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->InfantryButton->setIcon(QIcon(":/Units/Images/Units/osinfantry.gif"));
    connect(ui->BazookaButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->BazookaButton->setIcon(QIcon(":/Units/Images/Units/osmech.gif"));
    connect(ui->ReconButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->ReconButton->setIcon(QIcon(":/Units/Images/Units/osrecon.gif"));
    connect(ui->AntiAirButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->AntiAirButton->setIcon(QIcon(":/Units/Images/Units/osanti-air.gif"));
    connect(ui->TankButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->TankButton->setIcon(QIcon(":/Units/Images/Units/ostank.gif"));
    connect(ui->MdTankButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->MdTankButton->setIcon(QIcon(":/Units/Images/Units/osmd-tank.gif"));
    connect(ui->MegaTankButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->MegaTankButton->setIcon(QIcon(":/Units/Images/Units/osmegatank.gif"));
    connect(ui->NeoTankButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->NeoTankButton->setIcon(QIcon(":/Units/Images/Units/osneotank.gif"));
    connect(ui->TCopterButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->TCopterButton->setIcon(QIcon(":/Units/Images/Units/osb-copter.gif"));
    connect(ui->FighterButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->FighterButton->setIcon(QIcon(":/Units/Images/Units/osfighter.gif"));
    connect(ui->BomberButton, SIGNAL(clicked()), mapper, SLOT(map()));
    ui->BomberButton->setIcon(QIcon(":/Units/Images/Units/osbomber.gif"));

    mapper->setMapping(ui->InfantryButton, 1);
    mapper->setMapping(ui->BazookaButton, 2);
    mapper->setMapping(ui->ReconButton, 3);
    mapper->setMapping(ui->AntiAirButton, 4);
    mapper->setMapping(ui->TankButton, 5);
    mapper->setMapping(ui->MdTankButton, 6);
    mapper->setMapping(ui->MegaTankButton, 7);
    mapper->setMapping(ui->NeoTankButton, 8);
    mapper->setMapping(ui->TCopterButton, 1);
    mapper->setMapping(ui->FighterButton, 2);
    mapper->setMapping(ui->BomberButton, 3);
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(UnitButtonPushed(int)));
}

void MainWindow::displayUnitInfo()
{
    Unit* un = this->game->getSelected_unit();
    if(un){
        ui->HealthPointsCounter->display(un->getHealth());
        ui->MovementPointsCounter->display(un->getMovesLeft());
        QPixmap unitPix;
        int w = ui->UnitImage->width();
        int h = ui->UnitImage->height();
        if(un->getTeam() == 'b'){
            ui->UnitImage->setPixmap(unitPix.fromImage(*bmUnitImages[un->getUnitType()]).scaled(w,h,Qt::KeepAspectRatio));
        }
        else{
            ui->UnitImage->setPixmap(unitPix.fromImage(*osUnitImages[un->getUnitType()]).scaled(w,h,Qt::KeepAspectRatio));
        }
    }
    else{
        ui->UnitImage->setPixmap(QPixmap());
        ui->HealthPointsCounter->display(0);
    }


}

void MainWindow::UnitButtonPushed(int a)
{
    this->game->buyUnit(a);
}

void MainWindow::endTurn()
{
    this->game->endTurn();
}

