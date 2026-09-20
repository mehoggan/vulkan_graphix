#ifndef WEAPON_H
#define WEAPON_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGLColors.h"
#include "Tank.h"

class Weapon {
public:
    Weapon();
    virtual ~Weapon();
    /*	GETTERS AND SETTERS	*/
    virtual Weapon* getWeaponInstance() = 0;
    int getUNIQUEIDENTIFIER();
    float getScale();
    void setScale(float scale);
    int getRemaining();
    void setRemaining(int r);
    const char* getImageFileName();
    const char* getDescription();
    int getPrice();
    int getPackageNum();
    int getMaxStack();
    float getRadius();
    int getDamage();
    GLfloat* getExplosionColor1();
    GLfloat* getExplosionColor2();
    GLfloat* getExplosionColor3();
    GLfloat* getExplosionColor4();
    virtual void causeEffectToTank(GLfloat distance, Tank* tank);
    virtual void playFireSFX();
    virtual void playExplosionSFX();

protected:
    int UNIQUEIDENTIFIER;
    GLfloat explosionColor1[3];
    GLfloat explosionColor2[3];
    GLfloat explosionColor3[3];
    GLfloat explosionColor4[3];
    float radius;
    int damage;
    float scale;
    char* imageFileName;
    char* description;
    int price;
    int packageNum;
    int maxStack;
    int remaining;
    int specialNumber;
};

#endif  //	WEAPON_H