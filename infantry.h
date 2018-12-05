#include "unit.h"
#include "building.h"

class Infantry : public Unit{
	Building* capturing;
  public:
	Infantry(int x, int y, int type, char team);
    virtual std::string getType() const;
	bool move(int dir, int terrainMod);
	void setCapture(Building*);
    bool isCapturing();
};
