#include "vulkan_graphix/TankPlacement.h"

#include <cstdint>

namespace vulkan_graphix::TankPlacement {

namespace {
// parent + offset.x * col0 + offset.y * col1 + offset.z * col2, summed
// left to right exactly as Tank::setTankPos() wrote it out per component.
Math::Vec3<float> offsetThroughBasis(const Math::Vec3<float>& parent,
                                     const Math::Mat4<float>& basis,
                                     const Math::Vec3<float>& offset) {
    Math::Vec3<float> result;
    for (std::int32_t i = 0; i < 3; ++i) {
        result[i] = parent[i] + offset.x * basis[0][i] +
                    offset.y * basis[1][i] + offset.z * basis[2][i];
    }
    return result;
}
}  // namespace

const Math::Mat4<float>& uprightPartBasis() {
    static const Math::Mat4<float> basis(
            Math::Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f),
            Math::Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f),
            Math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f),
            Math::Vec4<float>(0.0f, 0.0f, 0.0f, 1.0f));
    return basis;
}

PartTranslations composePartTranslations(
        const Math::Vec3<float>& world_position,
        const Math::Mat4<float>& body_matrix,
        const Math::Mat4<float>& head_matrix,
        const PartOffsets& offsets) {
    PartTranslations translations;
    translations.body = world_position + offsets.body;
    translations.head =
            offsetThroughBasis(translations.body, body_matrix, offsets.head);
    translations.turret =
            offsetThroughBasis(translations.head, head_matrix, offsets.turret);
    return translations;
}

}  // namespace vulkan_graphix::TankPlacement
