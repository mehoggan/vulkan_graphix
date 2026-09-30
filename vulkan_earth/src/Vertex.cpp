#include "vulkan_earth/Vertex.h"
#include "vulkan_earth/MacroCrtdbg.h"

Vertex::Vertex() = default;

Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z) {
    coord_x = x;
    coord_y = y;
    coord_z = z;
}

Vertex::~Vertex() = default;