#ifndef TANK_E_H
#define TANK_E_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "vulkan_earth/Tank.h"

class TankE : public Tank {
public:
    TankE();
    TankE(float x, float y, float z);
    ~TankE() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif