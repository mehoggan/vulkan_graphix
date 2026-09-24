# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A collection of Vulkan API tutorials and examples, demonstrating progressive concepts from device initialization through rendering. The codebase uses autotools build system with a shared library (`libvulkan_graphix.la`) containing common Vulkan utilities and individual tutorial implementations.

## Build & Development

### Initial Setup

Install dependencies:
```sh
sudo apt install -y libvulkan-dev vulkan-validationlayers spirv-tools
sudo apt install -y libboost-log-dev libboost-system-dev libboost-thread-dev
sudo apt install -y fonts-dejavu-core
```

Generate build files (if needed):
```sh
autoreconf -i
```

### Building

Configure and build the project:
```sh
./configure
make -j8
```

Build output binaries:
- `./build/bin/tutorial01_runner` through `./build/bin/tutorial18_runner`

### Development Workflow

Clean build:
```sh
make distclean
./configure
make -j8
```

Build and generate compile_commands.json for IDE integration:
```sh
mkdir build && cd build
bear -- ../configure
bear -- make
```

Code coverage (`gcovr` or `lcov`/`genhtml` required — see README.md):
```sh
./configure --enable-coverage
make -j8
make coverage          # HTML report at coverage-html/index.html
make coverage-missing  # prints uncovered line numbers per file, no HTML
```
Only `lib/` and `tests/` are instrumented. All ten tutorials get a real
integration test (drives each through a live Vulkan device/X11 window -
skipped automatically when `DISPLAY` isn't set, e.g. headless CI), and
`OperatingSystem.cpp`'s X11 event loop, `Logging`/`LoggerHelpers`, and
`Tools` each have their own direct unit tests. `TutorialBase` and every
tutorial's own `create*()` Vulkan-call failure branches (`if (result !=
VK_SUCCESS) return false;`) are covered by a fault-injection layer: since
every Vulkan call here goes through a mutable `vulkan_graphix::vkSomething`
function-pointer global (see `VulkanFunctions.h`/`ListOfFunctions.inl`),
a test can bring a tutorial up through a real `prepareVulkan()`, swap one
of those pointers for a fake that returns a failure code, call the one
step under test, and assert it fails - see
`tests/Tutorial04FaultInjectionTest.cpp` and
`tests/TutorialBaseFaultInjectionCommon.h`'s header comments for the
technique and its one-real-device-per-process constraint. The WSI
surface/swapchain entry points (`vkCreateXlibSurfaceKHR`,
`vkCreateSwapchainKHR`, etc.) aren't routed through that seam - they
resolve to real `libvulkan.so` symbols instead - so `createPresentation
Surface()`'s and `createSwapChain()`'s own failure branches stay out of
reach without LD_PRELOAD interposition. Overall `lib/`+`include/` line
coverage is ~71%; the remaining gap is mostly those swapchain/surface
branches, plus a few `checkPhysicalDeviceProperties()` branches (e.g. "no
queue family with the required properties") that would need a fake
device's reported properties, not just a failure code, to reach.

### Compiling Shaders

Shaders are compiled to SPIR-V bytecode using glslangValidator:
```sh
source ./compile_shaders.sh <folder> <shader_name>
# Example: source ./compile_shaders.sh resources/03 shader
```

This compiles `.vert` and `.frag` files to `.spv` format in the Data folder.
`glslc` (from the `shaderc` project) works as a drop-in alternative when
`glslangValidator` isn't installed - see README.md's "Generating Shader
Files" section for the exact invocation.

## Code Structure

### Directory Layout

- **lib/**: Core library source files
  - `VulkanCommon.cpp/.h` - Shared Vulkan utilities and helpers
  - `Tutorial01-18.cpp/.h` - Individual tutorial implementations
  - `OrbitCamera.cpp/.h` - Mouse-orbit camera, shared by Tutorial09/10-14
  - `TerrainGenerator.cpp/.h` - Diamond-square height-field generator
    shared by Tutorial12 and (eventually) a real vulkan_earth port
  - `BitmapFont.cpp/.h` - Bakes a TrueType font into a glyph atlas via
    the vendored `STBTrueType.h` (kept fully behind std types - no
    `stbtt_*` symbol is reachable outside `BitmapFont.cpp`), shared by
    Tutorial15 and any future vulkan_earth UI port
  - `UiGeometry.cpp/.h` - Beveled 2D button-quad geometry, ported from
    vulkan_earth's `ControlItemButton::draw()`, shared the same way
  - `Logging.cpp/.h` - Boost-based logging framework
  - `LoggerHelpers.cpp/.h`, `LoggedClass.hpp` - Logging infrastructure
  - `Tools.cpp/.h` - Utility functions (also holds `loadOglMeshData()`,
    a shared parser for vulkan_earth's `.ogl` mesh format used by
    Tutorial13 and any future vulkan_earth Vulkan port)
  - `OperatingSystem.cpp/.h` - Platform-specific abstractions
  - `VulkanFunctions.cpp/.h` - Vulkan function wrappers

- **bin/**: Tutorial executable entry points
  - One `TutorialNNMain.cpp` per active tutorial (01-18)

- **include/vulkan_graphix/**: Public headers
  - `ListOfFunctions.inl` - Pre-defined Vulkan function list
  - `STBImage.h` - Vendored single-header image loading library
  - `STBTrueType.h` - Vendored single-header TrueType font rasterizer,
    used only by `lib/BitmapFont.cpp` (see above)
  - `vk_platform.h` - Platform-specific Vulkan definitions

- **resources/NN/Data/** - Each tutorial's own GLSL sources
  (`shader.NN.{vert,frag}`), compiled SPIR-V (`shader.{vert,frag}.NN.spv`
  + `.spv.txt` disassembly), and any texture/mesh assets it needs

### Dependencies

- **Vulkan SDK** - GPU API bindings
- **X11** - Display server protocol (X11 platform)
- **Boost** - Logging (boost_log, boost_system, boost_thread)
- **libm** - Math library (C standard)
- **C++20** - Modern C++ standard

## Code Style & Quality

### Clang-Tidy

Enabled checks focus on:
- Bug detection (argument comments, uninitialized constructors, use-after-move,
  dangling handles, swapped arguments, macro parentheses, unused return
  values, throw-by-value/catch-by-reference)
- Modern C++ practices (nullptr, override, move constructors, `using` over
  `typedef`, `= default`/`= delete`, emplace over insert)
- Performance (range-based for loops, unnecessary copies, inefficient vector
  operations, non-noexcept move constructors)
- Readability (const return types, identifier naming/length, container
  `empty()` over `size() == 0`, redundant string `c_str()`, C-style casts)

Not enabled: `cppcoreguidelines-*` cast/ownership checks, `modernize-loop-convert`,
and `modernize-pass-by-value` — the Vulkan C API this project wraps leans on
`reinterpret_cast`, index-based loops, and out-params in ways those checks
flag without a real bug being present.

Key naming conventions:
- Functions: `camelBack`
- Variables, members, and parameters: `lower_case`
- Minimum variable name length: 4 characters, with an
  `IgnoredVariableNames`/`IgnoredParameterNames` exemption list
  (`.clang-tidy`) covering the vocabulary that's genuinely
  domain-standard rather than actually unclear: coordinate/color/
  texture components (`x`/`y`/`z`/`w`, `r`/`g`/`b`/`a`, `dx`/`dy`/`dz`,
  `nx`/`ny`/`nz`, `s`/`t`/`u`/`v`, and numbered or axis-suffixed
  variants like `v0`, `n_x`, `x1`) that match the Vulkan/OpenGL APIs
  this project calls into directly, plus `mag`(nitude), `id`, and a
  handful of common single-letter setter parameters (`p`, `d`, `h`,
  `n`, ...) where the enclosing `setFoo(T p)`-style signature already
  states what the value is — exempted the same way loop counters
  (`i`/`j`/`k`) already are by clang-tidy's own default. Anything not
  covered by that list still needs a real name.
- Short namespace length: 40 lines

### CPPLINT

Configuration in `CPPLINT.cfg`:
- Line length: 80 characters
- Filters applied for C++20 compatibility and project-specific style
- Minimal whitespace/indent enforcement

### Clang-Format

Standard formatting applied (`.clang-format` present).

## Architecture Notes

### Logging System

The project uses a custom logging infrastructure built on Boost.Log:
- `LoggedClass` base class provides logging capabilities
- `Logging.h` contains the logging configuration
- `LoggerHelpers` provide utility functions for formatted output

### Vulkan Utilities

`VulkanCommon.h/cpp` contains abstractions for:
- Device creation and initialization
- Memory allocation and buffer management
- Command buffer recording
- Presentation and synchronization

Tutorial classes inherit patterns from Tutorial01, building incrementally:
- Tutorial01: Basic device initialization
- Tutorial02: Rendering pipeline setup
- Tutorial03 - Tutorial09: Progressive feature additions (vertex/index/
  uniform buffers, textures, depth testing, Phong lighting, mouse-orbit
  camera, tessellated terrain)
- Tutorial10: a real polyline through a `Math::CubicCurve`-sampled
  spline (Catmull-Rom), drawn alongside its control polygon as a
  second, thinner line strip — first tutorial with push constants,
  line topology, and `VK_DYNAMIC_STATE_LINE_WIDTH` (curve thicker
  than control polygon, gated on the device's `wideLines` feature)
- Tutorial11 - Tutorial14: ported from the `vulkan_earth/` OpenGL->Vulkan
  migration's standalone pilots (see that directory's own git history) -
  an indexed textured skybox cube, a diamond-square-generated terrain
  (height-field generation shared via `TerrainGenerator`, see above), the
  real tank-shell mesh parsed from a `.ogl` file (parser shared via
  `Tools::loadOglMeshData()`), and a translucent alpha-blended particle
  sphere (`Math::Sphere`, same as Tutorial08's)
- Tutorial15: a text title and one clickable button with a text label -
  the two primitives vulkan_earth's whole UI (`ControlItem*`/`MainMenu`/
  `SubMenu*`/`ReadyMenu`/`ShopMenu`) is built from, without porting that
  entire class hierarchy. Text comes from `BitmapFont` (a TrueType glyph
  atlas baked via the vendored `STBTrueType.h`, replacing vulkan_earth's
  `glutBitmapCharacter()` calls, which have no Vulkan equivalent); the
  button's raised/pressed bevel comes from `UiGeometry::buildButtonBevel()`
  (ported from `ControlItemButton::draw()`). First tutorial to use
  `Tools::getOrthographicProjectionMatrix()` for real UI layout (Tutorial07
  already used it for one static full-screen quad) and the first with a
  host-visible vertex buffer rewritten every frame from CPU-side UI state,
  rather than a device-local one uploaded once
- Tutorial16: the real "Hellfire" tank (`vulkan_earth/src/TankB.cpp`)
  assembled from its three independently-positioned mesh parts (body/
  head/turret - `Tank`/`TankB` track a fourth "wheel" part, but `TankB`
  never loads or draws one, so this tutorial doesn't either), each parsed
  via `Tools::loadOglMeshData()` and sharing one texture, viewed via
  `OrbitCamera`. Each part's model matrix (TankB's own offset/scale/basis
  data, taken verbatim - see the tutorial's own header comment) is
  delivered as a vertex-stage push constant set immediately before that
  part's draw call, the first push constant here carrying a matrix rather
  than Tutorial10's flat color
- Tutorial17: a titled bevel-panel grid of all 8 real vulkan_earth items
  (`vulkan_earth/src/ItemXxx.cpp` - `Item` itself has no `draw()` method,
  so there was nothing to port there beyond this real data), ported from
  `Inventory`'s own rendering (`ControlItemGrid`'s bevel, built the same
  way as Tutorial15's button). Two textures - the `BitmapFont` glyph atlas
  and a combined icon atlas stitched at runtime from the 8 real
  `ItemXxx.raw` assets (tutorial-local plumbing, not a shared component) -
  stay behind the established one-texture-per-descriptor-set shape by
  using two descriptor sets from one layout and drawing in two passes per
  frame (bind the font set and draw every flat-color/text quad, then bind
  the icon set and draw the 8 icon quads on top). Click a cell to select
  it and read its real description, word-wrapped to the panel width
  (Tutorial16's per-part model matrix was later fixed to match
  `Tank::setTankPos()`'s real hierarchical composition - a child part's
  offset is rotated through its *parent's* basis columns before being
  added to the parent's translation, not added directly to a common
  origin - discovered while researching Tutorial18 below)
- Tutorial18: two real Hellfire tanks (`Tank::setTankPos()`'s real
  hierarchical positioning, see the Tutorial16 note above) tinted by an
  illustrative per-player team color via a push constant extended with a
  `Vec4` tint, plus a HUD overlay using the real
  `GameState::drawHUD()` health/power color ramps (`(1-ratio, ratio, 0)`
  and its inverse). First tutorial to combine a depth-tested 3D pass and
  a depth-disabled 2D overlay pass in one render pass/two pipelines -
  `Player`/`PlayerHuman`/`PlayerCPU` themselves have no rendering code at
  all (confirmed via full reads), so this is the real rendering logic
  living next to Player: `Tank::setTankPos()` and `GameState::drawHUD()`

## Common Tasks

### Adding a New Tutorial

1. Create `TutorialNN.h` in `include/vulkan_graphix/` and `TutorialNN.cpp`
   in `lib/`
2. Implement the tutorial class with Vulkan setup
3. Add `./TutorialNN.cpp` to `lib/Makefile.am` libvulkan_graphix_la_SOURCES
4. Create `TutorialNNMain.cpp` in bin/
5. Add binary target (`tutorialNN_runner`) to `bin/Makefile.am`
6. Create shader files in `resources/NN/Data/` (`shader.NN.vert`/
   `shader.NN.frag`), compile with `compile_shaders.sh` (or `glslc`
   directly - see README.md), and copy the compiled `.spv`/any texture
   assets into the build dir via an `all-local:` rule in `bin/Makefile.am`

### Debugging

Binaries can be run directly after building:
```sh
./build/bin/tutorial01_runner
```

The logging system will output debug information. Set logging levels in code via the logging API.

### Shader Compilation Issues

If shader compilation fails:
1. Verify glslangValidator is installed: `which glslangValidator`
2. Check shader syntax validity
3. Generated `.spv.txt` contains disassembly and errors
