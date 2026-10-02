#!/usr/bin/env bash
# Recompiles vulkan_earth's GLSL shaders to the SPIR-V the game loads
# (resources/vulkan_earth/Data/shaders/*.spv, committed like every
# tutorial's own .spv). Needs glslc (shaderc).
set -euo pipefail
cd "$(dirname "$0")"
for shader in *.vert *.frag; do
    glslc -I. "$shader" -o "../Data/shaders/${shader}.spv"
done
