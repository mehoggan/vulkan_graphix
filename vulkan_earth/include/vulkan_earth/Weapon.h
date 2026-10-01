#ifndef WEAPON_H
#define WEAPON_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include <string>
#include "vulkan_earth/OpenGLColors.h"
#include "vulkan_earth/Tank.h"

class Weapon {
public:
    Weapon();
    virtual ~Weapon();
    /*	GETTERS AND SETTERS	*/
    virtual Weapon* getWeaponInstance() = 0;
    std::int32_t getUNIQUEIDENTIFIER();
    float getScale();
    void setScale(float new_scale);
    std::int32_t getRemaining();
    void setRemaining(std::int32_t r);
    std::string getImageFileName();
    std::string getDescription();
    std::int32_t getPrice();
    std::int32_t getPackageNum();
    std::int32_t getMaxStack();
    float getRadius();
    std::int32_t getDamage();
    float* getExplosionColor1();
    float* getExplosionColor2();
    float* getExplosionColor3();
    float* getExplosionColor4();
    virtual void causeEffectToTank(float distance, Tank* tank);
    virtual void playFireSFX();
    virtual void playExplosionSFX();

protected:
    std::int32_t uniqueidentifier;
    float explosion_color1[3];
    float explosion_color2[3];
    float explosion_color3[3];
    float explosion_color4[3];
    float radius;
    std::int32_t damage;
    float scale;
    std::string image_file_name;
    std::string description;
    std::int32_t price;
    std::int32_t package_num;
    std::int32_t max_stack;
    std::int32_t remaining;
    std::int32_t special_number;
};

#endif  //	WEAPON_H