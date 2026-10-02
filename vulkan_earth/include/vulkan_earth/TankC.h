#ifndef TANK_C_H
#define TANK_C_H

#include <cstdint>
#include "vulkan_earth/Tank.h"

class TankC : public Tank {
public:
    TankC();
    TankC(float x, float y, float z);
    ~TankC() override;

    std::int32_t getBaseHP() override;
    std::int32_t getBasePower() override;
    std::int32_t getBaseArmor() override;
    std::int32_t getBaseSpeed() override;
    std::string getName() override;
    void buildList();
    void updateHitBox() override;
};

#endif