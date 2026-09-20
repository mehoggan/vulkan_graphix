#include "VBOQualifer.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
// #include "macro_crtdbg.h"

VBOQualifer::VBOQualifer() {
    this->redBits = 0;
    this->greenBits = 0;
    this->blueBits = 0;
    this->alphaBits = 0;
    this->depthBits = 0;
    this->stencilBits = 0;
    this->maxTextureSize = 0;
    this->maxLights = 0;
    this->maxClipPlanes = 0;
    this->maxModelViewStacks = 0;
    this->maxAttribStacks = 0;
    this->maxTextureStacks = 0;
    this->qualified = true;
}

VBOQualifer::~VBOQualifer() = default;

bool VBOQualifer::getQualified() { return this->qualified; }

bool VBOQualifer::establishIfQualified() {
    const char* str = nullptr;

    str = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    if (str)
        this->vendor = str;
    else {
        this->qualified = false;
    }

    str = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    if (str)
        this->renderer = str;
    else {
        this->qualified = false;
    }

    str = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (str)
        this->version = str;
    else {
        this->qualified = false;
    }

    str = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    if (!str) {
        this->qualified = false;
        exit(0);
    }

    std::string extensions_str = str;
    std::string current;
    for (char ch : extensions_str) {
        if (ch != ' ') {
            current += ch;
        } else if (!current.empty()) {
            this->extensions.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) {
        this->extensions.push_back(current);
    }
    this->extensions_supported = static_cast<int>(this->extensions.size());

    glGetIntegerv(GL_RED_BITS, &this->redBits);
    glGetIntegerv(GL_GREEN_BITS, &this->greenBits);
    glGetIntegerv(GL_BLUE_BITS, &this->blueBits);
    glGetIntegerv(GL_ALPHA_BITS, &this->alphaBits);
    glGetIntegerv(GL_DEPTH_BITS, &this->depthBits);
    glGetIntegerv(GL_STENCIL_BITS, &this->stencilBits);
    glGetIntegerv(GL_MAX_LIGHTS, &this->maxLights);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->maxTextureSize);
    glGetIntegerv(GL_MAX_CLIP_PLANES, &this->maxClipPlanes);
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &this->maxModelViewStacks);
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &this->maxProjectionStacks);
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &this->maxAttribStacks);
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &this->maxTextureStacks);

    return this->qualified;
}

bool VBOQualifer::isExtensionSupported(const std::string& exten) {
    bool extension_exists = false;
    for (int x = 0; x < this->extensions_supported; x++) {
        if (this->extensions[x] == exten) {
            extension_exists = true;
        }
    }
    return extension_exists;
}