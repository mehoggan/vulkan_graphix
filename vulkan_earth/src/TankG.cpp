#include "vulkan_earth/TankG.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/MacroCrtdbg.h"

const char* tank_g_name = "Cubix";
const std::int32_t tank_g_hp = 1000;
const std::int32_t tank_g_power = 10;
const std::int32_t tank_g_armor = 5;
const std::int32_t tank_g_speed = 75;

TankG::TankG() = default;
TankG::TankG(float /*x*/, float /*y*/, float /*z*/) {
  initBody();
  initHead();
  initTurret();
  initWheel();
  initDuration();

  m_turret_offset[0] = 0;
  m_turret_offset[1] = 22;
  m_turret_offset[2] = 12;
  m_head_offset[0] = 0;
  m_head_offset[1] = 0;
  m_head_offset[2] = 0;
  m_body_offset[0] = 0;
  m_body_offset[1] = 120;
  m_body_offset[2] = 0;

  for (std::int32_t i = 0; i < 3; i++) {
    m_body_scale[i] = 100;
    m_head_scale[i] = 100;
    m_turret_scale[i] = 100;
    m_wheel_scale[i] = 100;
  }

  m_power = tank_g_power;
  m_armor = tank_g_armor;
  m_speed = tank_g_speed;
  m_current_power = 10;
  m_previous_power = 1000;
  m_previous_angle = 1;
  m_hp = m_armor * 100;

  m_vbo_shader_head = new VBOShaderLibrary();
  m_vbo_shader_body = new VBOShaderLibrary();
  m_vbo_shader_turret = new VBOShaderLibrary();

  m_vbo_shader_turret->loadClientData("./Cubix/Cubix_Turret.ogl");
  m_vbo_shader_body->loadClientData("./Cubix/Cubix_Body.ogl");
  m_vbo_shader_head->loadClientData("./Cubix/Cubix_Head.ogl");

  m_vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
  m_vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
  m_vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

  m_projectile_land_pos[0] = 9999999;
  m_projectile_land_pos[1] = 9999999;
}
TankG::~TankG() {
  delete m_vbo_shader_head;
  delete m_vbo_shader_body;
  delete m_vbo_shader_turret;
}

// GETTERS
std::int32_t TankG::getBaseHP() { return tank_g_hp; }
std::int32_t TankG::getBasePower() { return tank_g_power; }
std::int32_t TankG::getBaseArmor() { return tank_g_armor; }
std::int32_t TankG::getBaseSpeed() { return tank_g_speed; }
std::string TankG::getName() { return tank_g_name; }

void TankG::updateHitBox() {
  m_tank_pos.x = m_head_matrix[12];
  m_tank_pos.y = m_head_matrix[13];
  m_tank_pos.z = m_head_matrix[14];
  m_right.x = -m_head_matrix[0];
  m_right.y = -m_head_matrix[1];
  m_right.z = -m_head_matrix[2];
  m_left.x = m_head_matrix[0];
  m_left.y = m_head_matrix[1];
  m_left.z = m_head_matrix[2];
  m_up.x = m_head_matrix[4];
  m_up.y = m_head_matrix[5];
  m_up.z = m_head_matrix[6];
  m_down.x = -m_head_matrix[4];
  m_down.y = -m_head_matrix[5];
  m_down.z = -m_head_matrix[6];
  m_at.x = -m_head_matrix[8];
  m_at.y = -m_head_matrix[9];
  m_at.z = -m_head_matrix[10];
  m_back.x = m_head_matrix[8];
  m_back.y = m_head_matrix[9];
  m_back.z = m_head_matrix[10];
}