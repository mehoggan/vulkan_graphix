#ifndef VULKAN_EARTH_ITEMDOUBLEACTION_H
#define VULKAN_EARTH_ITEMDOUBLEACTION_H

#include <cstdint>
#include "vulkan_earth/Item.h"

class ItemDoubleAction : public Item {
public:
    ItemDoubleAction();
    ItemDoubleAction(std::int32_t id);
    ~ItemDoubleAction() override;
    ItemDoubleAction* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
};

#endif