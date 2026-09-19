# vulkan_graphix

A collection of Vulkan API tutorials and examples, demonstrating progressive
concepts from device initialization through rendering.

### Generating .clangdb files for Development Environments

```sh
sudo apt install -y bear
mkdir build
cd build
bear -- ../configure
bear -- make
```

### Installing Ubuntu Dependencies
```sh
sudo apt install -y libvulkan-dev
sudo apt install -y vulkan-validationlayers spirv-tools
sudo apt install -y glslc
```

### Generating Shader Files

Each tutorial's GLSL sources live in `resources/NN/Data/shader.NN.{vert,frag}`
and are compiled to two committed artifacts alongside them:

- `shader.{vert,frag}.NN.spv` — the compiled SPIR-V binary the tutorial
  binary loads at runtime.
- `shader.{vert,frag}.NN.spv.txt` — a human-readable SPIR-V disassembly of
  that binary, committed for review/diffing; not read by any tutorial.

Both are generated with `glslc` (package `glslc`, from the `shaderc`
project):

```sh
cd resources/NN/Data
glslc shader.NN.vert -o shader.vert.NN.spv       # binary
glslc -S shader.NN.vert -o shader.vert.NN.spv.txt  # disassembly
glslc shader.NN.frag -o shader.frag.NN.spv
glslc -S shader.NN.frag -o shader.frag.NN.spv.txt
```

(replace `NN` with the tutorial number, e.g. `07`). The `spirv-tools`
package installed above provides `spirv-dis`/`spirv-val` if you'd rather
disassemble or validate an existing `.spv` directly instead of
recompiling from source — `spirv-dis` produces the same disassembly
style `glslc -S` does, since `glslc` uses it internally.

### Code Coverage

`tests/MathTest.cpp`/`tests/VertexTypesTest.cpp` exercise the header-only
`Math/`/`VertexTypes/` modules. `tests/TutorialNNIntegrationTest.cpp` (one
binary per tutorial, linked against `libvulkan_graphix.la`) drives each
tutorial through its real `prepareVulkan()`/`create*()`/`draw()`/
`onWindowSizeChanged()` sequence, plus mouse input for Tutorial09/10 —
these need a live Vulkan device and X11 window, so they call `GTEST_SKIP()`
when `DISPLAY` isn't set (e.g. headless CI) rather than failing.
`tests/OperatingSystemTest.cpp` drives the real X11 event loop
(`os::Window::renderingLoop()`) with synthetic events. `tests/ToolsTest.cpp`,
`tests/LoggingTest.cpp`, and `tests/LoggerHelpersTest.cpp` cover
`Tools`/`Logging`/`LoggerHelpers` directly — pure/file-based code needing
no device or window. `tests/TutorialBaseHelpersTest.cpp` covers
`TutorialBase`'s pure swap-chain-parameter-selection helpers the same way.

A `tests/*FaultInjectionTest.cpp` per tutorial (plus
`tests/TutorialBaseFaultInjection*Test.cpp` for the shared setup sequence)
covers the `if (result != VK_SUCCESS) return false;` branches a real
device essentially never takes: every Vulkan call in this codebase goes
through a mutable `vulkan_graphix::vkSomething` function-pointer global
(populated at runtime via `dlsym`/`vkGetInstanceProcAddr`/
`vkGetDeviceProcAddr`, see `VulkanFunctions.h`/`ListOfFunctions.inl`), so
a test can bring a tutorial up through a real `prepareVulkan()`, swap one
of those pointers for a fake that returns a chosen failure `VkResult`,
call the one step under test, assert it fails, and restore the real
pointer. See `tests/Tutorial04FaultInjectionTest.cpp` and
`tests/TutorialBaseFaultInjectionCommon.h`'s header comments for the full
technique, including a real, observed hazard: `vkGetDeviceProcAddr`-loaded
pointers are process-global but not actually valid across different
`VkDevice` objects, so each binary keeps to one real device per process.
The WSI surface/swapchain entry points (`vkCreateXlibSurfaceKHR`,
`vkCreateSwapchainKHR`, etc.) aren't routed through that seam — they
resolve to the real `libvulkan.so` symbols instead — so
`createPresentationSurface()`'s and `createSwapChain()`'s own failure
branches are out of reach without LD_PRELOAD interposition.

Overall `lib/`+`include/vulkan_graphix/` line coverage is ~71%; the
remaining gap is mostly those swapchain/surface branches, plus a few
`checkPhysicalDeviceProperties()` branches that need a fake device's
reported properties (not just a failure code) to reach.

Install a report generator (either works; `gcovr` needs no root access):

```sh
pip install --user gcovr
# or
sudo apt install -y lcov
```

Configure with coverage instrumentation (this passes `--coverage -O0` to
`lib/` and `tests/` only — `bin/` stays uninstrumented), then generate a
report:

```sh
./configure --enable-coverage
make -j8
make coverage
```

`make coverage` rebuilds, runs `make check`, and writes an HTML report to
`coverage-html/index.html` — open it in a browser for a per-file, line-by-
line view with uncovered lines highlighted. It prefers `gcovr` when both
tools are present; `lcov`/`genhtml` are used as a fallback.

To see exactly which lines aren't covered without opening a browser:

```sh
make coverage-missing
```

This prints a `gcovr --txt` table per file with a `Missing` column listing
the uncovered line numbers/ranges directly (run `make coverage` or
`make check` first if the `.gcda` files might be stale). This one needs
`gcovr` specifically — `lcov` has no equivalent per-line text report; with
`lcov` only, read uncovered lines off the highlighted source in `make
coverage`'s HTML output instead. Both targets filter out system headers,
`glm`, `gtest`, `boost`, and the vendored `STBImage.h`/`ListOfFunctions.inl`
— only this project's own `lib/` and `include/vulkan_graphix/` code is
reported on.

`make clean` removes the generated `.gcno`/`.gcda`/`coverage-html`/
`coverage.info` files. Coverage instrumentation adds real runtime and
binary-size overhead, so reconfigure without `--enable-coverage` (a plain
`./configure`) for normal development/tutorial-running.
