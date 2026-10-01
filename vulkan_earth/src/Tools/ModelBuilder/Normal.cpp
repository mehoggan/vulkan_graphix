#include "vulkan_earth/Tools/ModelBuilder/Normal.h"

Normal::Normal() {}

Normal::Normal(float x, float y, float z) {
    this->compoX = x;
    this->compoY = y;
    this->compoZ = z;
}

Normal::~Normal() {}