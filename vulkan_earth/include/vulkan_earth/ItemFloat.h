#ifndef VULKAN_EARTH_ITEMFLOAT_H
#define VULKAN_EARTH_ITEMFLOAT_H

#include <cstdint>
#include "vulkan_earth/Item.h"

class ItemFloat : public Item {
public:
    ItemFloat();
    ItemFloat(std::int32_t id);
    ~ItemFloat() override;
    ItemFloat* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif