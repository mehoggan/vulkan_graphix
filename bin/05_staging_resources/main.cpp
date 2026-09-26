#include <cstdlib>

#include "vulkan_graphix/Tutorial05.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial05>();

    // Window creation
    if (!window.create("05 - Staging Resources")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial05> tutorial05 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial05>(tutorial);

    // Tutorial 05
    if (!tutorial05->createRenderingResources()) {
        return EXIT_FAILURE;
    }
    if (!tutorial05->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial05->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial05->createVertexBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial05->createStagingBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial05->copyVertexData()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
