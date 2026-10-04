#ifndef VULKAN_EARTH_WEAPON_H
#define VULKAN_EARTH_WEAPON_H

#include <cstdint>
#include <string>
#include "vulkan_earth/Tank.h"
#include "vulkan_graphix/GameCatalog.h"

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
    // Every field but the id, from the game's catalog.
    void loadSpec(const vulkan_graphix::GameCatalog::WeaponSpec& spec);

    std::int32_t m_uniqueidentifier;
    float m_explosion_color1[3];
    float m_explosion_color2[3];
    float m_explosion_color3[3];
    float m_explosion_color4[3];
    float m_radius;
    std::int32_t m_damage;
    float m_scale;
    std::string m_image_file_name;
    std::string m_description;
    std::int32_t m_price;
    std::int32_t m_package_num;
    std::int32_t m_max_stack;
    std::int32_t m_remaining;
    std::int32_t m_special_number;
};

#endif  // VULKAN_EARTH_WEAPON_H