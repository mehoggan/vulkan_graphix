#ifndef WEAPON_H
#define WEAPON_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
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
    std::string getImageFileName();
    std::string getDescription();
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
    int uniqueidentifier;
    GLfloat explosion_color1[3];
    GLfloat explosion_color2[3];
    GLfloat explosion_color3[3];
    GLfloat explosion_color4[3];
    float radius;
    int damage;
    float scale;
    std::string image_file_name;
    std::string description;
    int price;
    int package_num;
    int max_stack;
    int remaining;
    int special_number;
};

#endif  //	WEAPON_H