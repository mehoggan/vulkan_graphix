#include <cstdlib>

#include "Tutorial22.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"

// Real vulkan_earth GameState menu layer: the identical 5-quad bevel-
// panel background every real menu screen (MainMenu/ReadyMenu/ShopMenu/
// SubMenu*) opens with, plus ReadyMenu's own real technique - a live
// rotating 3D tank preview rendered into a scissored sub-region of the
// screen - see include/vulkan_graphix/Tutorial/Tutorial22.h. Click the button
// to see the click counter update.
int main(int /*argc*/, char** /*argv*/) {
  vulkan_graphix::os::Window window;
  std::shared_ptr<vulkan_graphix::TutorialBase> tutorial =
      std::make_shared<vulkan_graphix::Tutorial22>();

  if (!window.create("22 - GameState Menu")) {
    return EXIT_FAILURE;
  }

  if (!tutorial->prepareVulkan(window.getParameters())) {
    return EXIT_FAILURE;
  }

  std::shared_ptr<vulkan_graphix::Tutorial22> tutorial22 =
      std::dynamic_pointer_cast<vulkan_graphix::Tutorial22>(tutorial);

  if (!tutorial22->createResources()) {
    return EXIT_FAILURE;
  }

  if (!window.renderingLoop(*tutorial)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
