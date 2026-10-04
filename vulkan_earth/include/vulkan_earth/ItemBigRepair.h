#ifndef VULKAN_EARTH_ITEMBIGREPAIR_H
#define VULKAN_EARTH_ITEMBIGREPAIR_H

#include <cstdint>
#include "vulkan_earth/Item.h"

class ItemBigRepair : public Item {
public:
    ItemBigRepair();
    ItemBigRepair(std::int32_t id);
    ~ItemBigRepair() override;
    ItemBigRepair* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif