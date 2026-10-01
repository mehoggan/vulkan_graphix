#ifndef ITEM_EXTRA_BATTERY_H
#define ITEM_EXTRA_BATTERY_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include "vulkan_earth/Item.h"

class ItemExtraBattery : public Item {
public:
    ItemExtraBattery();
    ItemExtraBattery(std::int32_t id);
    ~ItemExtraBattery() override;
    ItemExtraBattery* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif