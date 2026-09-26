# Copilot Instructions

## Project Overview

A collection of Vulkan API tutorials built as a shared library (`libvulkan_graphix.la`) plus individual tutorial runner binaries. Tutorials build progressively — each one adds Vulkan concepts on top of the previous.

## Build & Development

### Dependencies

```sh
sudo apt install -y libvulkan-dev vulkan-validationlayers spirv-tools
```

### Build

```sh
autoreconf -i   # only needed if build files are missing/stale
./configure
make -j8
```

Clean build:
```sh
make distclean && ./configure && make -j8
```

Generate `compile_commands.json` for IDE/tooling:
```sh
mkdir build && cd build
bear -- ../configure
bear -- make
```

### Running Tutorials

```sh
./build/bin/tutorial01_runner
./build/bin/tutorial02_runner
./build/bin/tutorial03_runner
```

### Compiling Shaders

```sh
source ./compile_shaders.sh <folder> <shader_name>
# Example: source ./compile_shaders.sh Tutorials/03 shader
```

Compiles `.vert` / `.frag` files to `.spv` in the `Data/` folder. Pre-compiled SPV files live in `resources/<NN>/Data/` and are copied into the build directory at link time.

## Architecture

### Library + Binary Split

All Vulkan logic lives in `lib/` and is compiled into `libvulkan_graphix.la`. Each `bin/tutorial0X_main.cpp` is a thin entry point that constructs a window (`os::Window`), a tutorial instance, calls `prepareVulkan()`, and runs `window.renderingLoop(tutorial)`.

### Tutorial Class Pattern

Each tutorial follows the same structure:
1. A `VulkanTutorial0XParameters` value-type class holding raw Vulkan handles (e.g., `VkInstance`, `VkDevice`, swapchain).
2. A `Tutorial0X` class inheriting from both `os::ProjectBase` and `LoggedClass<Tutorial0X>` (CRTP).
3. `Tutorial0X::prepareVulkan()` calls a private method chain in sequence (e.g., `loadVulkanLibrary()` → `loadExportedEntryPoints()` → `createInstance()` → `createDevice()` → ...).
4. `draw()` and `onWindowSizeChanged()` are the virtual rendering callbacks invoked by the window loop.

Tutorials are additive — Tutorial02 adds swapchain, Tutorial03 adds pipeline and render pass, etc.

### Vulkan Function Loading via Macros

Vulkan functions are **dynamically loaded** (not linked against a loader). `ListOfFunctions.inl` declares all functions categorized as:

- `VK_EXPORTED_FUNCTION` — entry points exported by the Vulkan `.so`
- `VK_GLOBAL_LEVEL_FUNCTION` — instance-creation functions
- `VK_INSTANCE_LEVEL_FUNCTION` — physical/logical device queries
- `VK_DEVICE_LEVEL_FUNCTION` — draw-time operations

`VulkanFunctions.h` `#include`s this `.inl` file inside macro definitions (e.g., `#define VK_DEVICE_LEVEL_FUNCTION(fun) extern PFN_##fun fun;`). When adding a new Vulkan call, add it to `ListOfFunctions.inl` under the appropriate category and tutorial comment, then load it in `VulkanFunctions.cpp`.

Swapchain functions are conditionally compiled under `#if defined(USE_SWAPCHAIN_EXTENSIONS)`.

### Logging

Classes gain per-instance logging by inheriting `LoggedClass<DerivedType>` (CRTP). The constructor auto-registers stdout/stderr sinks. Use the `LOG_TAG` protected member for log statements. Do **not** use raw `std::cout`/`std::cerr` — route all output through the logging API.

### Platform Abstraction

`OperatingSystem.h` / `OperatingSystem.cpp` wrap X11 (Xlib) for window creation and the `dlopen`/`dlsym` Vulkan library handle. `os::ProjectBase` is the abstract base for tutorial classes; `os::Window` drives the render loop.

### Adding a New Tutorial

1. Create `lib/Tutorial0X.cpp` and `include/vulkan_graphix/Tutorial0X.h`.
2. Add `Tutorial0X.cpp` to `lib/Makefile.am` under `libvulkan_graphix_la_SOURCES`.
3. Create `bin/tutorial0X_main.cpp` and add it to `bin/Makefile.am` following the existing pattern.
4. Add any new Vulkan functions to `ListOfFunctions.inl` with a `// Tutorial 0X` comment.
5. Place GLSL shaders in `Tutorials/0X/Data/`; compiled SPV goes in `resources/0X/Data/`.
6. Add SPV copy rules to `bin/Makefile.am` (`all-local` target and `CLEANFILES`).

## Code Style

- **Formatting**: clang-format (Google-based, 79-column C++ limit). Use `git clang-format` on staged changes.
- **Naming**: functions `camelBack`, variables `lower_case`, minimum variable name length 4.
- **Line length**: 80 characters (CPPLINT).
- **Pointer alignment**: left (`int* ptr`, not `int *ptr`).
- **Build flags**: `-Werror -Wall -pedantic` — all warnings are errors.
- **C++ standard**: C++20.
- **No `using namespace`** in headers.
- Header guards use the `#ifndef VULKAN_GRAPHIX_<FILENAME>_H` pattern.
