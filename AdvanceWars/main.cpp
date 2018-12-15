#include <startingwindow.h>
#include "mainwindow.h"
#include "network.h"
#include "game.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    StartingWindow w2;
    w2.show();

	return a.exec();
}
