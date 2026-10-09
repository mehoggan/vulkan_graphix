#include <cstdlib>

#include "Tutorial21.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"

// Real vulkan_earth World/Camera integration: GameState::draw()'s own
// skybox -> terrain -> tank order, combining three previously-separate
// pilots (Tutorial11's skybox, Tutorial12's terrain, Tutorial16's tank)
// into one real scene, with the tank placed via a real
// TerrainGenerator::heightAt() query - see
// include/vulkan_graphix/Tutorial/Tutorial21.h. Drag with the mouse to orbit
// around the scene.
int main(int /*argc*/, char** /*argv*/) {
  vulkan_graphix::os::Window window;
  std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
      std::make_shared<vulkan_graphix::Tutorial21>();

  if (!window.create("21 - World")) {
    return EXIT_FAILURE;
  }

  if (!tutorial->prepareVulkan(window.getParameters())) {
    return EXIT_FAILURE;
  }

  std::shared_ptr<vulkan_graphix::Tutorial21> tutorial21 =
      std::dynamic_pointer_cast<vulkan_graphix::Tutorial21>(tutorial);

  if (!tutorial21->createResources()) {
    return EXIT_FAILURE;
  }

  if (!window.renderingLoop(*tutorial)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
