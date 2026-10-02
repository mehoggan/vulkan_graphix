#include "vulkan_earth/render/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace vulkan_earth::render::camera {

namespace {
Mat4 flipY(Mat4 projection) {
    projection[1][1] *= -1.0f;
    projection[2][1] *= -1.0f;
    projection[3][1] *= -1.0f;
    return projection;
}
}  // namespace

Mat4 perspective(float field_of_view_degrees,
                 float aspect,
                 float near_plane,
                 float far_plane) {
    return flipY(glm::perspectiveRH_ZO(glm::radians(field_of_view_degrees),
                                       aspect,
                                       near_plane,
                                       far_plane));
}

Mat4 ortho(float left,
           float right,
           float bottom,
           float top,
           float near_plane,
           float far_plane) {
    return flipY(
            glm::orthoRH_ZO(left, right, bottom, top, near_plane, far_plane));
}

Mat4 lookAt(Vec3 const& eye, Vec3 const& center, Vec3 const& up_vector) {
    return glm::lookAtRH(eye, center, up_vector);
}

}  // namespace vulkan_earth::render::camera
