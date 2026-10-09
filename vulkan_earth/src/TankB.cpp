#include "vulkan_earth/TankB.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_graphix/HellfireTank.h"
#include "vulkan_graphix/Render/TexturedModel.h"

const char* tank_b_name = "Hellfire";
const std::int32_t tank_b_hp = 1000;
const std::int32_t tank_b_power = 7;
const std::int32_t tank_b_armor = 5;
const std::int32_t tank_b_speed = 50;

TankB::TankB() = default;
TankB::TankB(float /*x*/, float /*y*/, float /*z*/) {
  initBody();
  initHead();
  initTurret();
  initWheel();
  initDuration();

  // The Hellfire's part offsets and scale live in libvulkan_graphix's
  // HellfireTank, shared with the tutorials that draw it.
  const vulkan_graphix::TankPlacement::PartOffsets& offsets =
      vulkan_graphix::HellfireTank::getPartOffsets();
  for (std::int32_t i = 0; i < 3; i++) {
    m_turret_offset[i] = offsets.m_turret[i];
    m_head_offset[i] = offsets.m_head[i];
    m_body_offset[i] = offsets.m_body[i];
  }

  for (std::int32_t i = 0; i < 3; i++) {
    m_body_scale[i] = vulkan_graphix::HellfireTank::c_part_scale;
    m_head_scale[i] = vulkan_graphix::HellfireTank::c_part_scale;
    m_turret_scale[i] = vulkan_graphix::HellfireTank::c_part_scale;
    m_wheel_scale[i] = vulkan_graphix::HellfireTank::c_part_scale;
  }

  m_power = tank_b_power;
  m_armor = tank_b_armor;
  m_speed = tank_b_speed;
  m_current_power = 10;
  m_previous_power = 1000;
  m_previous_angle = 1;
  m_hp = m_armor * 100;

  m_vbo_shader_head = new vulkan_graphix::Render::TexturedModel();
  m_vbo_shader_body = new vulkan_graphix::Render::TexturedModel();
  m_vbo_shader_turret = new vulkan_graphix::Render::TexturedModel();

  m_vbo_shader_turret->loadOgl("./Hellfire/Hellfire_Turret.ogl");
  m_vbo_shader_body->loadOgl("./Hellfire/Hellfire_Body.ogl");
  m_vbo_shader_head->loadOgl("./Hellfire/Hellfire_Head.ogl");

  m_vbo_shader_turret->loadRawTexture("TestImage.raw", 1024, 1024);
  m_vbo_shader_body->loadRawTexture("TestImage.raw", 1024, 1024);
  m_vbo_shader_head->loadRawTexture("TestImage.raw", 1024, 1024);

  m_projectile_land_pos[0] = 9999999;
  m_projectile_land_pos[1] = 9999999;
}
TankB::~TankB() {
  delete m_vbo_shader_head;
  delete m_vbo_shader_body;
  delete m_vbo_shader_turret;
}

// GETTERS
std::int32_t TankB::getBaseHP() { return tank_b_hp; }
std::int32_t TankB::getBasePower() { return tank_b_power; }
std::int32_t TankB::getBaseArmor() { return tank_b_armor; }
std::int32_t TankB::getBaseSpeed() { return tank_b_speed; }
std::string TankB::getName() { return tank_b_name; }
