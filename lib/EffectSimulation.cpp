#include "vulkan_graphix/EffectSimulation.h"

#include <cmath>
#include <cstdlib>

namespace vulkan_graphix::EffectSimulation {

Particle makeParticle(ParticleKind kind,
                      float x,
                      float y,
                      float z,
                      float dir_x,
                      float dir_y,
                      float dir_z,
                      float speed,
                      std::int32_t frames) {
    Particle particle;
    particle.kind = kind;
    particle.x = x;
    particle.y = y;
    particle.z = z;
    particle.dir = {dir_x, dir_y, dir_z};
    particle.speed = speed;
    particle.active_frames = frames;
    particle.current_frame = 0;
    switch (kind) {
        case ParticleKind::Smoke:
            particle.size = 2;
            particle.red = 1;
            particle.green = 1;
            particle.blue = 1;
            break;
        case ParticleKind::Acid:
            particle.size = 2;
            particle.red = 0;
            particle.green = 1;
            particle.blue = 0;
            break;
        case ParticleKind::Float:
            particle.size = 4;
            particle.red = 1;
            particle.green = 1;
            particle.blue = 1;
            break;
    }
    return particle;
}

bool updateParticle(Particle& particle) {
    switch (particle.kind) {
        case ParticleKind::Smoke:
            particle.x += particle.dir[0] * particle.speed;
            particle.y += 7 * (static_cast<float>(particle.current_frame) /
                               static_cast<float>(particle.active_frames));
            particle.z += particle.dir[2] * particle.speed;
            if (particle.current_frame <= 30) {
                particle.blue =
                        1 -
                        (static_cast<float>(particle.current_frame) / 30.0);
            } else if (particle.current_frame <= 60) {
                particle.green =
                        1 -
                        ((static_cast<float>(particle.current_frame) - 30.0) /
                         30.0);
            } else {
                // Measured from frame 30, not 60, as the game had it: red
                // drops straight to about 0.2 when this band starts.
                particle.red =
                        1 -
                        ((static_cast<float>(particle.current_frame) - 30.0) /
                         40.0);
            }
            break;
        case ParticleKind::Acid:
            particle.x += particle.dir[0] * particle.speed;
            particle.y += particle.dir[1] * particle.speed;
            particle.z += particle.dir[2] * particle.speed;
            break;
        case ParticleKind::Float:
            particle.x += particle.dir[0] * particle.speed;
            particle.y += particle.dir[1] * particle.speed / 10;
            particle.z += particle.dir[2] * particle.speed;
            break;
    }
    particle.current_frame++;
    return particle.current_frame != particle.active_frames;
}

ParticleEmitter::ParticleEmitter(std::int32_t spawn,
                                 std::int32_t /*rate*/,
                                 std::int32_t speed,
                                 std::int32_t life,
                                 ParticleKind kind)
        : m_particles_per_emission(spawn)
        , m_emission_speed(speed)
        , m_emission_life(life)
        , m_kind(kind)
        , m_slots(c_capacity) {}

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
        float dir_x = static_cast<float>(std::rand()) * 2 /
                              static_cast<float>(RAND_MAX) -
                      1;
        float dir_y = static_cast<float>(std::rand()) * 2 /
                              static_cast<float>(RAND_MAX) -
                      1;
        float dir_z = static_cast<float>(std::rand()) * 2 /
                              static_cast<float>(RAND_MAX) -
                      1;
        float mag = std::sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
        dir_x /= mag;
        dir_y /= mag;
        dir_z /= mag;
        m_slots[i] = makeParticle(m_kind,
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
    explosion.radius += .1;
    explosion.timer += .5;
    const float timer = explosion.timer;
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

float explosionSphereRadius(const Explosion& explosion,
                            std::int32_t weapon_radius) {
    // "ASSUMING SCALE ON TERRAIN IS 150 I NEED TO GET ACTUAL VALUE"
    return explosion.radius * (weapon_radius * 5.56 + 22.22);
}

}  // namespace vulkan_graphix::EffectSimulation
