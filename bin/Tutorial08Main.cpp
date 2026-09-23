#include "vulkan_graphix/Tutorial08.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial08>();

    // Window creation
    if (!window.create("08 - Phong Sphere")) {
        return -1;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial08> tutorial08 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial08>(tutorial);

    // Tutorial 08
    if (!tutorial08->createRenderingResources()) {
        return -1;
    }
    if (!tutorial08->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial08->createDepthResources()) {
        return -1;
    }
    if (!tutorial08->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial08->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial08->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial08->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial08->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial08->createRenderPass()) {
        return -1;
    }
    if (!tutorial08->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial08->createPipeline()) {
        return -1;
    }
    if (!tutorial08->createVertexBuffer()) {
        return -1;
    }
    if (!tutorial08->createIndexBuffer()) {
        return -1;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}
