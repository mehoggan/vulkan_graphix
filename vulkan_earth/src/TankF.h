#ifndef TANK_F_H
#define TANK_F_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Tank.h"

class TankF : public Tank {
public:
    TankF();
    TankF(GLfloat x, GLfloat y, GLfloat z);
    ~TankF() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif