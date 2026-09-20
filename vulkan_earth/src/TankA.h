#ifndef TANK_A_H
#define TANK_A_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Tank.h"

class TankA : public Tank {
public:
    TankA();
    TankA(GLfloat x, GLfloat y, GLfloat z);
    ~TankA() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    const char* getName() override;
    void drawTankHitBox() override;
    void updateHitBox() override;
};

#endif