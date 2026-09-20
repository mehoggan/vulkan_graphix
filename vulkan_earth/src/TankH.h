#ifndef TANK_H_H
#define TANK_H_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Tank.h"

class TankH : public Tank {
public:
    TankH();
    TankH(GLfloat x, GLfloat y, GLfloat z);
    ~TankH() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    std::string getName() override;
};

#endif