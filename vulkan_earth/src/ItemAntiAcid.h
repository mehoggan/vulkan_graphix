#ifndef ITEM_ANTI_ACID_H
#define ITEM_ANTI_ACID_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Item.h"

class ItemAntiAcid : public Item {
public:
    ItemAntiAcid();
    ItemAntiAcid(int id);
    ~ItemAntiAcid() override;
    ItemAntiAcid* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif