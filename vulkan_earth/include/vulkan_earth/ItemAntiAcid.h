#ifndef VULKAN_EARTH_ITEMANTIACID_H
#define VULKAN_EARTH_ITEMANTIACID_H

#include <cstdint>
#include "vulkan_earth/Item.h"

class ItemAntiAcid : public Item {
public:
    ItemAntiAcid();
    ItemAntiAcid(std::int32_t id);
    ~ItemAntiAcid() override;
    ItemAntiAcid* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif