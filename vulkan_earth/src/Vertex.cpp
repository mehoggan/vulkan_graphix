#include "Vertex.h"
#include "macro_crtdbg.h"

Vertex::Vertex() = default;

Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z) {
    this->coordX = x;
    this->coordY = y;
    this->coordZ = z;
}

Vertex::~Vertex() = default;