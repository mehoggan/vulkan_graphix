#include "Vector.h"
#include "macro_crtdbg.h"

Vector::Vector() {
	this->compoX = 0.0f;
	this->compoY = 0.0f;
	this->compoZ = 0.0f;
}

Vector::Vector(GLfloat compoX, GLfloat compoY, GLfloat compoZ) {
	this->compoX = compoX;
	this->compoY = compoY;
	this->compoZ = compoZ;
}

Vector::~Vector() {
}