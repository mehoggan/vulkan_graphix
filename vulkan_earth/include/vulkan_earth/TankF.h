#ifndef TANK_F_H
#define TANK_F_H

#include <cstdint>
#include "vulkan_earth/Tank.h"

class TankF : public Tank {
public:
    TankF();
    TankF(float x, float y, float z);
    ~TankF() override;

    std::int32_t getBaseHP() override;
    std::int32_t getBasePower() override;
    std::int32_t getBaseArmor() override;
    std::int32_t getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif