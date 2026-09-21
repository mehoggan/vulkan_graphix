#ifndef VBO_QUALIFER_H
#define VBO_QUALIFER_H

#include <string>
#include <vector>

class VBOQualifer {
public:
    VBOQualifer();
    ~VBOQualifer();
    bool getQualified();
    bool establishIfQualified();
    bool isExtensionSupported(const std::string& exten);

private:
    std::string vendor;
    std::string renderer;
    std::string version;
    int extensions_supported;
    std::vector<std::string> extensions;
    int red_bits;
    int green_bits;
    int blue_bits;
    int alpha_bits;
    int depth_bits;
    int stencil_bits;
    int max_texture_size;
    int max_lights;
    int max_attrib_stacks;
    int max_model_view_stacks;
    int max_projection_stacks;
    int max_clip_planes;
    int max_texture_stacks;
    bool qualified;
};

#endif  // VBO_QUALIFER_H