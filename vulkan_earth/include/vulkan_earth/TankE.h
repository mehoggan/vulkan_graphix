#ifndef VULKAN_EARTH_TANKE_H
#define VULKAN_EARTH_TANKE_H

#include <cstdint>
#include "vulkan_earth/Tank.h"

class TankE : public Tank {
public:
    TankE();
    TankE(float x, float y, float z);
    ~TankE() override;

    std::int32_t getBaseHP() override;
    std::int32_t getBasePower() override;
    std::int32_t getBaseArmor() override;
    std::int32_t getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif