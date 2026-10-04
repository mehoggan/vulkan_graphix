#ifndef VULKAN_EARTH_ITEMEXTRABATTERY_H
#define VULKAN_EARTH_ITEMEXTRABATTERY_H

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