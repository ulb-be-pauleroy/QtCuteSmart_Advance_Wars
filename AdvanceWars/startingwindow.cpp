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
    this->income = 1000;
	this->AIOption = 0;
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(launchGame()));
    this->fillIPComboBox();
}

StartingWindow::~StartingWindow()
{
    delete ui;
}

void StartingWindow::checkRadioButtons()
{
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
    this->ui->comboBox->addItem("Local online gaming (fill the space below with other IP)");


}

void StartingWindow::checkNetworkOptions()
{
    if(this->ui->comboBox->currentText() != "Offline"){
        this->network = true;
        this->IPAdress = this->ui->lineEdit->text();
    }
}

void StartingWindow::launchGame()
{
    qDebug() << "j'ai lancé la fonction!";
    checkRadioButtons();
    checkNetworkOptions();
    MainWindow* w = new MainWindow();
    w->show();
    Game* gm = Game::getInstance();
	gm->setupGame(income, AIcnt, AIOption, isHost);
    w->receiveGame(gm);
    this->hide(); //TODO should destruct itself
}
