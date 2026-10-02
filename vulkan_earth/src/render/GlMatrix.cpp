#include "vulkan_earth/render/GlMatrix.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vulkan_earth::render::glmatrix {

Mat4 toMat4(float const* matrix) { return glm::make_mat4(matrix); }

void store(Mat4 const& source, float* matrix) {
    float const* values = glm::value_ptr(source);
    for (std::int32_t i = 0; i < 16; ++i) {
        matrix[i] = values[i];
    }
}

Mat4 rotated(Mat4 const& matrix, float degrees, float x, float y, float z) {
    if (x == 0.0f && y == 0.0f && z == 0.0f) {
        return matrix;
    }
    return glm::rotate(matrix, glm::radians(degrees), Vec3(x, y, z));
}

Mat4 translated(Mat4 const& matrix, float x, float y, float z) {
    return glm::translate(matrix, Vec3(x, y, z));
}

Mat4 scaled(Mat4 const& matrix, float x, float y, float z) {
    return glm::scale(matrix, Vec3(x, y, z));
}

void rotate(float* matrix, float degrees, float x, float y, float z) {
    store(rotated(toMat4(matrix), degrees, x, y, z), matrix);
}

void translate(float* matrix, float x, float y, float z) {
    store(translated(toMat4(matrix), x, y, z), matrix);
}

}  // namespace vulkan_earth::render::glmatrix
