#include "VBOQualifer.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
// #include "MacroCrtdbg.h"

VBOQualifer::VBOQualifer() {
    red_bits = 0;
    green_bits = 0;
    blue_bits = 0;
    alpha_bits = 0;
    depth_bits = 0;
    stencil_bits = 0;
    max_texture_size = 0;
    max_lights = 0;
    max_clip_planes = 0;
    max_model_view_stacks = 0;
    max_attrib_stacks = 0;
    max_texture_stacks = 0;
    qualified = true;
}

VBOQualifer::~VBOQualifer() = default;

bool VBOQualifer::getQualified() { return qualified; }

bool VBOQualifer::establishIfQualified() {
    const char* gl_string = nullptr;

    gl_string = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    if (gl_string)
        vendor = gl_string;
    else {
        qualified = false;
    }

    gl_string = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    if (gl_string)
        renderer = gl_string;
    else {
        qualified = false;
    }

    gl_string = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (gl_string)
        version = gl_string;
    else {
        qualified = false;
    }

    gl_string = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    if (!gl_string) {
        qualified = false;
        exit(0);
    }

    std::string extensions_str = gl_string;
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

    glGetIntegerv(GL_RED_BITS, &red_bits);
    glGetIntegerv(GL_GREEN_BITS, &green_bits);
    glGetIntegerv(GL_BLUE_BITS, &blue_bits);
    glGetIntegerv(GL_ALPHA_BITS, &alpha_bits);
    glGetIntegerv(GL_DEPTH_BITS, &depth_bits);
    glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
    glGetIntegerv(GL_MAX_LIGHTS, &max_lights);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    glGetIntegerv(GL_MAX_CLIP_PLANES, &max_clip_planes);
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &max_model_view_stacks);
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &max_projection_stacks);
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &max_attrib_stacks);
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &max_texture_stacks);

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