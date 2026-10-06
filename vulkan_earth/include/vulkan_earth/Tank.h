#ifndef VULKAN_EARTH_TANK_H
#define VULKAN_EARTH_TANK_H

#include <cstdint>
#include <string>
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Renderer.h"

// class vulkan_graphix::Math::Vec3<float>;

class ParticleGenerator;
class VBOShaderLibrary;

namespace vulkan_graphix::Render {
class RenderContext;
}

class Tank {
public:
  Tank();
  virtual ~Tank();

  // GETTERS
  vulkan_graphix::Math::Vec3<float> getAlignmentVector();
  vulkan_graphix::Math::Vec3<float> getRotateAbout();
  void resetTurret();
  void rotateHead(float degrees);
  void rotateTurret(float degrees);
  void rotateWheel(float degrees);
  void adjustPower(float amount);
  float getCurrentPower();
  void setPreviousPower(std::int32_t new_previous_power);
  void setPreviousAngle(std::int32_t new_previous_angle);
  void setProjectileLandPos(float x, float y);
  std::int32_t getPreviousPower();
  std::int32_t getPreviousAngle();
  float* getProjectileLandPos();
  const float* getBodyMatrix();
  const float* getBodyColor();
  float* getBodyScale();
  const float* getHeadMatrix();
  const float* getHeadColor();
  float* getHeadScale();
  float* getTurretMatrix();
  const float* getTurretColor();
  float* getTurretScale();
  float getTurretDegrees();
  const float* getWheelMatrix();
  const float* getWheelColor();
  float* getWheelScale();
  std::int32_t getHP();
  std::int32_t getPower();
  std::int32_t getArmor();
  std::int32_t getSpeed();
  Tank* getTankPointer();
  virtual std::int32_t getBaseHP();
  virtual std::int32_t getBasePower();
  virtual std::int32_t getBaseArmor();
  virtual std::int32_t getBaseSpeed();
  virtual std::string getName();

  // SETTERS
  void setTankPos(float x, float y, float z);
  void orientTank(vulkan_graphix::Math::Vec3<float>* n);

  void setBodyColor(float r, float g, float b, float a);
  void setBodyScale(float x, float y, float z);

  void setHeadColor(float r, float g, float b, float a);
  void setHeadScale(float x, float y, float z);

  void setTurretColor(float r, float g, float b, float a);
  void setTurretScale(float x, float y, float z);

  void setWheelColor(float r, float g, float b, float a);
  void setWheelScale(float x, float y, float z);

  void setHP(std::int32_t new_hp);
  void setPower(std::int32_t p);
  void setArmor(std::int32_t a);
  void setSpeed(std::int32_t d);

  std::int32_t getCurrentHeight();
  void setCurrentHeight(std::int32_t curr_height);
  std::int32_t getPreviousHeight();
  void setPreviousHeight(std::int32_t prev_height);

  // Other functions
  bool checkCollision(float x, float y, float z);
  void fire();
  void keyHandler();
  void draw(vulkan_graphix::Render::RenderContext& context);
  virtual void drawTankHitBox(vulkan_graphix::Render::RenderContext& context);
  virtual void updateHitBox();
  void changeHeadTexture(std::int32_t current_player_index);
  void dealDamage(std::int32_t damage);
  void checkFallingDamage();
  bool isAlive();
  void tankRevive();

  void initBody();
  void initHead();
  void initTurret();
  void initWheel();
  void initDuration();

  // Getters for durations
  std::int32_t getDurationAcid();
  std::int32_t getDurationShield();
  std::int32_t getDurationEMP();
  std::int32_t getDurationFloat();
  std::int32_t getDurationDoubleAction();
  std::int32_t getDurationPadlock();
  std::int32_t getDurationCloak();
  std::int32_t getDurationParalyze();
  // Setters for durations
  void setDurationAcid(std::int32_t value);
  void setDurationShield(std::int32_t value);
  void setDurationEMP(std::int32_t value);
  void setDurationFloat(std::int32_t value);
  void setDurationDoubleAction(std::int32_t value);
  void setDurationPadlock(std::int32_t value);
  void setDurationCloak(std::int32_t value);
  void setDurationParalyze(std::int32_t value);

  void setDurationAllPassTurn();

  void printTurretMatrix();
  void printBodyMatrix();
  void printHeadMatrix();

protected:
  float m_body_pos[3];
  float m_head_pos[3];
  float m_turret_pos[3];
  float m_wheel_pos[3];
  float m_body_right[3];
  float m_head_right[3];
  float m_turret_right[3];
  float m_wheel_right[3];
  float m_body_up[3];
  float m_head_up[3];
  float m_turret_up[3];
  float m_wheel_up[3];
  float m_body_at[3];
  float m_head_at[3];
  float m_turret_at[3];
  float m_wheel_at[3];
  float m_body_color[4];
  float m_head_color[4];
  float m_turret_color[4];
  float m_wheel_color[4];
  float m_body_scale[3];
  float m_head_scale[3];
  float m_turret_scale[3];
  float m_wheel_scale[3];
  float m_body_matrix[16];
  float m_head_matrix[16];
  float m_turret_matrix[16];
  float m_wheel_matrix[16];
  float m_rotate_degrees;
  float m_turret_degrees;
  float m_wheel_degrees;
  vulkan_graphix::Math::Vec3<float> m_alignment_vector;
  vulkan_graphix::Math::Vec3<float> m_rotate_about;

  float m_turret_offset[3];
  float m_head_offset[3];
  float m_body_offset[3];

  float m_hit_box_length;
  float m_hit_box_height;
  float m_hit_box_width;

  VBOShaderLibrary* m_vbo_shader_turret;
  VBOShaderLibrary* m_vbo_shader_body;
  VBOShaderLibrary* m_vbo_shader_head;
  VBOShaderLibrary* m_vbo_shader_wheel;

  bool m_tank_alive;
  ParticleGenerator* m_smoke_gen;
  ParticleGenerator* m_acid_gen;
  ParticleGenerator* m_float_gen;

  float m_current_power;
  std::int32_t m_previous_power;
  std::int32_t m_previous_angle;
  std::int32_t m_previous_height;
  std::int32_t m_current_height;

  std::int32_t m_hp;
  std::int32_t m_power;
  std::int32_t m_armor;
  std::int32_t m_speed;

  float m_projectile_land_pos[2];

  std::int32_t m_duration_acid;
  std::int32_t m_duration_shield;
  std::int32_t m_duration_emp;
  std::int32_t m_duration_float;
  std::int32_t m_duration_double_action;
  std::int32_t m_duration_padlock;
  std::int32_t m_duration_cloak;
  std::int32_t m_duration_paralyze;

  vulkan_graphix::Math::Vec3<float> m_tank_pos;
  vulkan_graphix::Math::Vec3<float> m_right =
      vulkan_graphix::Math::Vec3<float>(0.0f);
  vulkan_graphix::Math::Vec3<float> m_up =
      vulkan_graphix::Math::Vec3<float>(0.0f);
  vulkan_graphix::Math::Vec3<float> m_at =
      vulkan_graphix::Math::Vec3<float>(0.0f);
  vulkan_graphix::Math::Vec3<float> m_left =
      vulkan_graphix::Math::Vec3<float>(0.0f);
  vulkan_graphix::Math::Vec3<float> m_down =
      vulkan_graphix::Math::Vec3<float>(0.0f);
  vulkan_graphix::Math::Vec3<float> m_back =
      vulkan_graphix::Math::Vec3<float>(0.0f);
};

#endif