#ifndef TANK_G_H
#define TANK_G_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include "vulkan_earth/Tank.h"

class TankG : public Tank {
public:
    TankG();
    TankG(float x, float y, float z);
    ~TankG() override;

    std::int32_t getBaseHP() override;
    std::int32_t getBasePower() override;
    std::int32_t getBaseArmor() override;
    std::int32_t getBaseSpeed() override;
    std::string getName() override;
    void buildList();
    void drawTankHitBox() override;
    void updateHitBox() override;
};

#endif