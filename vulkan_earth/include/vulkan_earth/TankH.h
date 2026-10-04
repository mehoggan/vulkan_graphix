#ifndef VULKAN_EARTH_TANKH_H
#define VULKAN_EARTH_TANKH_H

#include <cstdint>
#include "vulkan_earth/Tank.h"

class TankH : public Tank {
public:
    TankH();
    TankH(float x, float y, float z);
    ~TankH() override;

    std::int32_t getBaseHP() override;
    std::int32_t getBasePower() override;
    std::int32_t getBaseArmor() override;
    std::int32_t getBaseSpeed() override;
    std::string getName() override;
};

#endif