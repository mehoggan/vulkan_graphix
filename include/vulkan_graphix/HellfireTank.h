#ifndef VULKAN_GRAPHIX_HELLFIRETANK_H
#define VULKAN_GRAPHIX_HELLFIRETANK_H

// vulkan_earth's "Hellfire" tank (vulkan_earth/src/TankB.cpp): its own
// part offsets and scale - which TankB itself reads from here - plus
// convenience wrappers placing an untilted Hellfire via the shared
// TankPlacement composition (Tank::setTankPos()). Used by the tutorials
// that draw the tank (16/18/21/22). Pure math, no Vulkan/rendering
// coupling: a caller feeds buildPartMatrix()'s result to its own shader
// however it delivers a model matrix.
//
// TankB's own constructor values (body_offset/head_offset/turret_offset,
// a shared 50x scale) are taken verbatim rather than recomputed from live
// gameplay state - see Tutorial16.h's top comment for why. TankB also
// tracks a fourth "wheel" part, but never loads or draws one, so neither
// does this.

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/TankPlacement.h"

namespace vulkan_graphix::HellfireTank {

using PartTranslations = TankPlacement::PartTranslations;

// TankB's own constructor values (vulkan_earth/src/TankB.cpp now reads
// them from here): every part drawn at 50x scale.
inline constexpr float c_part_scale = 50.0f;

// TankB's body/head/turret offsets.
const TankPlacement::PartOffsets& getPartOffsets();

// TankPlacement::uprightPartBasis(), the basis every part of an untilted
// tank has.
const Math::Mat4<float>& getPartBasis();

// An untilted Hellfire at world_position:
// TankPlacement::composePartTranslations() with every part's basis upright.
PartTranslations getPartTranslations(const Math::Vec3<float>& world_position);

// translate(translation) * getPartBasis() * scale(c_part_scale).
Math::Mat4<float> buildPartMatrix(const Math::Vec3<float>& translation);

}  // namespace vulkan_graphix::HellfireTank

#endif  // VULKAN_GRAPHIX_HELLFIRETANK_H
