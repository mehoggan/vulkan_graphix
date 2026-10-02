#ifndef VULKAN_EARTH_RENDER_GL_MATRIX_H
#define VULKAN_EARTH_RENDER_GL_MATRIX_H

// The game keeps object transforms as column-major float[16] arrays and
// used the GL matrix stack purely as a calculator on them
// (glLoadMatrixf() -> glRotatef()/glTranslatef() -> glGetFloatv()). These
// are those same operations on the arrays directly, via glm: each
// right-multiplies, exactly as the GL calls did.

#include "vulkan_earth/render/RenderTypes.h"

namespace vulkan_earth::render::glmatrix {

Mat4 toMat4(float const* matrix);
void store(Mat4 const& source, float* matrix);

// matrix = matrix * rotation(degrees about axis); a zero axis leaves it
// unchanged (as Mesa's glRotatef() does).
void rotate(float* matrix, float degrees, float x, float y, float z);
// matrix = matrix * translation(x, y, z).
void translate(float* matrix, float x, float y, float z);

// The same operations on a Mat4, for building draw transforms.
Mat4 rotated(Mat4 const& matrix, float degrees, float x, float y, float z);
Mat4 translated(Mat4 const& matrix, float x, float y, float z);
Mat4 scaled(Mat4 const& matrix, float x, float y, float z);

}  // namespace vulkan_earth::render::glmatrix

#endif  // VULKAN_EARTH_RENDER_GL_MATRIX_H
