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
- `./build/bin/tutorial01_runner` through `./build/bin/tutorial10_runner`

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
`Tools` each have their own direct unit tests. Overall `lib/`+`include/`
line coverage is ~66% (header-only `Math/`/`VertexTypes/` modules are
close to fully covered); most of the remaining gap is Vulkan-call
failure branches (`if (result != VK_SUCCESS) return false;`) that would
need a fault-injection/mocking layer to exercise, not real device/window
setup.

### Compiling Shaders

Shaders are compiled to SPIR-V bytecode using glslangValidator:
```sh
source ./compile_shaders.sh <folder> <shader_name>
# Example: source ./compile_shaders.sh Tutorials/03 shader
```

This compiles `.vert` and `.frag` files to `.spv` format in the Data folder.

## Code Structure

### Directory Layout

- **lib/**: Core library source files
  - `VulkanCommon.cpp/.h` - Shared Vulkan utilities and helpers
  - `Tutorial01-10.cpp/.h` - Individual tutorial implementations
  - `OrbitCamera.cpp/.h` - Mouse-orbit camera, shared by Tutorial09/10
  - `Logging.cpp/.h` - Boost-based logging framework
  - `LoggerHelpers.cpp/.h`, `LoggedClass.hpp` - Logging infrastructure
  - `Tools.cpp/.h` - Utility functions
  - `OperatingSystem.cpp/.h` - Platform-specific abstractions
  - `VulkanFunctions.cpp/.h` - Vulkan function wrappers

- **bin/**: Tutorial executable entry points
  - One main per active tutorial (01-10)

- **include/vulkan_graphix/**: Public headers
  - `ListOfFunctions.inl` - Pre-defined Vulkan function list
  - `stb_image.h` - Single-header image loading library
  - `vk_platform.h` - Platform-specific Vulkan definitions

- **Tutorials/** - Shader files (GLSL) organized per tutorial

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
- Variables: `lower_case`
- Minimum variable name length: 4 characters
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

## Common Tasks

### Adding a New Tutorial

1. Create `Tutorial0X.cpp/.h` in lib/
2. Implement the tutorial class with Vulkan setup
3. Add `Tutorial0X.cpp` to `lib/Makefile.am` libvulkan_graphix_la_SOURCES
4. Create `tutorial0X_main.cpp` in bin/
5. Add binary target to `bin/Makefile.am`
6. Create shader files in `Tutorials/0X/Data/`

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
