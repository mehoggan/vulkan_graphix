#ifndef ITEM_EXTRA_BATTERY_H
#define ITEM_EXTRA_BATTERY_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Item.h"

class ItemExtraBattery : public Item {
public:
    ItemExtraBattery();
    ItemExtraBattery(int id);
    ~ItemExtraBattery() override;
    ItemExtraBattery* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif