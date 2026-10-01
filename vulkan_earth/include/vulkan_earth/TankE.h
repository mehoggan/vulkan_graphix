#ifndef TANK_E_H
#define TANK_E_H

#include <GL/glew.h>
#include <GL/freeglut.h>
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