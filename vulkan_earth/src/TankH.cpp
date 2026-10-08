#include "vulkan_earth/TankH.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/VBOShaderLibrary.h"

const char* tank_h_name = "Predator";
const std::int32_t tank_h_hp = 1000;
const std::int32_t tank_h_power = 10;
const std::int32_t tank_h_armor = 10;
const std::int32_t tank_h_speed = 100;

TankH::TankH() = default;
TankH::TankH(float /*x*/, float /*y*/, float /*z*/) {
  initBody();
  initHead();
  initTurret();
  initWheel();
  initDuration();

  m_turret_offset[0] = 0;
  m_turret_offset[1] = 0;
  m_turret_offset[2] = -35;
  m_head_offset[0] = 0;
  m_head_offset[1] = 65;
  m_head_offset[2] = 10;
  m_body_offset[0] = 0;
  m_body_offset[1] = 80;
  m_body_offset[2] = 0;

  for (std::int32_t i = 0; i < 3; i++) {
    m_body_scale[i] = 70;
    m_head_scale[i] = 80;
    m_turret_scale[i] = 70;
  }

  m_power = tank_h_power;
  m_armor = tank_h_armor;
  m_speed = tank_h_speed;
  m_current_power = 10;
  m_previous_power = 1000;
  m_previous_angle = 1;
  m_hp = m_armor * 100;

  m_vbo_shader_head = new VBOShaderLibrary();
  m_vbo_shader_body = new VBOShaderLibrary();
  m_vbo_shader_turret = new VBOShaderLibrary();

  m_vbo_shader_turret->loadClientData("./Predator/Predator_Turret.ogl");
  m_vbo_shader_body->loadClientData("./Predator/Predator_Body.ogl");
  m_vbo_shader_head->loadClientData("./Predator/Predator_Head.ogl");

  m_vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
  m_vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
  m_vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

  m_projectile_land_pos[0] = 9999999;
  m_projectile_land_pos[1] = 9999999;
}
TankH::~TankH() {
  delete m_vbo_shader_head;
  delete m_vbo_shader_body;
  delete m_vbo_shader_turret;
}

// GETTERS
std::int32_t TankH::getBaseHP() { return tank_h_hp; }
std::int32_t TankH::getBasePower() { return tank_h_power; }
std::int32_t TankH::getBaseArmor() { return tank_h_armor; }
std::int32_t TankH::getBaseSpeed() { return tank_h_speed; }
std::string TankH::getName() { return tank_h_name; }
