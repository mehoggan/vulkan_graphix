#include "vulkan_earth/Normal.h"
#include "vulkan_earth/MacroCrtdbg.h"

Normal::Normal() = default;

Normal::Normal(GLfloat x, GLfloat y, GLfloat z) {
    compo_x = x;
    compo_y = y;
    compo_z = z;
}

Normal::~Normal() = default;