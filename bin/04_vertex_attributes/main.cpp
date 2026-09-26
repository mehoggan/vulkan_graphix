#include <cstdlib>

#include "vulkan_graphix/Tutorial04.h"
#include "vulkan_graphix/TutorialBase.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
            std::make_shared<vulkan_graphix::Tutorial04>();

    // Window creation
    if (!window.create("04 - Vertex Attributes")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial->prepareVulkan(window.getParameters())) {
        return EXIT_FAILURE;
    }

    std::shared_ptr<vulkan_graphix::Tutorial04> tutorial04 =
            std::dynamic_pointer_cast<vulkan_graphix::Tutorial04>(tutorial);

    // Tutorial 04
    if (!tutorial04->createRenderPass()) {
        return EXIT_FAILURE;
    }
    if (!tutorial04->createPipeline()) {
        return EXIT_FAILURE;
    }
    if (!tutorial04->createVertexBuffer()) {
        return EXIT_FAILURE;
    }
    if (!tutorial04->createRenderingResources()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(*tutorial)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
