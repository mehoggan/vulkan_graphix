#ifndef VULKAN_GRAPHIX_TANKPLACEMENT_H
#define VULKAN_GRAPHIX_TANKPLACEMENT_H

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
    Math::Vec3<float> m_body;
    Math::Vec3<float> m_head;
    Math::Vec3<float> m_turret;
};

// World-space translations of a tank's three parts.
struct PartTranslations {
    Math::Vec3<float> m_body;
    Math::Vec3<float> m_head;
    Math::Vec3<float> m_turret;
};

// The right/up/at basis Tank::initBody()/initHead()/initTurret()/
// initWheel() give every part of an upright tank - not an identity
// rotation, a fixed axis permutation (model-local +X ends up along world
// +Z).
const Math::Mat4<float>& uprightPartBasis();

// Tank::setTankPos()'s hierarchical composition: the body sits at
// world_position + offsets.body, then a child part's offset is rotated
// through its parent's own basis columns (body_matrix's for the head,
// head_matrix's for the turret) before being added to the parent's
// translation - so the parts stay attached however the body is tilted or
// the head is turned. Only the matrices' basis columns are read. Sums in
// the original's order (translation + x*col0 + y*col1 + z*col2).
PartTranslations composePartTranslations(
        const Math::Vec3<float>& world_position,
        const Math::Mat4<float>& body_matrix,
        const Math::Mat4<float>& head_matrix,
        const PartOffsets& offsets);

}  // namespace vulkan_graphix::TankPlacement

#endif  // VULKAN_GRAPHIX_TANKPLACEMENT_H
