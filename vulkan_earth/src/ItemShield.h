#ifndef ITEM_SHIELD_H
#define ITEM_SHIELD_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Item.h"

class ItemShield : public Item {
public:
    ItemShield();
    ItemShield(int ID);
    ~ItemShield() override;
    ItemShield* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif