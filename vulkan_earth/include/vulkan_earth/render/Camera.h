#ifndef VULKAN_EARTH_RENDER_CAMERA_H
#define VULKAN_EARTH_RENDER_CAMERA_H

// The fixed-function GL camera calls the game was written against,
// reproduced for Vulkan: the same matrices gluPerspective()/glOrtho()/
// gluLookAt() built, except that the projections map depth to Vulkan's
// [0, 1] (not GL's [-1, 1]) and flip y (Vulkan's clip space is y-down),
// so a scene drawn with them lands on the same pixels with the same depth
// ordering it did under GL.

#include "vulkan_earth/render/RenderTypes.h"

namespace vulkan_earth::render::camera {

// gluPerspective(field_of_view_degrees, aspect, near_plane, far_plane).
Mat4 perspective(float field_of_view_degrees,
                 float aspect,
                 float near_plane,
                 float far_plane);

// glOrtho(left, right, bottom, top, near_plane, far_plane).
Mat4 ortho(float left,
           float right,
           float bottom,
           float top,
           float near_plane,
           float far_plane);

// gluLookAt(eye, center, up_vector).
Mat4 lookAt(Vec3 const& eye, Vec3 const& center, Vec3 const& up_vector);

}  // namespace vulkan_earth::render::camera

#endif  // VULKAN_EARTH_RENDER_CAMERA_H
