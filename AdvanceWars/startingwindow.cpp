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
    this->IA = false;
    this->isHost = true;
    this->income = 1000;
    this->IAOption = 0;
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(launchGame()));
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
        if (ui->GameOption_2->isChecked()) this->IA = true;
        else if (ui->GameOption_3->isChecked()) this->IA = true; //TODO IA vs IA is not implemented yet
    }

    if (! ui->IAOption_1->isChecked()){
        if (ui->IAOption_2->isChecked()) this->IAOption = 1;
        if (ui->IAOption_3->isChecked()) this->IAOption = 2;
    }


}

void StartingWindow::launchGame()
{
    qDebug() << "j'ai lancé la fonction!";
    checkRadioButtons();
    MainWindow* w = new MainWindow();
    w->show();
    Game* gm = Game::getInstance();
    gm->setupGame(income, IA, IAOption, isHost);
    w->receiveGame(gm);
    this->hide(); //TODO should destruct itself
}
