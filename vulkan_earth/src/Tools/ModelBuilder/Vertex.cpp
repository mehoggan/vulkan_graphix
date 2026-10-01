#include "vulkan_earth/Tools/ModelBuilder/Vertex.h"

Vertex::Vertex() {}

Vertex::Vertex(float x, float y, float z) {
    this->coordX = x;
    this->coordY = y;
    this->coordZ = z;
}

Vertex::~Vertex() {}