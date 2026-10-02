#include "vulkan_graphix/TankPlacement.h"

#include <cstdint>

namespace vulkan_graphix::TankPlacement {

namespace {
// parent + offset.x * col0 + offset.y * col1 + offset.z * col2, summed
// left to right exactly as Tank::setTankPos() wrote it out per component.
Math::Vec3<float> offsetThroughBasis(Math::Vec3<float> const& parent,
                                     Math::Mat4<float> const& basis,
                                     Math::Vec3<float> const& offset) {
    Math::Vec3<float> result;
    for (std::int32_t i = 0; i < 3; ++i) {
        result[i] = parent[i] + offset.x * basis[0][i] +
                    offset.y * basis[1][i] + offset.z * basis[2][i];
    }
    return result;
}
}  // namespace

Math::Mat4<float> const& uprightPartBasis() {
    static Math::Mat4<float> const basis(
            Math::Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f),
            Math::Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f),
            Math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f),
            Math::Vec4<float>(0.0f, 0.0f, 0.0f, 1.0f));
    return basis;
}

PartTranslations composePartTranslations(
        Math::Vec3<float> const& world_position,
        Math::Mat4<float> const& body_matrix,
        Math::Mat4<float> const& head_matrix,
        PartOffsets const& offsets) {
    PartTranslations translations;
    translations.body = world_position + offsets.body;
    translations.head =
            offsetThroughBasis(translations.body, body_matrix, offsets.head);
    translations.turret =
            offsetThroughBasis(translations.head, head_matrix, offsets.turret);
    return translations;
}

}  // namespace vulkan_graphix::TankPlacement
