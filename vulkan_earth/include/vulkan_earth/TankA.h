#ifndef TANK_A_H
#define TANK_A_H

#include <cstdint>
#include "vulkan_earth/Tank.h"

class TankA : public Tank {
public:
    TankA();
    TankA(float x, float y, float z);
    ~TankA() override;

    std::int32_t getBaseHP() override;
    std::int32_t getBasePower() override;
    std::int32_t getBaseArmor() override;
    std::int32_t getBaseSpeed() override;
    std::string getName() override;
    void updateHitBox() override;
};

#endif