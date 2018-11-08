#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include "network.h"
#include "game.h"
#include "mapsize.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	Game* game;
	int blk_size;
	Network* network;

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void keyPressEvent(QKeyEvent *);
	void mousePressEvent(QMouseEvent *);
	void wheelEvent(QWheelEvent *);
	void paintEvent(QPaintEvent *);
	void receiveGame(Game* gm);
	void receiveNetwork(Network* net);

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
