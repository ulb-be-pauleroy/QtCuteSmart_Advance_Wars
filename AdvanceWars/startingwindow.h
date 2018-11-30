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
	explicit StartingWindow(QWidget *parent =0);
    ~StartingWindow();

private:
    Ui::StartingWindow *ui;
};

#endif // STARTINGWINDOW_H
