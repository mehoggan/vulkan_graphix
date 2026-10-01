#ifndef TANK_C_H
#define TANK_C_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "vulkan_earth/Tank.h"

class TankC : public Tank {
public:
    TankC();
    TankC(float x, float y, float z);
    ~TankC() override;

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