#include "Vertex.h"
#include "macro_crtdbg.h"

Vertex::Vertex() = default;

Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z) {
    coordX = x;
    coordY = y;
    coordZ = z;
}

Vertex::~Vertex() = default;