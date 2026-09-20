#ifndef TANK_D_H
#define TANK_D_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Tank.h"

class TankD : public Tank {
public:
    TankD();
    TankD(GLfloat x, GLfloat y, GLfloat z);
    ~TankD() override;

    int getBaseHP() override;
    int getBasePower() override;
    int getBaseArmor() override;
    int getBaseSpeed() override;
    std::string getName() override;
    void buildList();
};

#endif