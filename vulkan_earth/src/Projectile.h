#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include <stdio.h>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

const int DEFAULT_DAMAGE = 100;
const int DEFAULT_RADIUS = 5;

class ChaseCam;
class Weapon;
class VBOShaderLibrary;
class GameState;

class Projectile {
	public:
		Projectile();
		Projectile(GameState* parent, GLfloat* turretMatrix, GLfloat speed, VBOShaderLibrary** projectileModels);
		~Projectile();
		void draw();
		//void update(GLfloat gravity);
		void update(GLfloat X, GLfloat Y, GLfloat Z);
		void chaseView();
		GLfloat* getPos();
		Weapon* getWeapon();
		void setWeapon(Weapon* wpn);
		Weapon* getDefaultWeapon();
		int getDefaultDamage();
		int getDefaultRadius();
		ChaseCam* getChaseCam();
		int getRadius();
		int getDamage();
		GLfloat getVox();
		GLfloat getVoy();
		GLfloat getVoz();
		GLfloat getXo();
		GLfloat getYo();
		GLfloat getZo();
		GLfloat getInitialPositionScalar();
	private:
		ChaseCam* chaseCam;
		GLfloat pos[3];
		GLfloat vVec[3];
		GLfloat speed;
		GLfloat wind; //implement later
		Weapon* weapon;
		Weapon* defaultWeapon;
		VBOShaderLibrary* projectileDefault;
		VBOShaderLibrary** projectileModels;
		float rotate;
		float y_not;
		GLfloat scalar;
		int timer;
		bool printed;
		GameState* parent;
		GLfloat Xo;
		GLfloat Yo;
		GLfloat Zo;
};

#endif /*	PROJECTILE_H_	*/