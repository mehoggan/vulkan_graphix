#ifndef ITEM_FLOAT_H
#define ITEM_FLOAT_H

#include <GL/glew.h>
#include <GL/freeglut.h>
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