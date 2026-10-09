#ifndef VULKAN_GRAPHIX_TANKORIENTATION_H
#define VULKAN_GRAPHIX_TANKORIENTATION_H

// Tilting a tank to sit flush on the ground under it, ported from
// vulkan_earth's Tank::orientTank() and its calcAngleBetweenVectors()
// helper (which vulkan_earth also kept a second, identical copy of in
// GameState). Pure math - the original did the rotation through the GL
// matrix stack (glLoadMatrixf/glRotatef/glGetFloatv); this is the same
// rotation via glm, with no rendering coupling.

#include <optional>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::TankOrientation {

// Angle between two vectors, in degrees. Returns 0.01 when the two
// normalized vectors' dot product falls outside acos()'s [-1, 1] domain
// (e.g. float rounding on parallel vectors, or a zero-length input) - the
// value calcAngleBetweenVectors() returned on its own errno check.
float angleBetweenDegrees(
    const Math::Vec3<float>& one, const Math::Vec3<float>& two);

struct Alignment {
  // body_matrix rotated so its up axis (column 1) points along the
  // ground normal.
  Math::Mat4<float> m_matrix;
  // World-space unit axis the rotation turned about (up x normal), and
  // how far, in degrees - vulkan_earth keeps both for its own
  // orientation debug drawing.
  Math::Vec3<float> m_axis;
  float m_angle_degrees;
};

// Rotates body_matrix so its up axis lines up with ground_normal, or
// returns nullopt when the two are already parallel (no rotation axis).
// The rotation is applied in the body's local frame about (axis.z, axis.y,
// axis.x) - swizzled exactly as Tank::orientTank()'s glRotatef call did,
// which converts the world-space axis into the frame of vulkan_earth's
// tank basis (an x<->z axis swap, see HellfireTank::getPartBasis()).
std::optional<Alignment> alignToGround(
    const Math::Mat4<float>& body_matrix,
    const Math::Vec3<float>& ground_normal);

}  // namespace vulkan_graphix::TankOrientation

#endif  // VULKAN_GRAPHIX_TANKORIENTATION_H
