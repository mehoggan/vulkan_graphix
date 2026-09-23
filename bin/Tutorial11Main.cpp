#include "vulkan_graphix/Tutorial11.h"
#include "vulkan_graphix/TutorialBase.h"

// Indexed, textured cube ported from vulkan_earth's SkyboxFactory geometry
// (see include/vulkan_graphix/Tutorial11.h) - drag with the mouse to orbit
// around/into it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial11>();

    if (!window.create("11 - Skybox")) {
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial11> tutorial11 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial11>(tutorial);

    if (!tutorial11->createRenderingResources()) {
        return -1;
    }
    if (!tutorial11->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial11->createTexture()) {
        return -1;
    }
    if (!tutorial11->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial11->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial11->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial11->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial11->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial11->createRenderPass()) {
        return -1;
    }
    if (!tutorial11->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial11->createPipeline()) {
        return -1;
    }
    if (!tutorial11->createVertexBuffer()) {
        return -1;
    }
    if (!tutorial11->createIndexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}
