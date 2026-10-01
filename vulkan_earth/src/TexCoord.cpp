#include "vulkan_earth/TexCoord.h"
#include "vulkan_earth/MacroCrtdbg.h"

TexCoord::TexCoord() = default;

TexCoord::TexCoord(float s, float t) {
    texcoord_s = s;
    texcoord_t = t;
}

TexCoord::~TexCoord() = default;