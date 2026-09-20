#ifndef TANK_B_H
#define TANK_B_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Tank.h"

class TankB : public Tank {
public:
    TankB();
    TankB(GLfloat x, GLfloat y, GLfloat z);
    ~TankB() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif