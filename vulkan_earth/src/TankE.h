#ifndef TANK_E_H
#define TANK_E_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Tank.h"

class TankE : public Tank {
public:
    TankE();
    TankE(GLfloat x, GLfloat y, GLfloat z);
    ~TankE() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif