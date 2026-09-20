#ifndef TANK_F_H
#define TANK_F_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class TankF : public Tank{
public:
	TankF();
	TankF(GLfloat x, GLfloat y, GLfloat z);
	~TankF();

	int getBaseHP();
	int getBasePower();
	int getBaseArmor();
	int getBaseSpeed();
	const char* getName();
	void buildList();
};

#endif