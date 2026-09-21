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
    int uniqueidentifier;
    std::string image_file_name;
    std::string description;
    int price;
    int package_num;
    int max_stack;
    int remaining;
    int special_num;
};

#endif  //	ITEM_H