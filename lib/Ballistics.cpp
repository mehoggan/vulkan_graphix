#include "vulkan_graphix/Ballistics.h"

#include <cmath>

namespace vulkan_graphix::Ballistics {

Math::Vec3<float> pointAlongBarrel(Math::Mat4<float> const& turret_matrix,
                                   float distance) {
    return Math::Vec3<float>(turret_matrix[3]) -
           distance * Math::Vec3<float>(turret_matrix[2]);
}

Launch launchFromBarrel(Math::Mat4<float> const& turret_matrix,
                        float speed,
                        float muzzle_distance) {
    return Launch{pointAlongBarrel(turret_matrix, muzzle_distance),
                  -Math::Vec3<float>(turret_matrix[2]) * speed};
}

Math::Vec3<float> positionAt(Launch const& launch, float gravity, float time) {
    return Math::Vec3<float>(
            launch.velocity.x * time + launch.origin.x,
            static_cast<float>(
                    0.5f * gravity * std::pow(static_cast<double>(time), 2.0) +
                    launch.velocity.y * time + launch.origin.y),
            launch.velocity.z * time + launch.origin.z);
}

}  // namespace vulkan_graphix::Ballistics
