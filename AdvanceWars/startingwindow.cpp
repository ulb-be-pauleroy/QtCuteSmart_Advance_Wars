#include "game.h"
#include "mainwindow.h"
#include "startingwindow.h"
#include "ui_startingwindow.h"
#include <QDebug>

StartingWindow::StartingWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartingWindow)
{
    ui->setupUi(this);
	this->AIcnt = 0;
    this->isHost = true;
    this->azerty = true;
    this->income = 1000;
	this->AIOption = 0;
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(launchGame()));
    this->fillIPComboBox();
    this->setWindowTitle("Advanced Wars (aka the return of the tank)");
}

StartingWindow::~StartingWindow()
{
    delete ui;
}

void StartingWindow::checkRadioButtons()
{
    if (! ui->AzertyButton->isChecked()) this->azerty = false;

    if(! ui->IncomeOption_1->isChecked()){
        if (ui->IncomeOption_2->isChecked()) this->income = 1500;
        else if (ui->IncomeOption_3->isChecked()) this->income = 2000;
        else if (ui->IncomeOption_4->isChecked()) this->income = 2500;
    }

    if(! ui->GameOption_1->isChecked()){
		if (ui->GameOption_2->isChecked()) this->AIcnt = 1;
		else if (ui->GameOption_3->isChecked()) this->AIcnt = 2;
    }

    if (! ui->IAOption_1->isChecked()){
		if (ui->IAOption_2->isChecked()) this->AIOption = 1;
		if (ui->IAOption_3->isChecked()) this->AIOption = 2;
    }


}

void StartingWindow::fillIPComboBox()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses(); //
    QList<QHostAddress>::iterator it;
    this->ui->comboBox->addItem("Offline");
	this->ui->comboBox->addItem("LAN gaming (fill the space below with other IP)");


}

void StartingWindow::checkNetworkOptions()
{
    if(this->ui->comboBox->currentText() != "Offline"){
        this->network = true;
		this->IPAddress = this->ui->lineEdit->text();
	}else{
		this->network = false;
	}
}

void StartingWindow::launchGame()
{
    qDebug() << "j'ai lancé la fonction!";
    checkRadioButtons();
    checkNetworkOptions();
    MainWindow* w = new MainWindow(azerty);
    w->show();

	if(this->network){
		if(this->IPAddress == "Please enter IP adress here") this->IPAddress = "127.0.0.1";
		qDebug() << "Connecting to: "<<this->IPAddress;
		Network* net = new Network(this->IPAddress,w);
	}else{
		Game* gm = Game::getInstance();
		gm->setupGame(income,true, AIcnt, AIOption, isHost);
		w->receiveGame(gm);
	}
    this->hide(); //TODO should destruct itself
}
