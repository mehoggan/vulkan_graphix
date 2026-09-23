#include "vulkan_graphix/Tutorial07.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial07>();

    // Window creation
    if (!window.create("07 - Uniform Buffers")) {
        return -1;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial07> tutorial07 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial07>(tutorial);

    // Tutorial 07
    if (!tutorial07->createRenderingResources()) {
        return -1;
    }
    if (!tutorial07->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial07->createTexture()) {
        return -1;
    }
    if (!tutorial07->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial07->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial07->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial07->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial07->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial07->createRenderPass()) {
        return -1;
    }
    if (!tutorial07->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial07->createPipeline()) {
        return -1;
    }
    if (!tutorial07->createVertexBuffer()) {
        return -1;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}
