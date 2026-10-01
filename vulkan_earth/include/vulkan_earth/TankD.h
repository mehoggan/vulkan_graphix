#ifndef TANK_D_H
#define TANK_D_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "vulkan_earth/Tank.h"

class TankD : public Tank {
public:
    TankD();
    TankD(float x, float y, float z);
    ~TankD() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif