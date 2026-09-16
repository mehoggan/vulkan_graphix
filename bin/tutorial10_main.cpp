#include "vulkan_graphix/Tutorial10.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial10>();

    // Window creation
    if (!window.create("10 - Phong Tube Along a Sampled Curve")) {
        return -1;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return -1;
    }

    std::shared_ptr<vulkan_graphix::Tutorial10> tutorial10 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial10>(tutorial);

    // Tutorial 10
    if (!tutorial10->createRenderingResources()) {
        return -1;
    }
    if (!tutorial10->createStagingBuffer()) {
        return -1;
    }
    if (!tutorial10->createDepthResources()) {
        return -1;
    }
    if (!tutorial10->createUniformBuffer()) {
        return -1;
    }
    if (!tutorial10->createDescriptorSetLayout()) {
        return -1;
    }
    if (!tutorial10->createDescriptorPool()) {
        return -1;
    }
    if (!tutorial10->allocateDescriptorSet()) {
        return -1;
    }
    if (!tutorial10->updateDescriptorSet()) {
        return -1;
    }
    if (!tutorial10->createRenderPass()) {
        return -1;
    }
    if (!tutorial10->createPipelineLayout()) {
        return -1;
    }
    if (!tutorial10->createTubePipeline()) {
        return -1;
    }
    if (!tutorial10->createLinePipeline()) {
        return -1;
    }
    if (!tutorial10->createTubeVertexBuffer()) {
        return -1;
    }
    if (!tutorial10->createTubeIndexBuffer()) {
        return -1;
    }
    if (!tutorial10->createLineVertexBuffer()) {
        return -1;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return -1;
    }

    return 0;
}
