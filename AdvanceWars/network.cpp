#include "network.h"
#include <iostream>
//#include <strstream>
#include "game.h"
#include "mainwindow.h"
#include "mapbuilder.h"

#include "gameobject.h"
#include "building.h"
#include <vector>
#include "city.h"
#include "airport.h"
#include "terrain.h"

//using namespace std;

Network::Network(QString hostIP, MainWindow *wn, QObject *parent) : QObject(parent)
{
	this->isConfigured = false;
	this->currentSize = 0;
	this->win = wn;
	wn->receiveNetwork(this);

	this->server = new QTcpServer();
	if(! this->server->listen(QHostAddress::Any, 8123)) {
		std::cout << "I am a client" << std::endl;
		this->other = new QTcpSocket();
		QObject::connect(this->other, SIGNAL(connected()), this, SLOT(onConnected()));
		this->other->connectToHost(hostIP, 8123);
		QObject::connect(this->other, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	} else {
		std::cout << "I am the server" << std::endl;
		this->other = NULL;
	}

	QObject::connect(this->server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

Network::~Network(){
	delete this->server;
	if(this->other) delete this->other;
	delete Game::getInstance();
}

void Network::onNewConnection() {
	std::cout << "A new client is connecting !" << std::endl;
	this->other = this->server->nextPendingConnection();
	connect(this->other, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(this->other, SIGNAL(readyRead()), this, SLOT(onData()));

	Game* game = Game::getInstance();
    game->receiveNetwork(this);
	game->setupGame(1000,true,0,1);
	this->win->receiveGame(game); //enables the gameplay as well
	this->team = 'o';
	this->isConfigured = true;

	QJsonArray map;
	std::vector<std::vector<int> > intMap = game->getIntMap();
	for(unsigned int i=0;i<intMap.size();i++){
		QJsonArray mapLine;
		for(unsigned int j=0;j<intMap[i].size();j++){
			mapLine.append(intMap[i][j]);
		}
		map.append(mapLine);
	}

	QJsonArray units;

	QJsonObject config;
	config["income"] = game->getIncome();
	config["map"] = 25087;//map;	//TODO input choice
	config["firstplayer"] = 5;
	config["secondplayer"] = 10;
	config["youplay"] = "secondplayer";
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
    std::cout << "Incoming data ! nbytes = " << this->other->bytesAvailable() << std::endl;
	if(this->currentSize == 0) {
		if(this->other->bytesAvailable() < 4)
			return;

		QDataStream in(this->other);
		in >> this->currentSize;
	}

	if(this->other->bytesAvailable() < this->currentSize)
        return;

	QByteArray data = this->other->read(this->currentSize);
	this->currentSize = 0;

	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonObject json = doc.object();

	QStringList keys = json.keys();

	if(! this->isConfigured) { //first transmission
		// recieves the map


		Game* game = Game::getInstance();
        game->receiveNetwork(this);
		if(!json["map"].isArray()){ //TODO array, parameter map num
			std::vector<std::vector<int> > intMap = MapBuilder::makeIntMap(":/Map/Images/Maps/Map.txt");
			game->setIntMap(intMap);
			game->setPath(":/Map/Images/Maps/Map.txt");
		}else{
			std::vector<std::vector<int> > intMap;
			QJsonArray map = json["map"].toArray();
			for(int i=0;i<map.size();i++){
				QJsonArray line = map[i].toArray();
				intMap.push_back(std::vector<int>());
				for(int j=0;j<line.size();j++){
					intMap[i].push_back(line[j].toInt());
				}
			}
			game->setIntMap(intMap);
			game->setPath(":/Map/Images/Maps/Map.txt");

		}

		switch(json[json["youplay"].toString()].toInt()){
			case 1: case 3: case 5: case 6: case 9: this->team = 'o'; break;
			case 2: case 4: case 7: case 8: case 10: this->team = 'b'; break;
		}
		bool iBegin = false;
		if(json["youplay"] == "firstplayer") iBegin = true;
		if((iBegin && this->team == 'b') || (!iBegin && this->team == 'o')){
			game->setupGame(json["income"].toInt(),false,0,1,false); //TODO AI settings
		}else{
			game->setupGame(json["income"].toInt(),true,0,1,false);
		}

		QJsonArray units = json["units"].toArray();
		for(int i=0;i<units.size();i++){
			QJsonObject unit = units[i].toObject();
			int country;
			switch(unit["country"].toInt()){
				case 1: case 3: case 5: case 6: case 9: country = 1; break;
				case 2: case 4: case 7: case 8: case 10: country = 2; break;
			}
			game->networkAction("newunit",unit["x"].toInt(),unit["y"].toInt(),
						this->getUnitType(unit["type"].toString()),country);
		}

		this->win->receiveGame(game); //enables the gameplay as well
		this->isConfigured = true;
	} else {
		//normal gameplay
		if(keys[0] == "move"){
			QJsonArray arr = json[keys[0]].toArray();
			int x = arr[0].toInt();
			int y = arr[1].toInt();
			int newX = arr[2].toInt();
			int newY = arr[3].toInt();
			if(x != newX || y != newY){
				Game::getInstance()->networkAction("move",arr[0].toInt(),arr[1].toInt(),arr[2].toInt(),arr[3].toInt());
			}
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

		this->win->update();
	}

    if(this->other->bytesAvailable())
        onData();
}

void Network::sendData(QString type, int x, int y, int data, int data2){
	QJsonObject action;
	if(type == "move"){
		QJsonArray arr;
		arr.append(x); arr.append(y);
		arr.append(data); arr.append(data2);  //data is newX and newY
		action["move"] = arr;
	}else if(type == "move+fusion"){
		QJsonArray arr;
		arr.append(x); arr.append(y);
		arr.append(data); arr.append(data2);
		action["move"] = arr;
		action["join"] = true;
	}else if(type == "attack"){
		QJsonArray arr;
		arr.append(x); arr.append(y);
		arr.append(x); arr.append(y);
		action["move"] = arr;
		QJsonArray pos;
		pos.append(data); pos.append(data2);
		action["attack"] = pos;
	}else if(type == "capture"){
		QJsonArray arr;
		arr.append(x); arr.append(y);
		arr.append(x); arr.append(y);
		action["move"] = arr;
		action["capture"] = true;
	}else if(type == "newunit"){
		QJsonArray pos;
		pos.append(x); pos.append(y);
        action["build"] = pos;
		action["type"] = this->getUnitName(data);
	}else if(type == "endTurn"){
		action["endofturn"] =true;
	}
	this->sendJson(action);
}

void Network::sendJson(const QJsonObject& obj) { //updated to reference
	QByteArray data = QJsonDocument(obj).toJson();
    QDataStream out(this->other);
    out << (quint32) data.length();
    this->other->write(data);

    std::cout << "Sending " << data.length() << " bytes :" << data.toStdString() << std::endl;
}

int Network::getUnitType(const QString & name)
{
	if(name == "infantry"){
		return 0;
	}else if(name == "bazooka"){
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
