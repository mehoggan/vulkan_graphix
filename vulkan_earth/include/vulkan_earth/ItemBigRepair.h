#ifndef ITEM_BIG_REPAIR_H
#define ITEM_BIG_REPAIR_H

#include <GL/glew.h>
#include <GL/freeglut.h>
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