#include "vulkan_earth/TankF.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/MacroCrtdbg.h"

const char* tank_f_name = "Behemoth";
const std::int32_t tank_f_hp = 1000;
const std::int32_t tank_f_power = 8;
const std::int32_t tank_f_armor = 8;
const std::int32_t tank_f_speed = 20;

TankF::TankF() = default;
TankF::TankF(float /*x*/, float /*y*/, float /*z*/) {
  initBody();
  initHead();
  initTurret();
  initWheel();
  initDuration();

  m_turret_offset[0] = 0;
  m_turret_offset[1] = 20;
  m_turret_offset[2] = -20.0001;
  m_head_offset[0] = 0;
  m_head_offset[1] = 80;
  m_head_offset[2] = 100;
  m_body_offset[0] = 0;
  m_body_offset[1] = 80;
  m_body_offset[2] = 0;

  for (std::int32_t i = 0; i < 3; i++) {
    m_body_scale[i] = 60;
    m_head_scale[i] = 60;
    m_turret_scale[i] = 60;
    m_wheel_scale[i] = 60;
  }

  m_power = tank_f_power;
  m_armor = tank_f_armor;
  m_speed = tank_f_speed;
  m_current_power = 10;
  m_previous_power = 1000;
  m_previous_angle = 1;
  m_hp = m_armor * 100;

  m_vbo_shader_head = new VBOShaderLibrary();
  m_vbo_shader_body = new VBOShaderLibrary();
  m_vbo_shader_turret = new VBOShaderLibrary();
  // vbo_shader_wheel=new VBOShaderLibrary();

  m_vbo_shader_turret->loadClientData("./Behemoth/Behemoth_Turret.ogl");
  m_vbo_shader_body->loadClientData("./Behemoth/Behemoth_Body.ogl");
  m_vbo_shader_head->loadClientData("./Behemoth/Behemoth_Head.ogl");
  // vbo_shader_wheel->loadClientData("./Behemoth/Behemoth_Wheel.ogl");

  m_vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
  m_vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
  m_vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);
  // vbo_shader_wheel->LoadTexture("TestImage.raw",1024,1024);

  m_projectile_land_pos[0] = 9999999;
  m_projectile_land_pos[1] = 9999999;
}
TankF::~TankF() {
  delete m_vbo_shader_head;
  delete m_vbo_shader_body;
  delete m_vbo_shader_turret;
  // delete vbo_shader_wheel;
}

// GETTERS
std::int32_t TankF::getBaseHP() { return tank_f_hp; }
std::int32_t TankF::getBasePower() { return tank_f_power; }
std::int32_t TankF::getBaseArmor() { return tank_f_armor; }
std::int32_t TankF::getBaseSpeed() { return tank_f_speed; }
std::string TankF::getName() { return tank_f_name; }
