#include "vulkan_earth/Weapon.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

using namespace std;

Weapon::Weapon() = default;
Weapon::~Weapon() = default;
/*GETTERS*/
std::int32_t Weapon::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
float Weapon::getScale() { return scale; }
std::int32_t Weapon::getRemaining() { return remaining; }
std::string Weapon::getImageFileName() { return image_file_name; }
std::string Weapon::getDescription() { return description; }
std::int32_t Weapon::getPrice() { return price; }
std::int32_t Weapon::getPackageNum() { return package_num; }
std::int32_t Weapon::getMaxStack() { return max_stack; }
float Weapon::getRadius() { return radius; }
std::int32_t Weapon::getDamage() { return damage; }
float* Weapon::getExplosionColor1() { return explosion_color1; }
float* Weapon::getExplosionColor2() { return explosion_color2; }
float* Weapon::getExplosionColor3() { return explosion_color3; }
float* Weapon::getExplosionColor4() { return explosion_color4; }
/*SETTERS*/
void Weapon::setScale(float new_scale) { scale = new_scale; }
void Weapon::setRemaining(std::int32_t r) { remaining = r; }
void Weapon::causeEffectToTank(float distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}

void Weapon::playFireSFX() {
    std::int32_t random = rand() % 3;

    if (random == 0)
        playSFX(TANK_FIRE1);
    else if (random == 1)
        playSFX(TANK_FIRE2);
    else
        playSFX(TANK_FIRE3);
}

void Weapon::playExplosionSFX() {
    std::int32_t random = rand() % 2;
    Mix_HaltChannel(0);

    if (random == 0)
        playSFX(EXPLOSION1);
    else
        playSFX(EXPLOSION2);
}

void Weapon::loadSpec(vulkan_graphix::GameCatalog::WeaponSpec const& spec) {
    max_stack = spec.max_stack;
    package_num = spec.package_num;
    remaining = spec.remaining;
    scale = spec.scale;
    image_file_name = spec.image_file;
    description = spec.description;
    price = spec.price;
    radius = spec.radius;
    damage = spec.damage;
    special_number = spec.special_number;
    for (std::int32_t i = 0; i < 3; i++) {
        explosion_color1[i] = spec.explosion_colors[0][i];
        explosion_color2[i] = spec.explosion_colors[1][i];
        explosion_color3[i] = spec.explosion_colors[2][i];
        explosion_color4[i] = spec.explosion_colors[3][i];
    }
}
