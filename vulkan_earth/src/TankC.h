#ifndef TANK_C_H
#define TANK_C_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class TankC : public Tank{
public:
	TankC();
	TankC(GLfloat x, GLfloat y, GLfloat z);
	~TankC();

	int getBaseHP();
	int getBasePower();
	int getBaseArmor();
	int getBaseSpeed();
	const char* getName();
	void buildList();
	virtual void drawTankHitBox();
	virtual void updateHitBox();
};

#endif