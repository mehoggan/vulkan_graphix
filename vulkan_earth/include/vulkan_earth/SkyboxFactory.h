#ifndef SKYBOX_FACTORY_H_
#define SKYBOX_FACTORY_H_

#include <cstdint>
#include <memory>
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"
#include "vulkan_graphix/Render/Texture.h"

namespace vulkan_graphix::Render {
class RenderContext;
}

class SkyboxFactory {
public:
    SkyboxFactory();
    SkyboxFactory(std::int32_t size_of_box);
    ~SkyboxFactory();
    void draw(vulkan_graphix::Render::RenderContext& context);

private:
    void buildGeometry();

    float size;
    std::shared_ptr<vulkan_graphix::Render::Texture> texture;
    vulkan_graphix::Render::UiMesh mesh;
};

#endif /* SKYBOX_FACTORY_H_ */