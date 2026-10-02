#include "vulkan_graphix/HellfireTank.h"

#include <glm/gtc/matrix_transform.hpp>

namespace vulkan_graphix::HellfireTank {

TankPlacement::PartOffsets const& getPartOffsets() {
    static TankPlacement::PartOffsets const offsets{
            Math::Vec3<float>(0.0f, 65.0f, 0.0f),
            Math::Vec3<float>(0.0f, 70.0f, 0.0f),
            Math::Vec3<float>(0.0f, 0.0001f, -50.0001f)};
    return offsets;
}

Math::Mat4<float> const& getPartBasis() {
    return TankPlacement::uprightPartBasis();
}

PartTranslations getPartTranslations(Math::Vec3<float> const& world_position) {
    return TankPlacement::composePartTranslations(
            world_position, getPartBasis(), getPartBasis(), getPartOffsets());
}

Math::Mat4<float> buildPartMatrix(Math::Vec3<float> const& translation) {
    return glm::translate(Math::Mat4<float>(1.0f), translation) *
           getPartBasis() *
           glm::scale(Math::Mat4<float>(1.0f),
                      Math::Vec3<float>(c_part_scale));
}

}  // namespace vulkan_graphix::HellfireTank
