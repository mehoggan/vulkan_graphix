#ifndef VBO_QUALIFER_H
#define VBO_QUALIFER_H

#include <cstdint>
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
    std::int32_t extensions_supported;
    std::vector<std::string> extensions;
    std::int32_t red_bits;
    std::int32_t green_bits;
    std::int32_t blue_bits;
    std::int32_t alpha_bits;
    std::int32_t depth_bits;
    std::int32_t stencil_bits;
    std::int32_t max_texture_size;
    std::int32_t max_lights;
    std::int32_t max_attrib_stacks;
    std::int32_t max_model_view_stacks;
    std::int32_t max_projection_stacks;
    std::int32_t max_clip_planes;
    std::int32_t max_texture_stacks;
    bool qualified;
};

#endif  // VBO_QUALIFER_H