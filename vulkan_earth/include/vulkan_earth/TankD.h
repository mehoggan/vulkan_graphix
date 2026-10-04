#ifndef VULKAN_EARTH_TANKD_H
#define VULKAN_EARTH_TANKD_H

#include <cstdint>
#include "vulkan_earth/Tank.h"

class TankD : public Tank {
public:
    TankD();
    TankD(float x, float y, float z);
    ~TankD() override;

    std::int32_t getBaseHP() override;
    std::int32_t getBasePower() override;
    std::int32_t getBaseArmor() override;
    std::int32_t getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif