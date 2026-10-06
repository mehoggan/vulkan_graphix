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
std::int32_t Weapon::getUNIQUEIDENTIFIER() { return m_uniqueidentifier; }
float Weapon::getScale() { return m_scale; }
std::int32_t Weapon::getRemaining() { return m_remaining; }
std::string Weapon::getImageFileName() { return m_image_file_name; }
std::string Weapon::getDescription() { return m_description; }
std::int32_t Weapon::getPrice() { return m_price; }
std::int32_t Weapon::getPackageNum() { return m_package_num; }
std::int32_t Weapon::getMaxStack() { return m_max_stack; }
float Weapon::getRadius() { return m_radius; }
std::int32_t Weapon::getDamage() { return m_damage; }
float* Weapon::getExplosionColor1() { return m_explosion_color1; }
float* Weapon::getExplosionColor2() { return m_explosion_color2; }
float* Weapon::getExplosionColor3() { return m_explosion_color3; }
float* Weapon::getExplosionColor4() { return m_explosion_color4; }
/*SETTERS*/
void Weapon::setScale(float new_scale) { m_scale = new_scale; }
void Weapon::setRemaining(std::int32_t r) { m_remaining = r; }
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

void Weapon::loadSpec(const vulkan_graphix::GameCatalog::WeaponSpec& spec) {
  m_max_stack = spec.m_max_stack;
  m_package_num = spec.m_package_num;
  m_remaining = spec.m_remaining;
  m_scale = spec.m_scale;
  m_image_file_name = spec.m_image_file;
  m_description = spec.m_description;
  m_price = spec.m_price;
  m_radius = spec.m_radius;
  m_damage = spec.m_damage;
  m_special_number = spec.m_special_number;
  for (std::int32_t i = 0; i < 3; i++) {
    m_explosion_color1[i] = spec.m_explosion_colors[0][i];
    m_explosion_color2[i] = spec.m_explosion_colors[1][i];
    m_explosion_color3[i] = spec.m_explosion_colors[2][i];
    m_explosion_color4[i] = spec.m_explosion_colors[3][i];
  }
}
