#ifndef STARTINGWINDOW_H
#define STARTINGWINDOW_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class StartingWindow;
}

class StartingWindow : public QDialog
{
    Q_OBJECT

//public:
    bool network;
	QString IPAddress;
	int AIcnt;
    bool isHost;
    int income;
	int AIOption;
	MainWindow* w;
	Network* net;

public:
	explicit StartingWindow(QWidget *parent =0);
    ~StartingWindow();

private:
    Ui::StartingWindow *ui;
    void checkRadioButtons();
    void fillIPComboBox();
    void checkNetworkOptions();

private slots:
    void launchGame();
};

#endif // STARTINGWINDOW_H
