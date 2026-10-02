#ifndef ITEM_SHIELD_H
#define ITEM_SHIELD_H

#include <cstdint>
#include "vulkan_earth/Item.h"

class ItemShield : public Item {
public:
    ItemShield();
    ItemShield(std::int32_t id);
    ~ItemShield() override;
    ItemShield* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
    void playUseSFX() override;
};

#endif