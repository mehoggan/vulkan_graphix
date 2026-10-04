#ifndef VULKAN_EARTH_ITEMCLOAK_H
#define VULKAN_EARTH_ITEMCLOAK_H

#include <cstdint>
#include "vulkan_earth/Item.h"

class ItemCloak : public Item {
public:
    ItemCloak();
    ItemCloak(std::int32_t id);
    ~ItemCloak() override;
    ItemCloak* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
};

#endif