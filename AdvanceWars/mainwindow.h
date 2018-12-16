#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include "network.h"
#include "game.h"
#include "mapsize.h"
#include "imageloader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

    bool isAzerty;
	Game* game;
	int blk_size;
	Network* network;
    std::vector<std::vector<QImage*> > imageMap;
    std::map<int, QImage*> osUnitImages ;
    std::map<int, QImage*> bmUnitImages ;
	bool isAnimating;

public:
    explicit MainWindow(bool isAzerty, QWidget *parent = 0);
	~MainWindow();
    void drawGamePannel(QPainter painter);
	void keyPressEvent(QKeyEvent *);
	void mousePressEvent(QMouseEvent *);
	void wheelEvent(QWheelEvent *);
	void paintEvent(QPaintEvent *);
	void receiveGame(Game* gm);
	void receiveNetwork(Network* net);
    void loadImages();
    void reloadImage(int x, int y);
    void setButtons();
    void displayUnitInfo();

public slots:
    void UnitButtonPushed(int a);
    void endTurn();

private:
	Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
