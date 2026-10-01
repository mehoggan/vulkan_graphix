#ifndef WEAPON_H
#define WEAPON_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "vulkan_earth/OpenGLColors.h"
#include "vulkan_earth/Tank.h"

class Weapon {
public:
    Weapon();
    virtual ~Weapon();
    /*	GETTERS AND SETTERS	*/
    virtual Weapon* getWeaponInstance() = 0;
    int getUNIQUEIDENTIFIER();
    float getScale();
    void setScale(float new_scale);
    int getRemaining();
    void setRemaining(int r);
    std::string getImageFileName();
    std::string getDescription();
    int getPrice();
    int getPackageNum();
    int getMaxStack();
    float getRadius();
    int getDamage();
    float* getExplosionColor1();
    float* getExplosionColor2();
    float* getExplosionColor3();
    float* getExplosionColor4();
    virtual void causeEffectToTank(float distance, Tank* tank);
    virtual void playFireSFX();
    virtual void playExplosionSFX();

protected:
    int uniqueidentifier;
    float explosion_color1[3];
    float explosion_color2[3];
    float explosion_color3[3];
    float explosion_color4[3];
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