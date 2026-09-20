#ifndef ITEM_H
#define ITEM_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "Tank.h"

class Item {
public:
    Item();
    virtual ~Item();
    /*	GETTERS AND SETTERS	*/
    virtual Item* getItemInstance() = 0;
    int getUNIQUEIDENTIFIER();
    int getRemaining();
    void setRemaining(int r);
    std::string getImageFileName();
    std::string getDescription();
    int getPrice();
    int getPackageNum();
    int getMaxStack();
    virtual bool causeEffectToTank(Tank* tank) = 0;
    virtual void playUseSFX();

protected:
    int UNIQUEIDENTIFIER;
    std::string imageFileName;
    std::string description;
    int price;
    int packageNum;
    int maxStack;
    int remaining;
    int specialNum;
};

#endif  //	ITEM_H