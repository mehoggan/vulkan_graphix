#include "vulkan_graphix/Tutorial06.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial06>();

    // Window creation
    if (!window.create("06 - Descriptor Sets")) {
        return -1;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial06> tutorial06 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial06>(tutorial);

    // Tutorial 06
    if (!tutorial06->createRenderingResources()) {
        return -1;
    }
    if (!tutorial06->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial06->createTexture()) {
        return -1;
    }
    if (!tutorial06->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial06->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial06->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial06->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial06->createRenderPass()) {
        return -1;
    }
    if (!tutorial06->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial06->createPipeline()) {
        return -1;
    }
    if (!tutorial06->createVertexBuffer()) {
        return -1;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}
