#include <cstdlib>

#include "vulkan_graphix/Tutorial03.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial03>();

    // Window creation
    if (!window.create("03 - First Triangle")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial03> tutorial03 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial03>(tutorial);

    // Tutorial 03
    if (!tutorial03->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial03->createFramebuffers()) {
        return EXIT_FAILURE;
    }
    if (!tutorial03->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial03->createSemaphores()) {
        return EXIT_FAILURE;
    }
    if (!tutorial03->createCommandBuffers()) {
        return EXIT_FAILURE;
    }
    if (!tutorial03->recordCommandBuffers()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
