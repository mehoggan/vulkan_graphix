#include "vulkan_earth/Tools/ModelBuilder/Vertex.h"

Vertex::Vertex() {}

Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z) {
    this->coordX = x;
    this->coordY = y;
    this->coordZ = z;
}

Vertex::~Vertex() {}