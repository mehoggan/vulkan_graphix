#ifndef ITEM_ANTI_ACID_H
#define ITEM_ANTI_ACID_H

#include <GL/glew.h>
#include <GL/freeglut.h>
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