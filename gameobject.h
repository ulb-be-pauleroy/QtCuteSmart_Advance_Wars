#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include<string>

class GameObject
{
protected:
	int posX;
	int posY;

public:
	GameObject(int x, int y);
	int getPosX();
	int getPosY();
	virtual bool operator== (GameObject const& obj) const;// =0; // causes invalid abstract parameter
	virtual std::string getType() const =0; //same error
	//virtual void select();
	virtual void interactWith() =0;

    virtual ~GameObject() {} //otherwise we have a dyn cast problem
};

#endif // GAMEOBJECT_H
