#ifndef STARTINGWINDOW_H
#define STARTINGWINDOW_H

#include <QDialog>

namespace Ui {
class StartingWindow;
}

class StartingWindow : public QDialog
{
    Q_OBJECT

public:
	int AIcnt;
    bool isHost;
    int income;
	int AIOption;
	explicit StartingWindow(QWidget *parent =0);
    ~StartingWindow();

private:
    Ui::StartingWindow *ui;
    void checkRadioButtons();

private slots:
    void launchGame();
};

#endif // STARTINGWINDOW_H
