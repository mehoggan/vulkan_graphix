#include <cstdlib>

#include "vulkan_graphix/Tutorial01.h"

int main(int /*argc*/, char** /*argv*/) {
    vulkan_graphix::os::Window window;
    vulkan_graphix::Tutorial01 tutorial01(true);

    // Window creation
    if (!window.create("01 - The Beginning")) {
        return EXIT_FAILURE;
    }

    // Vulkan preparations and initialization
    if (!tutorial01.prepareVulkan()) {
        return EXIT_FAILURE;
    }

    // Rendering loop
    if (!window.renderingLoop(tutorial01)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
