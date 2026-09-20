#ifndef ITEM_FLOAT_H
#define ITEM_FLOAT_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Item.h"

class ItemFloat : public Item {
public:
    ItemFloat();
    ItemFloat(int ID);
    ~ItemFloat() override;
    ItemFloat* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif