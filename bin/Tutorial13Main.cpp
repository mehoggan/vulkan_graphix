#include "vulkan_graphix/Tutorial13.h"
#include "vulkan_graphix/TutorialBase.h"

// Real tank-shell mesh, parsed from vulkan_earth's own projectileDefault.ogl
// via the shared Tools::loadOglMeshData() (see include/vulkan_graphix/
// Tutorial13.h) - drag with the mouse to orbit around it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial13>();

    if (!window.create("13 - Projectile Model")) {
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial13> tutorial13 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial13>(tutorial);

    if (!tutorial13->createRenderingResources()) {
        return -1;
    }
    if (!tutorial13->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial13->createTexture()) {
        return -1;
    }
    if (!tutorial13->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial13->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial13->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial13->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial13->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial13->createRenderPass()) {
        return -1;
    }
    if (!tutorial13->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial13->createPipeline()) {
        return -1;
    }
    if (!tutorial13->createVertexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}
