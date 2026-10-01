#include "vulkan_earth/Tools/ModelBuilder/TexCoord.h"

TexCoord::TexCoord() {}

TexCoord::TexCoord(float s, float t) {
    this->texcoordS = s;
    this->texcoordT = t;
}

TexCoord::~TexCoord() {}