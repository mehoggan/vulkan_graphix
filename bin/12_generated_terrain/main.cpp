#include <cstdlib>

#include "Tutorial12.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"

// Generated terrain ported from vulkan_earth's TerrainMaker height-field
// algorithm (see include/vulkan_graphix/Tutorial/Tutorial12.h and
// TerrainGenerator.h) - drag with the mouse to orbit around it.
int main(int /*argc*/, char** /*argv*/) {
  vulkan_graphix::os::Window window;
  std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
      std::make_shared<vulkan_graphix::Tutorial12>();

  if (!window.create("12 - Generated Terrain")) {
    return EXIT_FAILURE;
  }

  if (!tutorial->prepareVulkan(window.getParameters())) {
    return EXIT_FAILURE;
  }

  std::shared_ptr<vulkan_graphix::Tutorial12> tutorial12 =
      std::dynamic_pointer_cast<vulkan_graphix::Tutorial12>(tutorial);

  if (!tutorial12->createResources()) {
    return EXIT_FAILURE;
  }

  if (!window.renderingLoop(*tutorial)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
