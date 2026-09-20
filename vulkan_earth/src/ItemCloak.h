#ifndef ITEM_CLOAK_H
#define ITEM_CLOAK_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Item.h"

class ItemCloak : public Item {
public:
    ItemCloak();
    ItemCloak(int ID);
    ~ItemCloak() override;
    ItemCloak* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
};

#endif