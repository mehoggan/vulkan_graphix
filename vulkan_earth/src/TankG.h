#ifndef TANK_G_H
#define TANK_G_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Tank.h"

class TankG : public Tank {
public:
    TankG();
    TankG(GLfloat x, GLfloat y, GLfloat z);
    ~TankG() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    const char* getName() override;
    void buildList();
    void drawTankHitBox() override;
    void updateHitBox() override;
};

#endif