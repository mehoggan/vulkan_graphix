#ifndef TANK_E_H
#define TANK_E_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class TankE : public Tank{
public:
	TankE();
	TankE(GLfloat x, GLfloat y, GLfloat z);
	~TankE();

	int getBaseHP();
	int getBasePower();
	int getBaseArmor();
	int getBaseSpeed();
	const char* getName();
	void buildList();
};

#endif