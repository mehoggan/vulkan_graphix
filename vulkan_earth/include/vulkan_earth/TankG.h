#ifndef TANK_G_H
#define TANK_G_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "vulkan_earth/Tank.h"

class TankG : public Tank {
public:
    TankG();
    TankG(float x, float y, float z);
    ~TankG() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    std::string getName() override;
    void buildList();
    void drawTankHitBox() override;
    void updateHitBox() override;
};

#endif