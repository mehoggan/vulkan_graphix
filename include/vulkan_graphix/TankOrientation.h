#ifndef VULKAN_GRAPHIX_TANK_ORIENTATION_H
#define VULKAN_GRAPHIX_TANK_ORIENTATION_H

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
float angleBetweenDegrees(Math::Vec3<float> const& one,
                          Math::Vec3<float> const& two);

struct Alignment {
    // body_matrix rotated so its up axis (column 1) points along the
    // ground normal.
    Math::Mat4<float> matrix;
    // World-space unit axis the rotation turned about (up x normal), and
    // how far, in degrees - vulkan_earth keeps both for its own
    // orientation debug drawing.
    Math::Vec3<float> axis;
    float angle_degrees;
};

// Rotates body_matrix so its up axis lines up with ground_normal, or
// returns nullopt when the two are already parallel (no rotation axis).
// The rotation is applied in the body's local frame about (axis.z, axis.y,
// axis.x) - swizzled exactly as Tank::orientTank()'s glRotatef call did,
// which converts the world-space axis into the frame of vulkan_earth's
// tank basis (an x<->z axis swap, see HellfireTank::getPartBasis()).
std::optional<Alignment> alignToGround(Math::Mat4<float> const& body_matrix,
                                       Math::Vec3<float> const& ground_normal);

}  // namespace vulkan_graphix::TankOrientation

#endif  // VULKAN_GRAPHIX_TANK_ORIENTATION_H
