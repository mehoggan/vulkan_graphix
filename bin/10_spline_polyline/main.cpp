#include <cstdlib>

#include "vulkan_graphix/Tutorial10.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial10>();

    // Window creation
    if (!window.create("10 - Sampled Bezier Curve Polyline")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial10> tutorial10 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial10>(tutorial);

    // Tutorial 10
    if (!tutorial10->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createDepthResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createUniformBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createDescriptorSetLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createDescriptorPool()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->allocateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->updateDescriptorSet()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createPipelineLayout()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createLinePipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createCurveVertexBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial10->createControlPolygonVertexBuffer()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
