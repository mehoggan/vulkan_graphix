#include "VBOQualifer.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
// #include "macro_crtdbg.h"

VBOQualifer::VBOQualifer() {
    redBits = 0;
    greenBits = 0;
    blueBits = 0;
    alphaBits = 0;
    depthBits = 0;
    stencilBits = 0;
    maxTextureSize = 0;
    maxLights = 0;
    maxClipPlanes = 0;
    maxModelViewStacks = 0;
    maxAttribStacks = 0;
    maxTextureStacks = 0;
    qualified = true;
}

VBOQualifer::~VBOQualifer() = default;

bool VBOQualifer::getQualified() { return qualified; }

bool VBOQualifer::establishIfQualified() {
    const char* str = nullptr;

    str = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    if (str)
        vendor = str;
    else {
        qualified = false;
    }

    str = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    if (str)
        renderer = str;
    else {
        qualified = false;
    }

    str = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (str)
        version = str;
    else {
        qualified = false;
    }

    str = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    if (!str) {
        qualified = false;
        exit(0);
    }

    std::string extensions_str = str;
    std::string current;
    for (char ch : extensions_str) {
        if (ch != ' ') {
            current += ch;
        } else if (!current.empty()) {
            extensions.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) {
        extensions.push_back(current);
    }
    extensions_supported = static_cast<int>(extensions.size());

    glGetIntegerv(GL_RED_BITS, &redBits);
    glGetIntegerv(GL_GREEN_BITS, &greenBits);
    glGetIntegerv(GL_BLUE_BITS, &blueBits);
    glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
    glGetIntegerv(GL_DEPTH_BITS, &depthBits);
    glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
    glGetIntegerv(GL_MAX_LIGHTS, &maxLights);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    glGetIntegerv(GL_MAX_CLIP_PLANES, &maxClipPlanes);
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &maxModelViewStacks);
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &maxProjectionStacks);
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &maxAttribStacks);
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &maxTextureStacks);

    return qualified;
}

bool VBOQualifer::isExtensionSupported(const std::string& exten) {
    bool extension_exists = false;
    for (int x = 0; x < extensions_supported; x++) {
        if (extensions[x] == exten) {
            extension_exists = true;
        }
    }
    return extension_exists;
}