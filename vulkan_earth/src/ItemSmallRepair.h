#ifndef ITEM_SMALL_REPAIR_H
#define ITEM_SMALL_REPAIR_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Item.h"

class ItemSmallRepair : public Item {
public:
    ItemSmallRepair();
    ItemSmallRepair(int ID);
    ~ItemSmallRepair() override;
    ItemSmallRepair* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif