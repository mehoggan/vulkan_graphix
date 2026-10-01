#include "vulkan_earth/Vertex.h"
#include "vulkan_earth/MacroCrtdbg.h"

Vertex::Vertex() = default;

Vertex::Vertex(float x, float y, float z) {
    coord_x = x;
    coord_y = y;
    coord_z = z;
}

Vertex::~Vertex() = default;