#include "vulkan_graphix/EffectSimulation.h"

#include <cmath>
#include <cstdlib>

namespace vulkan_graphix::EffectSimulation {

Particle makeParticle(
    ParticleKind kind,
    float x,
    float y,
    float z,
    float dir_x,
    float dir_y,
    float dir_z,
    float speed,
    std::int32_t frames) {
  Particle particle;
  particle.m_kind = kind;
  particle.m_x = x;
  particle.m_y = y;
  particle.m_z = z;
  particle.m_dir = {dir_x, dir_y, dir_z};
  particle.m_speed = speed;
  particle.m_active_frames = frames;
  particle.m_current_frame = 0;
  switch (kind) {
    case ParticleKind::Smoke:
      particle.m_size = 2;
      particle.m_red = 1;
      particle.m_green = 1;
      particle.m_blue = 1;
      break;
    case ParticleKind::Acid:
      particle.m_size = 2;
      particle.m_red = 0;
      particle.m_green = 1;
      particle.m_blue = 0;
      break;
    case ParticleKind::Float:
      particle.m_size = 4;
      particle.m_red = 1;
      particle.m_green = 1;
      particle.m_blue = 1;
      break;
  }
  return particle;
}

bool updateParticle(Particle& particle) {
  switch (particle.m_kind) {
    case ParticleKind::Smoke:
      particle.m_x += particle.m_dir[0] * particle.m_speed;
      particle.m_y += 7 *
          (static_cast<float>(particle.m_current_frame) /
           static_cast<float>(particle.m_active_frames));
      particle.m_z += particle.m_dir[2] * particle.m_speed;
      if (particle.m_current_frame <= 30) {
        particle.m_blue =
            1 - (static_cast<float>(particle.m_current_frame) / 30.0);
      } else if (particle.m_current_frame <= 60) {
        particle.m_green =
            1 - ((static_cast<float>(particle.m_current_frame) - 30.0) / 30.0);
      } else {
        // Measured from frame 30, not 60, as the game had it: red
        // drops straight to about 0.2 when this band starts.
        particle.m_red =
            1 - ((static_cast<float>(particle.m_current_frame) - 30.0) / 40.0);
      }
      break;
    case ParticleKind::Acid:
      particle.m_x += particle.m_dir[0] * particle.m_speed;
      particle.m_y += particle.m_dir[1] * particle.m_speed;
      particle.m_z += particle.m_dir[2] * particle.m_speed;
      break;
    case ParticleKind::Float:
      particle.m_x += particle.m_dir[0] * particle.m_speed;
      particle.m_y += particle.m_dir[1] * particle.m_speed / 10;
      particle.m_z += particle.m_dir[2] * particle.m_speed;
      break;
  }
  particle.m_current_frame++;
  return particle.m_current_frame != particle.m_active_frames;
}

ParticleEmitter::ParticleEmitter(
    std::int32_t spawn,
    std::int32_t /*rate*/,
    std::int32_t speed,
    std::int32_t life,
    ParticleKind kind) :
    m_particles_per_emission(spawn),
    m_emission_speed(speed),
    m_emission_life(life),
    m_kind(kind),
    m_slots(c_capacity) {}

void ParticleEmitter::update(float x, float y, float z) {
  m_x = x;
  m_y = y;
  m_z = z;
  for (std::optional<Particle>& slot : m_slots) {
    if (slot && !updateParticle(*slot)) {
      slot.reset();
    }
  }
  addParticles();
}

void ParticleEmitter::clear() {
  for (std::optional<Particle>& slot : m_slots) {
    slot.reset();
  }
}

const std::vector<std::optional<Particle>>& ParticleEmitter::slots() const {
  return m_slots;
}

void ParticleEmitter::addParticles() {
  std::int32_t count = 0;
  for (std::size_t i = 0;
       i < m_slots.size() && count < m_particles_per_emission;
       ++i) {
    if (m_slots[i]) {
      continue;
    }
    float dir_x =
        static_cast<float>(std::rand()) * 2 / static_cast<float>(RAND_MAX) - 1;
    float dir_y =
        static_cast<float>(std::rand()) * 2 / static_cast<float>(RAND_MAX) - 1;
    float dir_z =
        static_cast<float>(std::rand()) * 2 / static_cast<float>(RAND_MAX) - 1;
    float mag = std::sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
    dir_x /= mag;
    dir_y /= mag;
    dir_z /= mag;
    m_slots[i] = makeParticle(
        m_kind,
        m_x,
        m_y,
        m_z,
        dir_x,
        dir_y,
        dir_z,
        static_cast<float>(m_emission_speed),
        m_emission_life);
    count++;
  }
}

ExplosionFrame advanceExplosion(Explosion& explosion) {
  explosion.m_radius += .1;
  explosion.m_timer += .5;
  const float timer = explosion.m_timer;
  const float alpha = 1.0 - timer / 150.0;
  if (timer > 0 && timer < 25) {
    return {0, alpha};
  }
  if (timer >= 25 && timer < 50) {
    return {1, alpha};
  }
  if (timer >= 50 && timer < 75) {
    return {2, alpha};
  }
  if (timer >= 100) {
    return {3, alpha};
  }
  return {-1, alpha};
}

float explosionSphereRadius(
    const Explosion& explosion, std::int32_t weapon_radius) {
  // "ASSUMING SCALE ON TERRAIN IS 150 I NEED TO GET ACTUAL VALUE"
  return explosion.m_radius * (weapon_radius * 5.56 + 22.22);
}

}  // namespace vulkan_graphix::EffectSimulation
