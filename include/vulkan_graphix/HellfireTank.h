#ifndef VULKAN_GRAPHIX_HELLFIRE_TANK_H
#define VULKAN_GRAPHIX_HELLFIRE_TANK_H

// Shared placement math for vulkan_earth's "Hellfire" tank (vulkan_earth/
// src/TankB.cpp), ported from Tank::initBody()/initHead()/initTurret() and
// Tank::setTankPos() (vulkan_earth/src/Tank.cpp:73-105). Lives here (not
// as a private helper on Tutorial16) so every tutorial that draws the tank
// - and any future Vulkan port of vulkan_earth's own Tank rendering - calls
// the same composition code instead of keeping separate copies. Pure math,
// no Vulkan/rendering coupling: a caller feeds buildPartMatrix()'s result
// to its own shader however it delivers a model matrix.
//
// TankB's own constructor values (body_offset/head_offset/turret_offset,
// a shared 50x scale) are taken verbatim rather than recomputed from live
// gameplay state - see Tutorial16.h's top comment for why. TankB also
// tracks a fourth "wheel" part, but never loads or draws one, so neither
// does this.

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::HellfireTank {

// World-space translations of the three parts of one tank, composed
// hierarchically body -> head -> turret.
struct PartTranslations {
    Math::Vec3<float> body;
    Math::Vec3<float> head;
    Math::Vec3<float> turret;
};

// Tank::initBody()/initHead()/initTurret() all set this identical right/
// up/at basis - not an identity rotation, a fixed axis permutation
// (model-local +X ends up along world +Z).
Math::Mat4<float> const& getPartBasis();

// Mirrors Tank::setTankPos(x, y, z)'s own construction exactly: e.g.
// head_matrix[12] = body_matrix[12] + head_offset[0]*body_matrix[0] +
// head_offset[1]*body_matrix[4] + head_offset[2]*body_matrix[8] (and the
// [13]/[14] analogues) - a child part's offset is rotated through its
// parent's own basis columns before being added to the parent's
// translation, not added directly. (Adding it directly is only a no-op for
// head_offset, whose x/z components are both zero, and silently wrong for
// turret_offset - the turret ends up beside the head, not in front of it.)
PartTranslations getPartTranslations(Math::Vec3<float> const& world_position);

// translate(translation) * getPartBasis() * scale(TankB's 50x part scale).
Math::Mat4<float> buildPartMatrix(Math::Vec3<float> const& translation);

}  // namespace vulkan_graphix::HellfireTank

#endif  // VULKAN_GRAPHIX_HELLFIRE_TANK_H
