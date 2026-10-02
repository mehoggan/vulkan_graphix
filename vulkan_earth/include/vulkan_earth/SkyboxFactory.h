#ifndef SKYBOX_FACTORY_H_
#define SKYBOX_FACTORY_H_

#include <cstdint>
#include <memory>
#include "vulkan_earth/render/Mesh.h"
#include "vulkan_earth/render/Texture.h"

namespace vulkan_earth::render {
class RenderContext;
}

class SkyboxFactory {
public:
    SkyboxFactory();
    SkyboxFactory(std::int32_t size_of_box);
    ~SkyboxFactory();
    void draw(vulkan_earth::render::RenderContext& context);

private:
    void buildGeometry();

    float size;
    std::shared_ptr<vulkan_earth::render::Texture> texture;
    vulkan_earth::render::UiMesh mesh;
};

#endif /* SKYBOX_FACTORY_H_ */