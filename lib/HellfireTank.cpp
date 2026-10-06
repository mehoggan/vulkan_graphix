#include "vulkan_graphix/HellfireTank.h"

#include <glm/gtc/matrix_transform.hpp>

namespace vulkan_graphix::HellfireTank {

const TankPlacement::PartOffsets& getPartOffsets() {
    static const TankPlacement::PartOffsets offsets{
      Math::Vec3<float>(0.0f, 65.0f, 0.0f),
      Math::Vec3<float>(0.0f, 70.0f, 0.0f),
      Math::Vec3<float>(0.0f, 0.0001f, -50.0001f)};
    return offsets;
}

const Math::Mat4<float>& getPartBasis() {
    return TankPlacement::uprightPartBasis();
}

PartTranslations getPartTranslations(const Math::Vec3<float>& world_position) {
    return TankPlacement::composePartTranslations(
      world_position, getPartBasis(), getPartBasis(), getPartOffsets());
}

Math::Mat4<float> buildPartMatrix(const Math::Vec3<float>& translation) {
    return glm::translate(Math::Mat4<float>(1.0f), translation) *
      getPartBasis() *
      glm::scale(Math::Mat4<float>(1.0f), Math::Vec3<float>(c_part_scale));
}

}  // namespace vulkan_graphix::HellfireTank
