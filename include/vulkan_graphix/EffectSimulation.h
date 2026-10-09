#ifndef VULKAN_GRAPHIX_EFFECTSIMULATION_H
#define VULKAN_GRAPHIX_EFFECTSIMULATION_H

// vulkan_earth's particle and explosion effects as plain simulations (no
// drawing): ParticleSmoke/ParticleAcid/ParticleFloat's constructors and
// update(), ParticleGenerator's pool and spawning, and Explosion's
// per-frame growth and color timeline - the one copy both the game and
// Tutorial20 run. Units are the game's own; a caller drawing at another
// scale scales the results.

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace vulkan_graphix::EffectSimulation {

// ParticleGenerator's type codes.
enum class ParticleKind : std::int32_t {
  // A tank's death smoke: rises faster as it ages, fading white ->
  // yellow -> red -> black.
  Smoke = 0,
  // Acid status: green, straight-line motion.
  Acid = 1,
  // Float status: white, its vertical motion damped tenfold.
  Float = 2,
};

// One particle - the fields vulkan_earth's Particle base class held. Every
// particle is drawn as a sphere of radius size at (x, y, z), in (red,
// green, blue) at alpha 0.4.
struct Particle {
  ParticleKind m_kind = ParticleKind::Smoke;
  float m_size = 0.0f;
  float m_x = 0.0f;
  float m_y = 0.0f;
  float m_z = 0.0f;
  float m_speed = 0.0f;
  std::array<float, 3> m_dir = {0.0f, 0.0f, 0.0f};
  float m_red = 0.0f;
  float m_green = 0.0f;
  float m_blue = 0.0f;
  std::int32_t m_active_frames = 0;
  std::int32_t m_current_frame = 0;
};

inline constexpr float c_particle_alpha = 0.4f;

// A new particle of kind at (x, y, z), heading along the unit vector
// (dir_x, dir_y, dir_z) at speed, living frames updates.
Particle makeParticle(
    ParticleKind kind,
    float x,
    float y,
    float z,
    float dir_x,
    float dir_y,
    float dir_z,
    float speed,
    std::int32_t frames);

// Advances particle by one frame; false once it has lived all its frames.
bool updateParticle(Particle& particle);

// ParticleGenerator: a fixed pool of particles. Each update() moves the
// emitter, advances every live particle (freeing those that finish), then
// spawns up to `spawn` new ones at the emitter in random directions
// (rand()), filling free slots from the start of the pool.
class ParticleEmitter {
public:
  static constexpr std::size_t c_capacity = 1000;

  // The second argument (the original's emission rate) is accepted for
  // its signature's sake but, as there, never read.
  ParticleEmitter(
      std::int32_t spawn,
      std::int32_t rate,
      std::int32_t speed,
      std::int32_t life,
      ParticleKind kind);

  void update(float x, float y, float z);
  // Frees every particle.
  void clear();

  // Every slot, live or free, in pool order (the original's draw order).
  const std::vector<std::optional<Particle>>& slots() const;

private:
  void addParticles();

  float m_x = 0.0f;
  float m_y = 0.0f;
  float m_z = 0.0f;
  std::int32_t m_particles_per_emission;
  std::int32_t m_emission_speed;
  std::int32_t m_emission_life;
  ParticleKind m_kind;
  std::vector<std::optional<Particle>> m_slots;
};

// One explosion: a sphere growing from nothing while its color steps
// through its weapon's four explosion colors and it fades out.
struct Explosion {
  float m_radius = 0.0f;
  // The game starts each explosion halfway through the first color band.
  float m_timer = 50.0f;
};

struct ExplosionFrame {
  // Which of the four explosion colors this frame is drawn in (0-3), or
  // -1 where the original set no color at all (75 <= timer < 100): the
  // previous one stays in effect.
  std::int32_t m_color_index;
  float m_alpha;
};

// Advances explosion by one frame (Explosion::draw()'s own step).
ExplosionFrame advanceExplosion(Explosion& explosion);

// The sphere's radius for an explosion of a weapon with blast radius
// weapon_radius (Weapon::getRadius()).
float explosionSphereRadius(
    const Explosion& explosion, std::int32_t weapon_radius);

}  // namespace vulkan_graphix::EffectSimulation

#endif  // VULKAN_GRAPHIX_EFFECTSIMULATION_H
