#include <startingwindow.h>
#include "mainwindow.h"
#include "network.h"
#include "game.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    StartingWindow w2;
    w2.show(); //uncomment to show starting window

	bool online = false;

    Network* net = NULL;
    /*if(online){ // network creation in if statement doesnt connect
        net = new Network(&w);
    }else{
        Game* gm = Game::getInstance(); //Game and Mainwindow creation transferred in starting window!
        gm->setupGame();                //Adapt the code there
        w.receiveGame(gm);
    }*/

	//uncomment lines 18 and 19 (Game) and comment line 15 (Network)
	//when you want to play hotspot

	return a.exec();
}
