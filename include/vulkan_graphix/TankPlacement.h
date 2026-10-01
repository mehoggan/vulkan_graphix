#ifndef VULKAN_GRAPHIX_TANK_PLACEMENT_H
#define VULKAN_GRAPHIX_TANK_PLACEMENT_H

// Placing a multi-part vulkan_earth tank in the world, ported from
// vulkan_earth's Tank::setTankPos() and Tank::initBody()/initHead()/
// initTurret()/initWheel() (vulkan_earth/src/Tank.cpp). Every tank model
// (TankA-H) shares this; HellfireTank (TankB's own data) is one instance
// of it. Pure math, no rendering coupling.

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::TankPlacement {

// Each part's offset from its parent: body from the tank's world
// position, head from the body, turret from the head.
struct PartOffsets {
    Math::Vec3<float> body;
    Math::Vec3<float> head;
    Math::Vec3<float> turret;
};

// World-space translations of a tank's three parts.
struct PartTranslations {
    Math::Vec3<float> body;
    Math::Vec3<float> head;
    Math::Vec3<float> turret;
};

// The right/up/at basis Tank::initBody()/initHead()/initTurret()/
// initWheel() give every part of an upright tank - not an identity
// rotation, a fixed axis permutation (model-local +X ends up along world
// +Z).
Math::Mat4<float> const& uprightPartBasis();

// Tank::setTankPos()'s hierarchical composition: the body sits at
// world_position + offsets.body, then a child part's offset is rotated
// through its parent's own basis columns (body_matrix's for the head,
// head_matrix's for the turret) before being added to the parent's
// translation - so the parts stay attached however the body is tilted or
// the head is turned. Only the matrices' basis columns are read. Sums in
// the original's order (translation + x*col0 + y*col1 + z*col2).
PartTranslations composePartTranslations(
        Math::Vec3<float> const& world_position,
        Math::Mat4<float> const& body_matrix,
        Math::Mat4<float> const& head_matrix,
        PartOffsets const& offsets);

}  // namespace vulkan_graphix::TankPlacement

#endif  // VULKAN_GRAPHIX_TANK_PLACEMENT_H
