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
		this->other = NULL;
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

	QJsonArray units;

	QJsonObject config;
	config["income"] = 1000;
	config["map"] = map;		//TODO
	config["firstplayer"] = 5;
	config["secondplayer"] = 10;
	config["youplay"] = "firstplayer";
	config["units"] = units;

	this->sendJson(config);
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

	QStringList keys = json.keys();

	if(! this->isConfigured) { //first transmission
		// recieves the map
		QJsonObject map = json["map"].toObject();

		Game* game = Game::getInstance(false);
		game->recieveNetwork(this);
		int size = map["size"].toInt();
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
				dat[j] = map[key].toInt();
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

		QJsonArray units = json["units"].toArray();
		for(int i=0;i<units.size();i++){
			QJsonObject unit = units[i].toObject();
			game->networkAction("newunit",unit["x"].toInt(),unit["y"].toInt(),
						this->getUnitType(unit["type"].toString()),unit["country"].toInt()); //TODO country
		}

		this->win->receiveGame(game); //enables the gameplay as well
		this->team = 'b';
		this->isConfigured = true;
	} else {
		//normal gameplay

/*
		Game::getInstance()->networkAction(json["type"].toString().toStdString(),
										json["x"].toInt(),json["y"].toInt(),
										json["data"].toInt(),json["data2"].toInt());
*/
		if(keys[0] == "move"){
			QJsonArray arr = json[keys[0]].toArray();
			Game::getInstance()->networkAction("move",arr[0].toInt(),arr[1].toInt(),arr[2].toInt(),arr[3].toInt());
			if(keys.size() >1 && keys[1] == "attack"){
				QJsonArray vic = json[keys[1]].toArray();
				Game::getInstance()->networkAction("attack",arr[0].toInt(),arr[1].toInt(),vic[0].toInt(),vic[1].toInt());
			}
		}else if(keys[0] == "build"){
			QJsonArray arr = json[keys[0]].toArray(); // TODO in game.cpp
			int tm;
			if(this->team == 'o'){
				tm = 2;
			}else if(this->team == 'b'){
				tm = 1;
			}
			Game::getInstance()->networkAction("newunit",
							arr[0].toInt(),arr[1].toInt(),this->getUnitType(json[keys[1]].toString()),tm);
		}else if(keys[0] == "endofturn"){
			Game::getInstance()->networkAction("endTurn",0,0,0,0);
		}

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
	if(type == "move"){
		QJsonArray arr;
		arr.append(x); arr.append(y);
		arr.append(data); arr.append(data2);  //data is newX and newY
		//arr[0] = x; arr[1] = y; arr[2] = data; arr[3] = data2;
		action["move"] = arr;
	}else if(type == "attack"){
		QJsonArray arr;
		arr.append(x); arr.append(y);
		arr.append(x); arr.append(y);
		action["move"] = arr;
		QJsonArray pos;
		pos.append(data); pos.append(data2);
		action["attack"] = pos;
	}else if(type == "newunit"){
		QJsonArray pos;
		pos.append(x); pos.append(y);
		//pos[0] = x; pos[1] = y;
		action["build"] = pos;
		action["type"] = this->getUnitName(data);
	}else if(type == "endTurn"){
		action["endofturn"] =true;
	}
	/*
	action["type"] = type;
	action["x"] = x;
	action["y"] = y;
	action["data"] = data;
	action["data2"] = data2;*/
	this->sendJson(action);
}

void Network::sendJson(const QJsonObject& obj) { //updated to reference
	QByteArray data = QJsonDocument(obj).toJson();
	QDataStream out(this->other);
	out << (quint32) data.length();
	this->other->write(data);

	std::cout << "Sending " << data.toStdString() << std::endl;
}

int Network::getUnitType(const QString & name)
{
	if(name == "infantry"){
		return 0;
	}else if(name == "bazzoka"){
		return 1;
	}else if(name == "recon"){
		return 2;
	}else if(name == "antiair"){
		return 3;
	}else if(name == "tank"){
		return 4;
	}else if(name == "mdtank"){
		return 5;
	}else if(name == "megatank"){
		return 6;
	}else if(name == "neotank"){
		return 7;
	}else if(name == "bcopter"){
		return 8;
	}else if(name == "fighter"){
		return 9;
	}else if(name == "bomber"){
		return 10;
	}else{
		return -1; //error
	}
}

QString Network::getUnitName(const int type)
{
	switch(type){
		case 0: return "infantry";
		case 1: return "bazooka";
		case 2: return "recon";
		case 3: return "antiair";
		case 4: return "tank";
		case 5: return "mdtank";
		case 6: return "megatank";
		case 7: return "neotank";
		case 8: return "bcopter";
		case 9: return "fighter";
		case 10: return "bomber";
	default: return "";
	}
}

char Network::getTeam() const
{
	return this->team;
}
