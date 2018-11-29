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

	Game* game;
	int blk_size;
	Network* network;
    std::vector<std::vector<int> > intMap;
    std::vector<std::vector<QImage*> > imageMap;
    std::map<int, QImage*> UnitImages ;

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void keyPressEvent(QKeyEvent *);
	void mousePressEvent(QMouseEvent *);
	void wheelEvent(QWheelEvent *);
	void paintEvent(QPaintEvent *);
	void receiveGame(Game* gm);
	void receiveNetwork(Network* net);
    void loadImages();
	//void paintMap(QPainter &painter);
	//void paintUnits(QPainter &painter);

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
