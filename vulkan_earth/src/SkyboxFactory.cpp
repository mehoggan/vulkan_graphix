#include "vulkan_earth/SkyboxFactory.h"
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include "vulkan_earth/render/Renderer.h"
#include "vulkan_earth/render/UiBuilders.h"
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_earth::render;

SkyboxFactory::SkyboxFactory() = default;

SkyboxFactory::SkyboxFactory(std::int32_t size_of_box) {
    size = size_of_box;
    // One 1024x1024 image on every face (the per-face images the original
    // had begun slicing out are all commented out there too); GL_LINEAR,
    // GL_CLAMP.
    texture = render::Renderer::instance().loadRawTexture(
            "SkyBox.raw", 1024, 1024, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
    if (!texture) {
        std::cerr << "ERROR: File Not Found" << std::endl;
        exit(0);
    }
    buildGeometry();
}

SkyboxFactory::~SkyboxFactory() = default;

void SkyboxFactory::buildGeometry() {
    std::int32_t start = -size / 2;
    std::int32_t bound = size / 2;
    std::int32_t scale = 100;
    mesh.clear();
    mesh.setTexture(texture.get());
    mesh.setReplaceTexEnv(true);
    // front, right, back, left, top, bottom (the box's top is at half
    // height)
    mesh.addTexturedQuad(
            {render::Vec3(start * scale, start * scale, start * scale),
             render::Vec3(bound * scale, start * scale, start * scale),
             render::Vec3(bound * scale, bound * scale / 2, start * scale),
             render::Vec3(start * scale, bound * scale / 2, start * scale)},
            {render::Vec2(0, 1),
             render::Vec2(0, 0),
             render::Vec2(1, 0),
             render::Vec2(1, 1)},
            render::Vec4(1.0f));
    mesh.addTexturedQuad(
            {render::Vec3(bound * scale, start * scale, start * scale),
             render::Vec3(bound * scale, start * scale, bound * scale),
             render::Vec3(bound * scale, bound * scale / 2, bound * scale),
             render::Vec3(bound * scale, bound * scale / 2, start * scale)},
            {render::Vec2(0, 1),
             render::Vec2(0, 0),
             render::Vec2(1, 0),
             render::Vec2(1, 1)},
            render::Vec4(1.0f));
    mesh.addTexturedQuad(
            {render::Vec3(bound * scale, start * scale, bound * scale),
             render::Vec3(start * scale, start * scale, bound * scale),
             render::Vec3(start * scale, bound * scale / 2, bound * scale),
             render::Vec3(bound * scale, bound * scale / 2, bound * scale)},
            {render::Vec2(0, 1),
             render::Vec2(0, 0),
             render::Vec2(1, 0),
             render::Vec2(1, 1)},
            render::Vec4(1.0f));
    mesh.addTexturedQuad(
            {render::Vec3(start * scale, start * scale, bound * scale),
             render::Vec3(start * scale, start * scale, start * scale),
             render::Vec3(start * scale, bound * scale / 2, start * scale),
             render::Vec3(start * scale, bound * scale / 2, bound * scale)},
            {render::Vec2(0, 1),
             render::Vec2(0, 0),
             render::Vec2(1, 0),
             render::Vec2(1, 1)},
            render::Vec4(1.0f));
    mesh.addTexturedQuad(
            {render::Vec3(start * scale, bound * scale / 2, start * scale),
             render::Vec3(bound * scale, bound * scale / 2, start * scale),
             render::Vec3(bound * scale, bound * scale / 2, bound * scale),
             render::Vec3(start * scale, bound * scale / 2, bound * scale)},
            {render::Vec2(0, 1),
             render::Vec2(0, 0),
             render::Vec2(1, 0),
             render::Vec2(1, 1)},
            render::Vec4(1.0f));
    mesh.addTexturedQuad(
            {render::Vec3(start * scale, start * scale, start * scale),
             render::Vec3(start * scale, start * scale, bound * scale),
             render::Vec3(bound * scale, start * scale, bound * scale),
             render::Vec3(bound * scale, start * scale, start * scale)},
            {render::Vec2(0, 1),
             render::Vec2(0, 0),
             render::Vec2(1, 0),
             render::Vec2(1, 1)},
            render::Vec4(1.0f));
}

void SkyboxFactory::draw(render::RenderContext& context) {
    context.draw(mesh);
}
