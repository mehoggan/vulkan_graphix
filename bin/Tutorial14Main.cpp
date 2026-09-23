#include "vulkan_graphix/Tutorial14.h"
#include "vulkan_graphix/TutorialBase.h"

// Translucent particle-effect sphere ported from vulkan_earth's
// Particle::render() (see include/vulkan_graphix/Tutorial14.h) - drag with
// the mouse to orbit around it.
int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial14>();

    if (!window.create("14 - Particle Sphere")) {
        return -1;
    }

    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial14> tutorial14 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial14>(tutorial);

    if (!tutorial14->createRenderingResources()) {
        return -1;
    }
    if (!tutorial14->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial14->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial14->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial14->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial14->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial14->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial14->createRenderPass()) {
        return -1;
    }
    if (!tutorial14->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial14->createPipeline()) {
        return -1;
    }
    if (!tutorial14->createVertexBuffer()) {
        return -1;
    }
    if (!tutorial14->createIndexBuffer()) {
        return -1;
    }

    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}
