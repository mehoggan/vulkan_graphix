#ifndef TANK_F_H
#define TANK_F_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "vulkan_earth/Tank.h"

class TankF : public Tank {
public:
    TankF();
    TankF(float x, float y, float z);
    ~TankF() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif