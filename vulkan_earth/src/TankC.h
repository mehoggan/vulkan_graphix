#ifndef TANK_C_H
#define TANK_C_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Tank.h"

class TankC : public Tank {
public:
    TankC();
    TankC(GLfloat x, GLfloat y, GLfloat z);
    ~TankC() override;

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