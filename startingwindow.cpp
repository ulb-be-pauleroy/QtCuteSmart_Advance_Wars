#include "startingwindow.h"
#include "ui_startingwindow.h"
#include <iostream>
#include <string>
#include <QLayoutItem>

StartingWindow::StartingWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartingWindow)
{
    ui->setupUi(this);
}

StartingWindow::~StartingWindow()
{
    delete ui;
}


//QObject::connect(ui->pushButton, SIGNAL(clicked()), StartingWindow, SLOT(quit()));
