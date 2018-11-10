#include "network.h"
#include <iostream>
#include <strstream>
#include "game.h"
#include "mainwindow.h"

#include "gameobject.h"
#include "building.h"
#include <vector>
#include "city.h"
#include "airport.h"
#include "terrain.h"

//using namespace std;

Network::Network(MainWindow *wn, QObject *parent) : QObject(parent)
{
	this->isConfigured = false;
	this->currentSize = 0;
	this->win = wn;
	wn->receiveNetwork(this);

	this->server = new QTcpServer();
	//setting IP address and port here
	if(! this->server->listen(QHostAddress::Any, 8123)) {
		std::cout << "I am a client" << std::endl;
		this->other = new QTcpSocket();
		QObject::connect(this->other, SIGNAL(connected()), this, SLOT(onConnected()));
		this->other->connectToHost("127.0.0.1", 8123);
		QObject::connect(this->other, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
		//emit this->other->connected();
	} else {
		std::cout << "I am the server" << std::endl;
        this->other = nullptr;
	}

	QObject::connect(this->server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
	//emit this->server->newConnection();
}

void Network::onNewConnection() {
	std::cout << "A new client is connecting !" << std::endl;
	this->other = this->server->nextPendingConnection();
	connect(this->other, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(this->other, SIGNAL(readyRead()), this, SLOT(onData()));

	Game* game = Game::getInstance();
	game->recieveNetwork(this);
	//action to be executed when a client connects
	// = send him the map
	this->win->receiveGame(game); //enables the gameplay as well
	this->team = 'o';
	this->isConfigured = true;

	std::vector<GameObject*>* objects = game->getMapContent();
	int size = objects->size();
	QJsonObject map;
	map["size"] = size;
	for(int i=0; i<size;i++){
		//std::cout<< "Checking objects"<<endl;
		GameObject* obj = (*objects)[i];
		int type;
		if(obj->getType() == "Terrain"){
			type = 1;
		}else if(obj->getType() == "City"){
			type = 2;
		}else if(obj->getType() == "Factory"){
			type = 3;
		}else if(obj->getType() == "Airport"){
			type = 4;
		}else{
			type = 1;
		}
		int owner;
		if(type == 1){
			owner = 0;
		}else{
			Building* bld = dynamic_cast<Building*>(obj);
			switch(bld->getOwner()){
				case 'o': owner = 1; break;
				case 'b': owner = 2; break;
				default: owner = 0;
			}
		}
		std::strstream ss;
		ss << i;
		char* st;//[3];
		st = ss.str();
		char str[2];
		str[0] = st[0];
		//str[1] = st[1];
		str[1] = '\0';
		QString key = str;

		//QString key = ""+i;
		map[key+"0"] = type;
		map[key+"1"] = obj->getPosX();
		map[key+"2"] = obj->getPosY();
		map[key+"3"] = owner;
	}
	delete objects;
	/*


	map["size"] = 5; //TODO number of added buildings -> see game, hardcoded
	//int arr[] = {1,5,2,1};
	//QJsonArray ar = QJsonArray::fromVariantList(arr);
	map["00"] = 1; //go type (1=terrain, 2=city, 3=factory, 4=airport),x,y,type/ownership
	map["01"] = 5;
	map["02"] = 2;
	map["03"] = 1;
	map["10"] = 3;
	map["11"] = 0;
	map["12"] = 1;
	map["13"] = 1;
	map["20"] = 4;
	map["21"] = 0;
	map["22"] = 4;
	map["23"] = 1;
	map["30"] = 2;
	map["31"] = 0;
	map["32"] = 2;
	map["33"] = 0;
	map["40"] = 3;
	map["41"] = 0;
	map["42"] = 6;
	map["43"] = 2;
*/
	this->sendJson(map);
}

void Network::onConnected() {
	std::cout << "I am connected" << std::endl;
	QObject::connect(this->other, SIGNAL(readyRead()), this, SLOT(onData()));
}

void Network::onDisconnected() {
	std::cout << "The other guy just disconnected" << std::endl;
}

void Network::onData() {
	std::cout << "Incoming data !" << std::endl;
	if(this->currentSize == 0) {
		if(this->other->bytesAvailable() < 4)
			return;

		QDataStream in(this->other);
		in >> this->currentSize;
	}

	if(this->other->bytesAvailable() < this->currentSize)
		return;

	QByteArray data = this->other->read(this->currentSize);
	//std::cout << data.toStdString() << std::endl;
	this->currentSize = 0;

	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonObject json = doc.object();

	if(! this->isConfigured) { //first transmission
		// recieves the map

		Game* game = Game::getInstance(false);
		game->recieveNetwork(this);
		int size = json["size"].toInt();
		for(int i=0;i<size;i++){
			int dat[4];
			for(int j=0;j<4;j++){
				std::strstream ss;
				ss << i; ss << j;
				char* st;//[3];
				st = ss.str();
				char str[3];
				str[0] = st[0];
				str[1] = st[1];
				str[2] = '\0';
				QString key = str;//ss.str();//std::to_string(i) + std::to_string(j);
				//std::cout<<str<<std::endl;
				dat[j] = json[key].toInt();
			}
			//int dat[] = json[i].toArray();
			int x = dat[1];
			int y = dat[2];
			char owner;
			switch(dat[3]){
				case 0: owner = '\0'; break;
				case 1: owner = 'o'; break;
				case 2: owner = 'b'; break;
			}
			//std::cout<< x<<y<<owner<<std::endl;

			switch(dat[0]){
				case 1: game->addGameObject(new Terrain(x,y,dat[3]),x,y); break;
				case 2: game->addGameObject(new City(x,y,owner),x,y,owner); break;
				case 3: game->addGameObject(new Factory(x,y,owner),x,y,owner); break;
				case 4: game->addGameObject(new Airport(x,y,owner),x,y,owner); break;
			}
		}

		this->win->receiveGame(game); //enables the gameplay as well
		this->team = 'b';
		this->isConfigured = true;
	} else {
		//normal gameplay


		//if(json["type"] == "move"){
		Game::getInstance()->networkAction(json["type"].toString().toStdString(),
										json["x"].toInt(),json["y"].toInt(),
										json["data"].toInt(),json["data2"].toInt());
		//}else if(json["type"] == "endTurn"){
		//	Game::getInstance()->endTurn();
		//}

		//idk what is this, probably error checking
		/*
		if(!(posX == oldX && posY == oldY)) {
			std::cerr << "ERROR" << std::endl;
			//destroy(); //needs QWidget
			return;
		}*/

		this->win->update();
	}
}

void Network::sendData(QString type, int x, int y, int data, int data2){
	QJsonObject action;
	action["type"] = type;
	action["x"] = x;
	action["y"] = y;
	action["data"] = data;
	action["data2"] = data2;
	this->sendJson(action);
}

void Network::sendJson(const QJsonObject& obj) { //updated to reference
	QByteArray data = QJsonDocument(obj).toJson();
	QDataStream out(this->other);
	out << (quint32) data.length();
	this->other->write(data);

	std::cout << "Sending " << data.toStdString() << std::endl;
}

char Network::getTeam() const
{
	return this->team;
}
