#ifndef TANK_B_H
#define TANK_B_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class TankB : public Tank{
public:
	TankB();
	TankB(GLfloat x, GLfloat y, GLfloat z);
	~TankB();

	int getBaseHP();
	int getBasePower();
	int getBaseArmor();
	int getBaseSpeed();
	const char* getName();
	void buildList();
};

#endif