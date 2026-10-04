#ifndef VULKAN_GRAPHIX_BALLISTICS_H
#define VULKAN_GRAPHIX_BALLISTICS_H

// Shared projectile flight math, ported from vulkan_earth, where the same
// launch-from-barrel and flight formulas were written out separately in
// Projectile's constructor, GameState::handleProjectileState() (the shell
// actually in flight), GameState::constructProjectile(), and twice in
// Player (calculateProjectilePhysics()/displayProjectilePhysiscs(), the
// CPU player's shot simulation). This is the one copy all of those now
// call. Pure math, no rendering coupling.
//
// A barrel points down its turret matrix's -Z axis (column 2), from the
// turret's translation (column 3) - vulkan_earth's "THE COORD SYSTEM WE
// USE HAS X AND Z INVERSED" convention.

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Ballistics {

// Where a shell starts and how fast it's moving when it leaves the barrel.
struct Launch {
    Math::Vec3<float> m_origin;
    Math::Vec3<float> m_velocity;
};

// The point `distance` world units out along the barrel of turret_matrix:
// translation - distance * (column 2).
Math::Vec3<float> pointAlongBarrel(const Math::Mat4<float>& turret_matrix,
                                   float distance);

// A shell fired at `speed` from pointAlongBarrel(turret_matrix,
// muzzle_distance), travelling down the barrel.
Launch launchFromBarrel(const Math::Mat4<float>& turret_matrix,
                        float speed,
                        float muzzle_distance);

// Position `time` after launch under constant vertical gravity (negative
// pulls down): x/z move linearly, y follows y0 + vy*t + g*t^2/2.
Math::Vec3<float> positionAt(const Launch& launch, float gravity, float time);

}  // namespace vulkan_graphix::Ballistics

#endif  // VULKAN_GRAPHIX_BALLISTICS_H
