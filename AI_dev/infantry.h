#include "unit.h"
#include "city.h"

class Infantry : public Unit{
    City* capturing;
  public:
	Infantry(int x, int y, int type, char team);
    virtual std::string getType() const;
	bool move(int dir, int terrainMod);
    void setCapture(City*);
    bool isCapturing();
};
