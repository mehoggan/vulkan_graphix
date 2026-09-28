#include "vulkan_graphix/HellfireTank.h"

#include <glm/gtc/matrix_transform.hpp>

namespace vulkan_graphix::HellfireTank {

namespace {
// TankB's own constructor values (vulkan_earth/src/TankB.cpp).
constexpr float c_part_scale = 50.0f;
Math::Vec3<float> const c_body_offset(0.0f, 65.0f, 0.0f);
Math::Vec3<float> const c_head_offset(0.0f, 70.0f, 0.0f);
Math::Vec3<float> const c_turret_offset(0.0f, 0.0001f, -50.0001f);

Math::Vec3<float> rotateOffsetThroughBasis(Math::Mat4<float> const& basis,
                                           Math::Vec3<float> const& offset) {
    return Math::Vec3<float>(basis * Math::Vec4<float>(offset, 0.0f));
}
}  // namespace

Math::Mat4<float> const& getPartBasis() {
    static Math::Mat4<float> const basis(
            Math::Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f),
            Math::Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f),
            Math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f),
            Math::Vec4<float>(0.0f, 0.0f, 0.0f, 1.0f));
    return basis;
}

PartTranslations getPartTranslations(Math::Vec3<float> const& world_position) {
    // head/turret's basis is identical to body's in TankB (see
    // getPartBasis()), so this is the same fixed coordinate permutation at
    // every level, just chained through each parent's own translation.
    Math::Vec3<float> const body = world_position + c_body_offset;
    Math::Vec3<float> const head =
            body + rotateOffsetThroughBasis(getPartBasis(), c_head_offset);
    Math::Vec3<float> const turret =
            head + rotateOffsetThroughBasis(getPartBasis(), c_turret_offset);
    return PartTranslations{body, head, turret};
}

Math::Mat4<float> buildPartMatrix(Math::Vec3<float> const& translation) {
    return glm::translate(Math::Mat4<float>(1.0f), translation) *
           getPartBasis() *
           glm::scale(Math::Mat4<float>(1.0f),
                      Math::Vec3<float>(c_part_scale));
}

}  // namespace vulkan_graphix::HellfireTank
