#ifndef NETWORK_H
#define NETWORK_H

//#include <QObject>
#include <QtNetwork>
//#include "mainwindow.h"
class MainWindow;

class Network : public QObject
{
	Q_OBJECT

	QTcpServer* server = NULL;
	QTcpSocket* other = NULL;

	bool isConfigured;
	char team; //server is always orange
	int currentSize;
	MainWindow* win;

public:
	explicit Network(MainWindow* wn, QObject *parent = 0);
	virtual ~Network() {}
	void sendData(QString type, int x=0, int y=0, int data=0, int data2=0);
	char getTeam() const;

signals:

public slots:
	void onNewConnection();
	void onConnected();
	void onData();
	void onDisconnected();

private:
	void sendJson(const QJsonObject&);
	int getUnitType(const QString&);
	QString getUnitName(const int);
};

#endif // NETWORK_H
