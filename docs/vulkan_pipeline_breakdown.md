# Vulkan Pipeline Setup: Tutorial 01–03 Breakdown

## Architecture Overview

| Tutorial | Base Class | Purpose |
|---|---|---|
| `Tutorial01` | `os::ProjectBase` | Device initialization only — no rendering |
| `Tutorial02` | `os::ProjectBase` | Adds presentation surface, swapchain, and a clear-color render via transfer commands |
| `Tutorial03` | `TutorialBase` | Builds on the abstracted base to add a real graphics pipeline, render pass, framebuffers, and shader-driven triangle draw |

`TutorialBase` (used by Tutorial03) encapsulates the full device + swapchain setup that Tutorial02 duplicates inline. Tutorial01 is a self-contained bootstrapper with no window or rendering.

---

## Pipeline Divergence Flowchart

```mermaid
flowchart TD
    A["**loadVulkanLibrary**
    dlopen('libvulkan.so.1')"] --> B

    B["**loadExportedEntryPoints**
    dlsym → vkGetInstanceProcAddr"] --> C

    C["**loadGlobalLevelEntryPoints**
    vkGetInstanceProcAddr(null, ...)
    vkCreateInstance · vkEnumerateInstanceLayerProperties"] --> D

    D["**createInstance**
    VkApplicationInfo + VkInstanceCreateInfo
    vkCreateInstance"] --> E

    E["**setupDebugMessenger** (optional)
    vkCreateDebugUtilsMessengerEXT
    VK_EXT_debug_utils"] --> F

    F["**loadInstanceLevelEntryPoints**
    vkGetInstanceProcAddr(instance, ...)
    vkEnumeratePhysicalDevices · vkCreateDevice"] --> DIV1

    DIV1{{"⬦ Divergence Point 1"}}

    DIV1 -->|"Tutorial01 only"| T01_DEV
    DIV1 -->|"Tutorial02 + Tutorial03–07"| T0203_SURF

    %% ─── TUTORIAL 01 BRANCH ───
    T01_DEV["**createDevice**
    No surface · No device extensions
    Selects VK_QUEUE_GRAPHICS_BIT only
    1× VkDeviceQueueCreateInfo"] --> T01_DLEP

    T01_DLEP["**loadDeviceLevelEntryPoints**
    vkGetDeviceProcAddr(device, ...)"] --> T01_GDQ

    T01_GDQ["**getDeviceQueue**
    vkGetDeviceQueue
    1 queue (graphics only)"] --> T01_DONE

    T01_DONE(["✓ Tutorial01 COMPLETE
    No surface · No swapchain · No rendering"])

    %% ─── TUTORIAL 02 + 03 SHARED ───
    T0203_SURF["**createPresentationSurface**
    vkCreateXlibSurfaceKHR
    Instance extensions: VK_KHR_surface
    + VK_KHR_xlib_surface"] --> T0203_DEV

    T0203_DEV["**createDevice** (surface-aware)
    vkEnumerateDeviceExtensionProperties
    vkGetPhysicalDeviceSurfaceSupportKHR
    Device extension: VK_KHR_swapchain
    Selects graphics queue + present queue"] --> T0203_DLEP

    T0203_DLEP["**loadDeviceLevelEntryPoints**
    vkGetDeviceProcAddr(device, ...)"] --> T0203_GDQ

    T0203_GDQ["**getDeviceQueue**
    vkGetDeviceQueue × 2
    graphics queue + present queue
    (may be same family or different)"] --> T0203_SEM

    T0203_SEM["**createSemaphores**
    vkCreateSemaphore × 2
    image_available_semaphore
    rendering_finished_semaphore"] --> DIV2

    DIV2{{"⬦ Divergence Point 2"}}

    DIV2 -->|"Tutorial02 only"| T02_SC
    DIV2 -->|"Tutorial03–07 (via TutorialBase)"| T03_SC

    %% ─── TUTORIAL 02 BRANCH ───
    T02_SC["**createSwapChain**
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR
    vkGetPhysicalDeviceSurfaceFormatsKHR
    vkGetPhysicalDeviceSurfacePresentModesKHR
    vkCreateSwapchainKHR
    ⚠ No VkImageViews created"] --> T02_CP

    T02_CP["**createCommandPool**
    vkCreateCommandPool
    ⚠ On PRESENT queue family"] --> T02_ACB

    T02_ACB["**allocateCommandBuffers**
    vkGetSwapchainImagesKHR
    vkAllocateCommandBuffers
    1× primary buffer per swapchain image"] --> T02_RCB

    T02_RCB["**recordCommandBuffers**
    vkBeginCommandBuffer
    vkCmdPipelineBarrier (UNDEFINED → TRANSFER_DST)
    vkCmdClearColorImage ← clear happens here
    vkCmdPipelineBarrier (TRANSFER_DST → PRESENT_SRC)
    vkEndCommandBuffer
    ⚠ No render pass · No pipeline · No shaders"] --> T02_DRAW

    T02_DRAW(["**draw() loop**
    vkAcquireNextImageKHR
    vkQueueSubmit → present queue
      wait: VK_PIPELINE_STAGE_TRANSFER_BIT
    vkQueuePresentKHR
    ─────────────────────
    ✓ Tutorial02 COMPLETE
    Renders solid clear color"])

    %% ─── TUTORIAL 03–07 SHARED TRUNK (TutorialBase) ───
    T03_SC["**createSwapChain** (via TutorialBase)
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR
    vkGetPhysicalDeviceSurfaceFormatsKHR
    vkGetPhysicalDeviceSurfacePresentModesKHR
    vkCreateSwapchainKHR"] --> T03_IV

    T03_IV["**createSwapChainImageViews** (TutorialBase)
    vkCreateImageView per swapchain image
    ✓ Required for framebuffer attachments"] --> T03_RP

    T03_RP["**createRenderPass**
    vkCreateRenderPass
    VkAttachmentDescription: loadOp=CLEAR storeOp=STORE
    finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    VkSubpassDescription: GRAPHICS bind point"] --> T03_FB

    T03_FB["**createFramebuffers**
    vkCreateFramebuffer × N
    Binds each VkImageView as color attachment
    Tied to the render pass"] --> DIV3

    DIV3{{"⬦ Divergence Point 3"}}

    DIV3 --> LBL_T03["Tutorial03 only"] --> T03_PL
    DIV3 --> LBL_T04["Tutorial04"] --> T04_PL
    DIV3 --> LBL_T05["Tutorial05"] --> T05_PL
    DIV3 --> LBL_T06["Tutorial06"] --> T06_DSL
    DIV3 --> LBL_T07["Tutorial07"] --> T07_DSL
    DIV3 --> LBL_T08["Tutorial08"] --> T08_RR
    DIV3 --> LBL_T09["Tutorial09"] --> T09_RR
    DIV3 --> LBL_T10["Tutorial10"] --> T10_RR

    T03_PL["**createPipeline**
    vkCreateShaderModule × 2 (vert + frag SPIR-V)
    VkPipelineShaderStageCreateInfo × 2
    VkPipelineVertexInputStateCreateInfo (no vertex buffers)
    VkPipelineInputAssemblyStateCreateInfo (TRIANGLE_LIST)
    VkPipelineViewportStateCreateInfo (300×300)
    VkPipelineRasterizationStateCreateInfo (FILL, cull BACK)
    VkPipelineMultisampleStateCreateInfo (1 sample)
    VkPipelineColorBlendStateCreateInfo (blend disabled)
    vkCreatePipelineLayout (empty — no descriptors)
    vkCreateGraphicsPipelines"] --> T03_CP

    T03_CP["**createCommandPool**
    vkCreateCommandPool
    ✓ On GRAPHICS queue family"] --> T03_ACB

    T03_ACB["**allocateCommandBuffers**
    vkAllocateCommandBuffers
    1× primary buffer per swapchain image"] --> T03_RCB

    T03_RCB["**recordCommandBuffers**
    vkBeginCommandBuffer
    (opt) vkCmdPipelineBarrier — queue ownership acquire
    vkCmdBeginRenderPass ← loadOp clears here
    vkCmdBindPipeline (GRAPHICS)
    vkCmdDraw(3, 1, 0, 0) ← 3 verts from vertex shader
    vkCmdEndRenderPass ← storeOp writes here
    (opt) vkCmdPipelineBarrier — queue ownership release
    vkEndCommandBuffer"] --> T03_DRAW

    T03_DRAW(["**draw() loop**
    vkAcquireNextImageKHR
    vkQueueSubmit → graphics queue
      wait: VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    vkQueuePresentKHR → present queue
    ─────────────────────
    ✓ Tutorial03 COMPLETE
    Renders shader-driven triangle"])

    %% ─── TUTORIAL 04 BRANCH ───
    T04_PL["**createPipeline**
    VkVertexInputBindingDescription × 1
    VkVertexInputAttributeDescription × 2 (position, color)
    topology = TRIANGLE_STRIP
    PipelineLayout: empty (no descriptors)"] --> T04_VB

    T04_VB["**createVertexBuffer**
    vkCreateBuffer (VERTEX_BUFFER_BIT)
    HOST_VISIBLE memory · vkAllocateMemory · vkBindBufferMemory
    vkMapMemory → memcpy → vkFlushMappedMemoryRanges → vkUnmapMemory
    ⚠ Direct host-visible upload, no staging"] --> T04_CP

    T04_CP["**createCommandPool**
    vkCreateCommandPool
    ✓ On GRAPHICS queue family"] --> T04_ACB

    T04_ACB["**allocateCommandBuffers**
    1× primary buffer per swapchain image"] --> T04_RCB

    T04_RCB["**recordCommandBuffers**
    vkCmdBeginRenderPass → vkCmdBindPipeline
    vkCmdBindVertexBuffers ← NEW
    vkCmdDraw(4, 1, 0, 0) ← quad, not triangle
    vkCmdEndRenderPass"] --> T04_DRAW

    T04_DRAW(["**draw() loop**
    (acquire/submit/present same pattern as Tutorial03)
    ─────────────────────
    ✓ Tutorial04 COMPLETE
    Renders vertex-buffer-driven colored quad"])

    %% ─── TUTORIAL 05 BRANCH ───
    T05_PL["**createPipeline**
    Same vertex input / layout as Tutorial04"] --> T05_VB

    T05_VB["**createVertexBuffer**
    vkCreateBuffer (VERTEX_BUFFER_BIT | TRANSFER_DST_BIT)
    DEVICE_LOCAL memory ← NEW (fast GPU-local, not host-visible)"] --> T05_STG

    T05_STG["**createStagingBuffer**
    vkCreateBuffer (TRANSFER_SRC_BIT)
    HOST_VISIBLE memory"] --> T05_COPY

    T05_COPY["**copyVertexData**
    map/memcpy/flush staging buffer
    vkCmdCopyBuffer (staging → vertex) ← NEW
    VkBufferMemoryBarrier: TRANSFER_WRITE → VERTEX_ATTRIBUTE_READ
    one-off vkQueueSubmit + vkDeviceWaitIdle"] --> T05_CP

    T05_CP["**createCommandPool**
    vkCreateCommandPool
    ✓ On GRAPHICS queue family"] --> T05_ACB

    T05_ACB["**allocateCommandBuffers**
    1× primary buffer per swapchain image"] --> T05_RCB

    T05_RCB["**recordCommandBuffers**
    vkCmdBindVertexBuffers
    vkCmdDraw(4, 1, 0, 0)"] --> T05_DRAW

    T05_DRAW(["**draw() loop**
    (same pattern as Tutorial04)
    ─────────────────────
    ✓ Tutorial05 COMPLETE
    Same quad, now via staging→device-local upload"])

    %% ─── TUTORIAL 06 BRANCH ───
    T06_DSL["**createDescriptorSetLayout** ← NEW
    vkCreateDescriptorSetLayout
    1 binding: COMBINED_IMAGE_SAMPLER (fragment stage)"] --> T06_DP

    T06_DP["**createDescriptorPool + allocateDescriptorSet**
    vkCreateDescriptorPool (1× COMBINED_IMAGE_SAMPLER, maxSets=1)
    vkAllocateDescriptorSets"] --> T06_TEX

    T06_TEX["**createTexture**
    Load PNG (Tools::getImageData)
    vkCreateImage (TILING_OPTIMAL, TRANSFER_DST | SAMPLED)
    DEVICE_LOCAL memory · vkBindImageMemory
    vkCreateImageView + vkCreateSampler (LINEAR, CLAMP_TO_EDGE)"] --> T06_COPY

    T06_COPY["**copyTextureData**
    staging buffer upload
    vkCmdCopyBufferToImage
    VkImageMemoryBarrier ×2:
      UNDEFINED → TRANSFER_DST_OPTIMAL
      TRANSFER_DST_OPTIMAL → SHADER_READ_ONLY_OPTIMAL"] --> T06_UDS

    T06_UDS["**updateDescriptorSet**
    VkDescriptorImageInfo + VkWriteDescriptorSet
    vkUpdateDescriptorSets"] --> T06_PL

    T06_PL["**createPipeline**
    Vertex input: position + texcoord (u, v)
    PipelineLayout ← references descriptor set layout (NEW)
    RenderPass: initial/finalLayout = COLOR_ATTACHMENT_OPTIMAL
    ⚠ 0 subpass dependencies (barriers moved to per-frame)"] --> T06_VB

    T06_VB["**createVertexBuffer**
    Device-local via staging (pos + uv attributes)"] --> T06_CP

    T06_CP["**createCommandPool**
    vkCreateCommandPool
    ✓ On GRAPHICS queue family"] --> T06_ACB

    T06_ACB["**allocateCommandBuffers**
    1× primary buffer per swapchain image"] --> T06_RCB

    T06_RCB["**recordCommandBuffers / prepareFrame**
    Per-frame VkImageMemoryBarrier (no subpass deps)
    vkCmdBindVertexBuffers
    vkCmdBindDescriptorSets ← NEW
    vkCmdDraw(4, 1, 0, 0)"] --> T06_DRAW

    T06_DRAW(["**draw() loop**
    ─────────────────────
    ✓ Tutorial06 COMPLETE
    Renders a textured quad (image + sampler)"])

    %% ─── TUTORIAL 07 BRANCH ───
    T07_DSL["**createDescriptorSetLayout**
    vkCreateDescriptorSetLayout
    binding 0: COMBINED_IMAGE_SAMPLER (fragment)
    binding 1: UNIFORM_BUFFER (vertex) ← NEW"] --> T07_DP

    T07_DP["**createDescriptorPool + allocateDescriptorSet**
    Sized for 2 descriptor types"] --> T07_TEX

    T07_TEX["**createTexture + copyTextureData**
    (identical to Tutorial06)"] --> T07_UBO

    T07_UBO["**createUniformBuffer** ← NEW
    vkCreateBuffer (UNIFORM_BUFFER_BIT | TRANSFER_DST_BIT)
    size = 16 × sizeof(float) (mat4)
    DEVICE_LOCAL memory"] --> T07_COPYU

    T07_COPYU["**copyUniformBufferData**
    Tools::getOrthographicProjectionMatrix(swapchain extent)
    staging upload → vkCmdCopyBuffer
    VkBufferMemoryBarrier: TRANSFER_WRITE → UNIFORM_READ_BIT
    stage: TRANSFER_BIT → VERTEX_SHADER_BIT"] --> T07_UDS

    T07_UDS["**updateDescriptorSet**
    2× VkWriteDescriptorSet (image info + buffer info)"] --> T07_PL

    T07_PL["**createPipeline**
    Vertex input: position (pixel-space) + texcoord
    PipelineLayout: descriptor set (sampler + UBO)"] --> T07_VB

    T07_VB["**createVertexBuffer**
    Device-local via staging"] --> T07_CP

    T07_CP["**createCommandPool**
    vkCreateCommandPool
    ✓ On GRAPHICS queue family"] --> T07_ACB

    T07_ACB["**allocateCommandBuffers**
    1× primary buffer per swapchain image"] --> T07_RCB

    T07_RCB["**recordCommandBuffers / prepareFrame**
    vkCmdBindVertexBuffers
    vkCmdBindDescriptorSets (sampler + UBO)
    vkCmdDraw(4, 1, 0, 0)"] --> T07_DRAW

    T07_DRAW(["**draw() loop**
    ─────────────────────
    ✓ Tutorial07 COMPLETE
    Textured quad transformed by projection matrix"])

    T07_DRAW -.->|"on window resize"| T07_RESIZE["**childOnWindowSizeChanged()** ← NEW
    vkDeviceWaitIdle
    + re-run copyUniformBufferData()
    (recompute projection matrix for new extent)"]

    %% ─── TUTORIAL 08 BRANCH (diverges from Tutorial07 at DIV3) ───
    T08_RR["**createRenderingResources**
    Same as Tutorial07 (cmd pool/buffers, fences, semaphores)"] --> T08_STG

    T08_STG["**createStagingBuffer**
    Same as Tutorial07"] --> T08_DEPTH

    T08_DEPTH["**createDepthResources** ← NEW
    vkCreateImage (VK_FORMAT_D32_SFLOAT, DEPTH_STENCIL_ATTACHMENT_BIT)
    DEVICE_LOCAL memory · vkCreateImageView (ASPECT_DEPTH_BIT)
    ⚠ First tutorial with a depth buffer"] --> T08_UBO

    T08_UBO["**createUniformBuffer** ← differs from Tutorial07
    vkCreateBuffer (UNIFORM_BUFFER_BIT)
    HOST_VISIBLE | HOST_COHERENT (Tutorial07 was DEVICE_LOCAL + staging)
    mat4 model/view/projection + vec4 light_pos/light_color/view_pos
    Rewritten directly every frame — no staging, no one-off submit"] --> T08_DSL

    T08_DSL["**createDescriptorSetLayout** ← differs from Tutorial07
    vkCreateDescriptorSetLayout
    binding 0: UNIFORM_BUFFER (VERTEX | FRAGMENT stages)
    ⚠ No COMBINED_IMAGE_SAMPLER binding — no texture in this tutorial"] --> T08_DP

    T08_DP["**createDescriptorPool + allocateDescriptorSet**
    Sized for 1× UNIFORM_BUFFER only"] --> T08_UDS

    T08_UDS["**updateDescriptorSet**
    1× VkWriteDescriptorSet (buffer info only)"] --> T08_RP

    T08_RP["**createRenderPass** ← differs from Tutorial07
    VkAttachmentDescription × 2: color + depth (NEW)
    VkSubpassDescription.pDepthStencilAttachment set (NEW)
    ⚠ First tutorial with a depth attachment"] --> T08_PLL

    T08_PLL["**createPipelineLayout**
    References descriptor set layout (UBO only)"] --> T08_PL

    T08_PL["**createPipeline** ← differs from Tutorial07
    Vertex input: position (vec4) + normal (vec3) ← NEW attribute
    topology = TRIANGLE_LIST (Tutorial07 was TRIANGLE_STRIP)
    VkPipelineDepthStencilStateCreateInfo (NEW): depthTest/Write, LESS
    cullMode = BACK, frontFace = CLOCKWISE (Tutorial03–07 were
      COUNTER_CLOCKWISE — this mesh's winding is reversed)
    Tools::getPerspectiveProjectionMatrix ← first non-orthographic
      projection (Tutorial07 used getOrthographicProjectionMatrix)"] --> T08_VB

    T08_VB["**createVertexBuffer**
    Math::Sphere<float,uint32_t> icosphere (edge-midpoint subdivision)
    Device-local via staging (position + normal, not position + texcoord)"] --> T08_IB

    T08_IB["**createIndexBuffer** ← NEW
    vkCreateBuffer (INDEX_BUFFER_BIT | TRANSFER_DST_BIT)
    Device-local via staging
    ⚠ First tutorial with an index buffer"] --> T08_DRAW

    T08_DRAW(["**draw() loop** ← differs from Tutorial07
    vkDeviceWaitIdle + updateUniformBufferData() every frame
      (re-uploads a Y-axis-rotating model matrix)
    vkCmdBindIndexBuffer + vkCmdDrawIndexed ← NEW (not vkCmdDraw)
    ─────────────────────
    ✓ Tutorial08 COMPLETE
    Renders a lit, rotating Phong-shaded sphere"])

    T08_DRAW -.->|"on window resize"| T08_RESIZE["**childOnWindowSizeChanged()** ← differs from Tutorial07
    Fully rebuilds every resource childClear() destroys:
    rendering resources → staging buffer → depth image →
    uniform buffer → descriptor set → render pass → pipeline →
    vertex buffer → index buffer
    ⚠ Tutorial07's version only re-ran copyUniformBufferData(),
    leaving fences/pipeline/buffers dangling after a real resize —
    same fix retroactively applied to Tutorial04–07"]

    %% ─── TUTORIAL 09 BRANCH (diverges from Tutorial08 at DIV3) ───
    T09_RR["**createRenderingResources**
    Same as Tutorial08 (cmd pool/buffers, fences, semaphores)"] --> T09_STG

    T09_STG["**createStagingBuffer**
    Same as Tutorial08"] --> T09_DEPTH

    T09_DEPTH["**createDepthResources**
    Same as Tutorial08 (VK_FORMAT_D32_SFLOAT)"] --> T09_TEX

    T09_TEX["**createTexture** ← re-introduces Tutorial07's texture
    loading (image + view + sampler + staged upload) - Tutorial08
    had no texture at all
    addressMode = REPEAT (Tutorial06/07 used CLAMP_TO_EDGE)
    ⚠ UVs intentionally exceed [0,1] so the ground texture tiles
      across the terrain instead of stretching one copy over it"] --> T09_UBO

    T09_UBO["**createUniformBuffer**
    Same shape/host-visible-every-frame strategy as Tutorial08"] --> T09_DSL

    T09_DSL["**createDescriptorSetLayout** ← differs from Tutorial08
    binding 0: UNIFORM_BUFFER (vertex+fragment, same as Tutorial08)
    binding 1: COMBINED_IMAGE_SAMPLER (fragment) ← NEW here
    (Tutorial08 had no sampler binding at all)"] --> T09_DP

    T09_DP["**createDescriptorPool + allocateDescriptorSet**
    Sized for 1× UNIFORM_BUFFER + 1× COMBINED_IMAGE_SAMPLER"] --> T09_UDS

    T09_UDS["**updateDescriptorSet**
    2× VkWriteDescriptorSet (buffer info + image info)"] --> T09_RP

    T09_RP["**createRenderPass**
    Same as Tutorial08 (color + depth attachments)"] --> T09_PLL

    T09_PLL["**createPipelineLayout**
    References descriptor set layout (UBO + sampler)"] --> T09_PL

    T09_PL["**createPipeline** ← differs from Tutorial08
    Vertex input: position + normal + texcoord ← first tutorial
      combining all three attributes
    cullMode = NONE (Tutorial03–08 were all BACK) ← explicit ask:
      see the terrain's underside from below
    Fragment shader flips the normal to face the camera when
      culling is off, so the underside is lit instead of black"] --> T09_VB

    T09_VB["**createVertexBuffer** ← geometry source differs
    Two large triangles spanning a flat square, subdivided via
      Math::tessellateTrianglesByMidpointSubdivision (the *vector*
      overload, sharing one dedup map across both triangles so
      their shared diagonal edge doesn't duplicate vertices)
    ⚠ First tutorial building its mesh from the tessellation module
      directly, instead of a purpose-built generator like
      Tutorial08's Math::Sphere"] --> T09_IB

    T09_IB["**createIndexBuffer**
    Same mechanism as Tutorial08; indices come from the tessellator"] --> T09_DRAW

    T09_DRAW(["**draw() loop** ← differs from Tutorial08
    Model matrix: identity, static terrain (Tutorial08 rotated its
      model instead)
    View matrix: rebuilt from OrbitCamera state every frame
      (Tutorial08 used a fixed glm::lookAt)
    vkDeviceWaitIdle + updateUniformBufferData() every frame, same
      single-shared-UBO rationale as Tutorial08
    ─────────────────────
    ✓ Tutorial09 COMPLETE
    Renders a lit, textured, tessellated flat terrain, orbitable
    by mouse, underside visible"])

    T09_DRAW -.->|"on mouse drag / scroll"| T09_CAMERA["**OrbitCamera** ← NEW capability
    os::ProjectBase::onMouseButton/onMouseMove ← NEW virtuals
      (empty default bodies - Tutorial01–08 unaffected)
    os::Window::renderingLoop() now selects Button{Press,Release}Mask
      + PointerMotionMask and dispatches to them
    ⚠ First tutorial with any mouse input; left-drag orbits yaw/pitch
      (clamped to ±89°), scroll wheel (X11 buttons 4/5) zooms"]

    T09_DRAW -.->|"on window resize"| T09_RESIZE["**childOnWindowSizeChanged()**
    Same full-rebuild pattern as Tutorial08, now also recreating the
    texture image (the one resource kind Tutorial08 didn't have)"]

    %% ─── TUTORIAL 10 BRANCH (diverges from Tutorial09 at DIV3) ───
    T10_RR["**createRenderingResources**
    Same as Tutorial09 (cmd pool/buffers, fences, semaphores)"] --> T10_STG

    T10_STG["**createStagingBuffer**
    Same as Tutorial08/09"] --> T10_DEPTH

    T10_DEPTH["**createDepthResources**
    Same as Tutorial08/09 (VK_FORMAT_D32_SFLOAT)"] --> T10_UBO

    T10_UBO["**createUniformBuffer** ← no texture this time
    view + projection + light/view-position fields only
    (Tutorial09's texcoord/sampler fields dropped)"] --> T10_DSL

    T10_DSL["**createDescriptorSetLayout** ← differs from Tutorial09
    binding 0: UNIFORM_BUFFER only (vertex+fragment)
    (Tutorial09's sampler binding dropped — no texture here)"] --> T10_DP

    T10_DP["**createDescriptorPool + allocateDescriptorSet**
    Sized for 1× UNIFORM_BUFFER only"] --> T10_UDS

    T10_UDS["**updateDescriptorSet**
    1× VkWriteDescriptorSet (buffer info only)"] --> T10_RP

    T10_RP["**createRenderPass**
    Same as Tutorial08/09 (color + depth attachments)"] --> T10_PLL

    T10_PLL["**createPipelineLayout** ← differs from Tutorial09
    References descriptor set layout (UBO only)
    + VkPushConstantRange (fragment stage, vec4 color) ← NEW
      declared once, shared by both pipelines below even though
      only the line pipeline's shader reads it"] --> T10_TPL

    T10_TPL["**createTubePipeline** ← first tutorial with 2 pipelines
    Vertex input: position + normal (Phong-lit tube mesh)
    topology = TRIANGLE_LIST, cullMode = BACK
    Fragment: Tutorial08-style Phong (ambient+diffuse+specular)
      vibrant teal object color"] --> T10_LPL

    T10_LPL["**createLinePipeline** ← NEW pipeline, NEW topology
    Vertex input: position only (no normal)
    topology = LINE_STRIP ← first tutorial with line topology
    Fragment: flat unlit color, read from the push constant
      set once per draw via vkCmdPushConstants
    Shares the tube pipeline's VkPipelineLayout/render pass"] --> T10_TVB

    T10_TVB["**createTubeVertexBuffer** ← geometry source differs
    Math::CubicCurve (CatmullRom) samples a spline through 5
      control points via computeSamplesAdaptive; each sample's
      position+tangent sweeps a ring of vertices (reference-up
      frame, radially outward normals) — the 'tube'
    ⚠ First tutorial building geometry from Math::CubicCurve rather
      than Math::Sphere (Tutorial08) or the tessellation module
      (Tutorial09)"] --> T10_TIB

    T10_TIB["**createTubeIndexBuffer**
    Ring-to-ring triangles, same two-triangles-per-quad shape as
      Tutorial09's terrain rows, but circularly wrapped per ring
      and open-ended along the path (no start/end caps)"] --> T10_LVB

    T10_LVB["**createLineVertexBuffer**
    The curve's raw control points, in order — the literal
      'polyline' drawn via the line pipeline as an unlit
      reference guide alongside the lit tube"] --> T10_DRAW

    T10_DRAW(["**draw() loop** ← differs from Tutorial09
    No texture, so no texture-related state
    vkDeviceWaitIdle + updateUniformBufferData() every frame, same
      single-shared-UBO rationale as Tutorial08/09
    Records two draw calls per frame: vkCmdDrawIndexed for the
      tube, then vkCmdPushConstants + vkCmdDraw for the line strip
    ─────────────────────
    ✓ Tutorial10 COMPLETE
    Renders a Phong-lit vibrant-teal tube swept along a sampled
      cubic curve, plus its unlit control-polygon guide line,
      orbitable by mouse"])

    T10_DRAW -.->|"on mouse drag / scroll"| T10_CAMERA["**OrbitCamera** ← reused, now shared
    Extracted out of Tutorial09.h/.cpp into its own
      include/vulkan_graphix/OrbitCamera.h + lib/OrbitCamera.cpp
      so Tutorial09 and Tutorial10 both depend on one definition
      instead of duplicating the class"]

    T10_DRAW -.->|"on window resize"| T10_RESIZE["**childOnWindowSizeChanged()**
    Same full-rebuild pattern as Tutorial08/09, now rebuilding
      both pipelines and three buffers (tube vertex/index + line
      vertex) instead of one pipeline and vertex/index buffers"]

    %% ─── STYLES ───
    classDef divLabel fill:#2a2a2a,color:#ccc,stroke:#555,font-size:11px
    class LBL_T03,LBL_T04,LBL_T05,LBL_T06,LBL_T07,LBL_T08,LBL_T09,LBL_T10 divLabel
    style DIV1 fill:#7b4f00,color:#fff,stroke:#c47d00
    style DIV2 fill:#7b4f00,color:#fff,stroke:#c47d00
    style DIV3 fill:#7b4f00,color:#fff,stroke:#c47d00
    style T01_DONE fill:#3a3a3a,color:#fff,stroke:#888
    style T02_DRAW fill:#0d5c7a,color:#fff,stroke:#0a90bf
    style T03_DRAW fill:#0d6b35,color:#fff,stroke:#0aab50
    style T02_RCB  fill:#103d52,color:#ddf,stroke:#0a90bf
    style T03_RCB  fill:#0d4222,color:#ddf,stroke:#0aab50
    style T03_PL   fill:#2a1a4a,color:#ddf,stroke:#8855cc
    style T04_DRAW fill:#7a4a0d,color:#fff,stroke:#bf8a0a
    style T04_VB   fill:#3a2a10,color:#ddf,stroke:#bf8a0a
    style T05_DRAW fill:#7a0d4a,color:#fff,stroke:#bf0a7a
    style T05_COPY fill:#3a0a2a,color:#ddf,stroke:#bf0a7a
    style T06_DRAW fill:#0d3a7a,color:#fff,stroke:#0a5abf
    style T06_TEX  fill:#0a1e3a,color:#ddf,stroke:#0a5abf
    style T06_PL   fill:#0a1e3a,color:#ddf,stroke:#0a5abf
    style T07_DRAW fill:#4a0d7a,color:#fff,stroke:#7a0abf
    style T07_UBO  fill:#2a0a3a,color:#ddf,stroke:#7a0abf
    style T07_RESIZE fill:#2a0a3a,color:#ddf,stroke:#7a0abf
    style T08_DRAW fill:#0d7a4a,color:#fff,stroke:#0abf7a
    style T08_DEPTH fill:#0a3a24,color:#ddf,stroke:#0abf7a
    style T08_RP   fill:#0a3a24,color:#ddf,stroke:#0abf7a
    style T08_PL   fill:#0a3a24,color:#ddf,stroke:#0abf7a
    style T08_IB   fill:#0a3a24,color:#ddf,stroke:#0abf7a
    style T08_RESIZE fill:#0a3a24,color:#ddf,stroke:#0abf7a
    style T09_DRAW fill:#0d5a7a,color:#fff,stroke:#22ccff
    style T09_TEX  fill:#0a2a3a,color:#ddf,stroke:#22ccff
    style T09_PL   fill:#0a2a3a,color:#ddf,stroke:#22ccff
    style T09_VB   fill:#0a2a3a,color:#ddf,stroke:#22ccff
    style T09_CAMERA fill:#0a2a3a,color:#ddf,stroke:#22ccff
    style T09_RESIZE fill:#0a2a3a,color:#ddf,stroke:#22ccff
    style T10_DRAW fill:#0d7a66,color:#fff,stroke:#1be6c4
    style T10_DSL  fill:#0a3a30,color:#ddf,stroke:#1be6c4
    style T10_PLL  fill:#0a3a30,color:#ddf,stroke:#1be6c4
    style T10_TPL  fill:#0a3a30,color:#ddf,stroke:#1be6c4
    style T10_LPL  fill:#0a3a30,color:#ddf,stroke:#1be6c4
    style T10_TVB  fill:#0a3a30,color:#ddf,stroke:#1be6c4
    style T10_CAMERA fill:#0a3a30,color:#ddf,stroke:#1be6c4
    style T10_RESIZE fill:#0a3a30,color:#ddf,stroke:#1be6c4
```

---

## Detailed Step Breakdown

### Tutorial 01 — Device Initialization

**Goal:** Load Vulkan, create a logical device, retrieve a queue handle. No window, no rendering.

#### Step 1 — Load Vulkan Library
**Source:** `lib/Tutorial01.cpp:146`

```cpp
m_vulkan_library_handle = dlopen("libvulkan.so.1", RTLD_NOW);
```

The Vulkan loader (`libvulkan.so`) is a shared library that dispatches calls to the
real ICD. You obtain the first entry point via `dlsym` before any Vulkan API is
available.

> **Spec:** [`vkGetInstanceProcAddr`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetInstanceProcAddr.html)

---

#### Step 2 — Load Exported Entry Points
**Source:** `lib/Tutorial01.cpp:156`

Uses `dlsym` (via the `VK_EXPORTED_FUNCTION` macro over `ListOfFunctions.inl`) to pull
`vkGetInstanceProcAddr` out of the library. This is the only function obtained this way —
everything else flows through it.

> **Spec:** [`vkGetInstanceProcAddr`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetInstanceProcAddr.html)

---

#### Step 3 — Load Global-Level Entry Points
**Source:** `lib/Tutorial01.cpp:174`

Calls `vkGetInstanceProcAddr(nullptr, ...)` to load functions that don't require a
`VkInstance` yet:

- [`vkCreateInstance`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateInstance.html)
- [`vkEnumerateInstanceLayerProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumerateInstanceLayerProperties.html)
- [`vkEnumerateInstanceExtensionProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumerateInstanceExtensionProperties.html)

---

#### Step 4 — Create Instance
**Source:** `lib/Tutorial01.cpp:190`

1. **`checkValidationLayerSupport()`** — calls
   [`vkEnumerateInstanceLayerProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumerateInstanceLayerProperties.html)
   and verifies `VK_LAYER_KHRONOS_validation` is present.

2. **Fill [`VkApplicationInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkApplicationInfo.html)** — declares the app name, engine name, and
   `apiVersion = VK_MAKE_VERSION(1,3,0)`.

3. **Fill [`VkInstanceCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkInstanceCreateInfo.html)** — attaches `VkApplicationInfo`; conditionally
   enables `VK_EXT_DEBUG_UTILS_EXTENSION_NAME` when debug mode is on.

4. **[`vkCreateInstance`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateInstance.html)** — creates the `VkInstance`.

5. **`setupDebugMessenger()`** — dynamically loads
   [`vkCreateDebugUtilsMessengerEXT`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDebugUtilsMessengerEXT.html)
   via `vkGetInstanceProcAddr` and registers a
   [`VkDebugUtilsMessengerEXT`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkDebugUtilsMessengerEXT.html)
   that routes validation messages to the logging system.

---

#### Step 5 — Load Instance-Level Entry Points
**Source:** `lib/Tutorial01.cpp:253`

Calls `vkGetInstanceProcAddr(instance, ...)` to load instance-scoped functions:

- [`vkEnumeratePhysicalDevices`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumeratePhysicalDevices.html)
- [`vkGetPhysicalDeviceProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceProperties.html)
- [`vkGetPhysicalDeviceFeatures`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceFeatures.html)
- [`vkGetPhysicalDeviceQueueFamilyProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceQueueFamilyProperties.html)
- [`vkCreateDevice`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDevice.html)

---

#### Step 6 — Create Logical Device
**Source:** `lib/Tutorial01.cpp:272`

1. [`vkEnumeratePhysicalDevices`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumeratePhysicalDevices.html) — enumerate all GPUs.

2. **`checkPhysicalDeviceProperties()`** — for each physical device:
   - [`vkGetPhysicalDeviceProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceProperties.html) — checks `apiVersion >= 1.x` and
     `maxImageDimension2D >= 4096`.
   - [`vkGetPhysicalDeviceQueueFamilyProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceQueueFamilyProperties.html) — selects the first
     queue family with `VK_QUEUE_GRAPHICS_BIT`.

3. Fill [`VkDeviceQueueCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkDeviceQueueCreateInfo.html) — one queue, priority `1.0f`.

4. Fill [`VkDeviceCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkDeviceCreateInfo.html) — no extensions, no features beyond defaults.

5. [`vkCreateDevice`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateDevice.html) — creates the `VkDevice`.

---

#### Step 7 — Load Device-Level Entry Points
**Source:** `lib/Tutorial01.cpp:412`

Calls [`vkGetDeviceProcAddr`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetDeviceProcAddr.html) to load device-scoped functions.

---

#### Step 8 — Get Device Queue
**Source:** `lib/Tutorial01.cpp:429`

[`vkGetDeviceQueue`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetDeviceQueue.html) — retrieves queue handle at family index 0, queue index 0. One queue
handles everything (graphics).

**Tutorial01 is complete here. It does not draw or present.**

---

### Tutorial 02 — Swapchain + Clear-Color Render

**Goal:** Open an X11 window, create a surface, set up a swapchain, and present a solid
color via transfer commands (no shader pipeline).

Steps 1–5 are identical to Tutorial01, then diverge:

---

#### Step 5b — Create Presentation Surface
**Source:** `lib/Tutorial02.cpp:713`

Inserted **between** `loadInstanceLevelEntryPoints` and `createDevice`.

```cpp
VkXlibSurfaceCreateInfoKHR surface_create_info = {
    .dpy    = m_window_parameters.getDisplayPtr(),
    .window = m_window_parameters.getWindowHandle()
};
vkCreateXlibSurfaceKHR(instance, &surface_create_info, nullptr, &surface);
```

> **Spec:** [`vkCreateXlibSurfaceKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateXlibSurfaceKHR.html) ·
> [`VkSurfaceKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkSurfaceKHR.html)

The instance was created with `VK_KHR_SURFACE_EXTENSION_NAME` and
`VK_KHR_XLIB_SURFACE_EXTENSION_NAME` enabled — these are checked against
[`vkEnumerateInstanceExtensionProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumerateInstanceExtensionProperties.html).

---

#### Step 6b — Create Device (surface-aware)
**Source:** `lib/Tutorial02.cpp:733`

Enhanced over Tutorial01:

1. [`vkEnumerateDeviceExtensionProperties`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumerateDeviceExtensionProperties.html) — verifies
   `VK_KHR_SWAPCHAIN_EXTENSION_NAME` is available.

2. [`vkGetPhysicalDeviceSurfaceSupportKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceSurfaceSupportKHR.html) — for each queue family, checks
   whether it can present to the surface. Prefers a family supporting both graphics and
   present; creates two `VkDeviceQueueCreateInfo` entries if needed.

3. [`VkDeviceCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkDeviceCreateInfo.html) now includes
   `extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }`.

---

#### Step 7b — Get Both Queues
**Source:** `lib/Tutorial02.cpp:980`

Two calls to [`vkGetDeviceQueue`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetDeviceQueue.html): one for `graphics_queue`, one for `present_queue`.

---

#### Step 8b — Create Semaphores
**Source:** `lib/Tutorial02.cpp:994`

Two [`vkCreateSemaphore`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateSemaphore.html) calls:

- `image_available_semaphore` — signaled by `vkAcquireNextImageKHR`, waited on before
  rendering.
- `rendering_finished_semaphore` — signaled after rendering, waited on before present.

---

#### Step 9b — Create Swapchain
**Source:** `lib/Tutorial02.cpp:304` (called via `onWindowSizeChanged`)

1. [`vkGetPhysicalDeviceSurfaceCapabilitiesKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceSurfaceCapabilitiesKHR.html) — surface size limits,
   supported transforms, image counts.

2. [`vkGetPhysicalDeviceSurfaceFormatsKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceSurfaceFormatsKHR.html) — selects
   `VK_FORMAT_R8G8B8A8_UNORM` or fallback.

3. [`vkGetPhysicalDeviceSurfacePresentModesKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetPhysicalDeviceSurfacePresentModesKHR.html) — prefers
   `VK_PRESENT_MODE_MAILBOX_KHR`, falls back to `FIFO`.

4. [`vkCreateSwapchainKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateSwapchainKHR.html) with:
   - `minImageCount = surfaceCaps.minImageCount + 1`
   - `imageUsage = COLOR_ATTACHMENT | TRANSFER_DST`
   - `imageSharingMode = EXCLUSIVE`

---

#### Step 10b — Create Command Buffers
**Source:** `lib/Tutorial02.cpp:452`

1. [`vkCreateCommandPool`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateCommandPool.html) — on the **present** queue family.

2. [`vkGetSwapchainImagesKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetSwapchainImagesKHR.html) — get one `VkImage` handle per swapchain image.

3. [`vkAllocateCommandBuffers`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkAllocateCommandBuffers.html) — one primary command buffer per swapchain image.

4. **`recordCommandBuffers()`** — for each buffer:
   - [`vkBeginCommandBuffer`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkBeginCommandBuffer.html) with `VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT`
   - [`vkCmdPipelineBarrier`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdPipelineBarrier.html) — transition `UNDEFINED → TRANSFER_DST_OPTIMAL`
   - [`vkCmdClearColorImage`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdClearColorImage.html) — fills with `{1.0, 0.8, 0.4, 0.0}` (orange-yellow)
   - [`vkCmdPipelineBarrier`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdPipelineBarrier.html) — transition `TRANSFER_DST_OPTIMAL → PRESENT_SRC_KHR`
   - [`vkEndCommandBuffer`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEndCommandBuffer.html)

> **Key difference from Tutorial03:** No render pass, no shaders. The clear is a raw
> transfer command (`vkCmdClearColorImage`) rather than a render pass `loadOp`.

---

#### Step 11b — Render Loop
**Source:** `lib/Tutorial02.cpp:508`

1. [`vkAcquireNextImageKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkAcquireNextImageKHR.html) — get next available swapchain image index.

2. [`vkQueueSubmit`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkQueueSubmit.html) — submits to **present queue**; waits at
   `VK_PIPELINE_STAGE_TRANSFER_BIT`.

3. [`vkQueuePresentKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkQueuePresentKHR.html) — presents the image.

---

### Tutorial 03 — Real Graphics Pipeline + Triangle Draw

**Goal:** Use a render pass, framebuffers, SPIR-V shaders, and a proper `VkPipeline` to
draw a procedurally-generated triangle (vertex positions in the vertex shader, no vertex
buffers).

Tutorial03 inherits `TutorialBase`, which encapsulates all the device + swapchain setup
from Tutorial02, plus `createSwapChainImageViews()`. The new work is in
`childOnWindowSizeChanged()`.

---

#### Step 9c — Create Swapchain Image Views (TutorialBase)

For each `VkImage` from [`vkGetSwapchainImagesKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkGetSwapchainImagesKHR.html), creates a
[`VkImageView`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkImageView.html) via
[`vkCreateImageView`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateImageView.html).
These views are required by framebuffers — Tutorial02 skips this because it clears
directly into `VkImage` handles via transfer.

---

#### Step 10c — Create Render Pass
**Source:** `lib/Tutorial03.cpp:141`

```
VkAttachmentDescription:
  format        = swapchain format
  loadOp        = CLEAR    ← clear happens here at render pass begin
  storeOp       = STORE    ← result written to image at render pass end
  initialLayout = UNDEFINED
  finalLayout   = PRESENT_SRC_KHR

VkSubpassDescription:
  pipelineBindPoint = GRAPHICS
  colorAttachment   = attachment[0] at COLOR_ATTACHMENT_OPTIMAL
```

> **Spec:** [`vkCreateRenderPass`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateRenderPass.html) ·
> [`VkAttachmentDescription`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkAttachmentDescription.html) ·
> [`VkSubpassDescription`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkSubpassDescription.html)

---

#### Step 11c — Create Framebuffers
**Source:** `lib/Tutorial03.cpp:193`

One [`VkFramebuffer`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkFramebuffer.html) per swapchain image, each binding that image's `VkImageView`
as the color attachment for the render pass.

> **Spec:** [`vkCreateFramebuffer`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateFramebuffer.html)

---

#### Step 12c — Create Graphics Pipeline
**Source:** `lib/Tutorial03.cpp:224`

##### 12c.1 — Shader Modules
[`vkCreateShaderModule`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateShaderModule.html) × 2 from pre-compiled SPIR-V files:

- `shader.03.vert.spv` — vertex shader (generates triangle vertices from `gl_VertexIndex`)
- `shader.03.frag.spv` — fragment shader (outputs solid color)

##### 12c.2 — Shader Stages
[`VkPipelineShaderStageCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineShaderStageCreateInfo.html) × 2 — `VERTEX_BIT` and `FRAGMENT_BIT`, entry
point `"main"`.

##### 12c.3 — Vertex Input State
[`VkPipelineVertexInputStateCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineVertexInputStateCreateInfo.html) — `vertexBindingDescriptionCount = 0`.
No vertex buffers; positions come from the shader.

##### 12c.4 — Input Assembly State
[`VkPipelineInputAssemblyStateCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineInputAssemblyStateCreateInfo.html) — `topology = TRIANGLE_LIST`.

##### 12c.5 — Viewport State
[`VkPipelineViewportStateCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineViewportStateCreateInfo.html) — `{0, 0, 300, 300, 0, 1}` viewport and
scissor.

##### 12c.6 — Rasterization State
[`VkPipelineRasterizationStateCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineRasterizationStateCreateInfo.html) — `FILL`, `cull BACK`,
`COUNTER_CLOCKWISE`.

##### 12c.7 — Multisample State
[`VkPipelineMultisampleStateCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineMultisampleStateCreateInfo.html) — `VK_SAMPLE_COUNT_1_BIT` (no MSAA).

##### 12c.8 — Color Blend State
[`VkPipelineColorBlendStateCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineColorBlendStateCreateInfo.html) — `blendEnable = VK_FALSE`,
`colorWriteMask = R|G|B|A`.

##### 12c.9 — Pipeline Layout
[`vkCreatePipelineLayout`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreatePipelineLayout.html) — empty (no descriptor sets, no push constants).

##### 12c.10 — Assemble and Create
[`vkCreateGraphicsPipelines`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateGraphicsPipelines.html) via
[`VkGraphicsPipelineCreateInfo`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkGraphicsPipelineCreateInfo.html) tying all state together, referencing the render pass
and subpass 0.

---

#### Step 13c — Create Command Buffers
**Source:** `lib/Tutorial03.cpp:402`

1. [`vkCreateCommandPool`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCreateCommandPool.html) — on the **graphics** queue family (unlike Tutorial02's present
   queue pool).

2. [`vkAllocateCommandBuffers`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkAllocateCommandBuffers.html) — one per swapchain image.

---

#### Step 14c — Record Command Buffers
**Source:** `lib/Tutorial03.cpp:425`

Per buffer:

1. [`vkBeginCommandBuffer`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkBeginCommandBuffer.html)
2. *(if separate queues)* [`vkCmdPipelineBarrier`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdPipelineBarrier.html) — ownership acquire from present family
3. [`vkCmdBeginRenderPass`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBeginRenderPass.html) — begins render pass, `loadOp` clears to orange-yellow
4. [`vkCmdBindPipeline`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdBindPipeline.html) — `VK_PIPELINE_BIND_POINT_GRAPHICS`
5. [`vkCmdDraw(3, 1, 0, 0)`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDraw.html) — draw 3 vertices (1 instance); shader generates positions from
   `gl_VertexIndex`
6. [`vkCmdEndRenderPass`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdEndRenderPass.html) — `storeOp` writes result; transitions to `PRESENT_SRC_KHR`
7. *(if separate queues)* [`vkCmdPipelineBarrier`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdPipelineBarrier.html) — ownership release to present family
8. [`vkEndCommandBuffer`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkEndCommandBuffer.html)

---

#### Step 15c — Render Loop
**Source:** `lib/Tutorial03.cpp:536`

1. [`vkAcquireNextImageKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkAcquireNextImageKHR.html)
2. [`vkQueueSubmit`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkQueueSubmit.html) — submits to **graphics queue**; waits at
   `VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT`
3. [`vkQueuePresentKHR`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkQueuePresentKHR.html) — submits to **present queue**

---

## Key Differences Summary

| Concept | Tutorial01 | Tutorial02 | Tutorial03 | Tutorial04 | Tutorial05 | Tutorial06 | Tutorial07 | Tutorial08 | Tutorial09 | Tutorial10 |
|---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| `VkInstance` | Yes | Yes | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) |
| `VkDevice` | Yes (no exts) | Yes (`VK_KHR_swapchain`) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) |
| Debug messenger | Optional | Optional | Optional | Optional | Optional | Optional | Optional | Optional | Optional | Optional |
| `VkSurfaceKHR` | No | Yes (Xlib) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) |
| `VkSwapchainKHR` | No | Yes | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) | Yes (TutorialBase) |
| `VkImageView` per image | No | **No** | **Yes** | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| Present queue | No | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| `VkSemaphore` | No | 2 | 2 | 2 | 2 | 2 | 2 | 2 | 2 | 2 |
| `VkCommandPool` family | — | Present | **Graphics** | Graphics | Graphics | Graphics | Graphics | Graphics | Graphics | Graphics |
| `VkRenderPass` | No | **No** | **Yes** | Yes | Yes | Yes (0 subpass deps) | Yes (0 subpass deps) | Yes (0 subpass deps, **+ depth attachment**) | Yes (same as Tutorial08) | Yes (same as Tutorial08/09) |
| `VkFramebuffer` | No | No | **Yes** | Yes | Yes | Yes | Yes | Yes (color + depth) | Yes (color + depth) | Yes (color + depth) |
| `VkShaderModule` | No | No | **Yes** (vert + frag) | Yes (vert + frag) | Yes (vert + frag) | Yes (vert + frag) | Yes (vert + frag) | Yes (vert + frag) | Yes (vert + frag) | **Yes ×4** (2 pipelines: tube vert+frag, line vert+frag) |
| `VkPipelineLayout` | No | No | **Yes** (empty) | Yes (empty) | Yes (empty) | **Yes** (1 descriptor set) | Yes (1 descriptor set) | Yes (1 descriptor set) | Yes (1 descriptor set) | Yes (1 descriptor set **+ push constant range**) |
| `VkPipeline` | No | No | **Yes** | Yes | Yes | Yes | Yes | Yes (**+ depth-stencil state**) | Yes (+ depth-stencil state) | **Yes ×2** (tube: TRIANGLE_LIST/BACK cull; line: LINE_STRIP/cull NONE — first tutorial with 2 pipelines) |
| `VkBuffer` / vertex | No | No | No | **Yes** (host-visible, mapped) | **Yes** (device-local) | Yes (device-local) | Yes (device-local) | Yes (device-local) | Yes (device-local) | Yes (device-local) — **2 buffers** (tube ring verts + line control-polygon verts) |
| `VkBuffer` / index | No | No | No | No | No | No | No | **Yes** (device-local, via staging) | Yes (device-local, via staging) | Yes (device-local, via staging) — tube only |
| `VkBuffer` / staging | No | No | No | No | **Yes** (vertex upload) | Yes (vertex + texture upload) | Yes (vertex + texture + UBO upload) | Yes (vertex + index upload — **not** uniform) | Yes (vertex + index + **texture** upload — not uniform) | Yes (tube vertex + tube index + line vertex upload — not uniform) |
| `VkBuffer` / uniform | No | No | No | No | No | No | **Yes** (mat4 projection, device-local) | Yes (**host-visible/coherent**, rewritten every frame) | Yes (host-visible/coherent, rewritten every frame) | Yes (host-visible/coherent, rewritten every frame) |
| Descriptor set layout | No | No | No | No | No | **Yes** (1 binding: sampler) | **Yes** (2 bindings: sampler + UBO) | Yes (1 binding: UBO only — **no sampler**) | Yes (2 bindings: UBO + sampler, **re-added**) | Yes (1 binding: UBO only — sampler dropped again, no texture) |
| Descriptor pool / set | No | No | No | No | No | **Yes** | Yes (2 descriptor types) | Yes (1 descriptor type) | Yes (2 descriptor types) | Yes (1 descriptor type) |
| Texture image + sampler | No | No | No | No | No | **Yes** (CLAMP_TO_EDGE) | Yes (CLAMP_TO_EDGE) | **No** | **Yes** (**REPEAT** — tiled, not clamped) | **No** |
| Depth buffer | No | No | No | No | No | No | No | **Yes** (`VK_FORMAT_D32_SFLOAT`) | Yes (same format) | Yes (same format) |
| Mouse input | No | No | No | No | No | No | No | No | **Yes** (orbit camera: drag rotates, scroll zooms) | Yes (same `OrbitCamera`, **now extracted/shared**) |
| Push constants | No | No | No | No | No | No | No | No | No | **Yes** (`vec4` line color, fragment stage — first tutorial to use them) |
| Vertex attributes | N/A (shader-gen) | N/A | N/A (shader-gen) | position + color | position + color | position + texcoord | position (pixel-space) + texcoord | position + normal | position + normal + **texcoord** (first to combine all three) | tube: position + normal; line: position only (**two vertex layouts in one tutorial**, first time) |
| Cull mode / front face | N/A | N/A | BACK / CCW | BACK / CCW | BACK / CCW | BACK / CCW | BACK / CCW | BACK / CW | **NONE** (underside visible) | tube: BACK / CW; line: **NONE** (line topology has no winding) |
| Projection | N/A | N/A | N/A | N/A | N/A | N/A | Orthographic | Perspective | Perspective | Perspective |
| `vkCmdBindVertexBuffers` | No | No | No | **Yes** | Yes | Yes | Yes | Yes | Yes | Yes (×2 per frame — tube, then line) |
| `vkCmdBindIndexBuffer` | No | No | No | No | No | No | No | **Yes** | Yes | Yes (tube only) |
| `vkCmdBindDescriptorSets` | No | No | No | No | No | **Yes** | Yes | Yes | Yes | Yes |
| Clear method | N/A | `vkCmdClearColorImage` (transfer) | Render pass `loadOp = CLEAR` | Render pass `loadOp = CLEAR` | Render pass `loadOp = CLEAR` | Render pass `loadOp = CLEAR` | Render pass `loadOp = CLEAR` | Render pass `loadOp = CLEAR` (color **+ depth**) | Render pass `loadOp = CLEAR` (color + depth) | Render pass `loadOp = CLEAR` (color + depth) |
| Draw call | N/A | None | `vkCmdDraw(3,1,0,0)` | `vkCmdDraw(4,1,0,0)` | `vkCmdDraw(4,1,0,0)` | `vkCmdDraw(4,1,0,0)` | `vkCmdDraw(4,1,0,0)` | `vkCmdDrawIndexed(indexCount,1,0,0,0)` | `vkCmdDrawIndexed(indexCount,1,0,0,0)` | `vkCmdDrawIndexed` (tube) + `vkCmdDraw` (line) — **first tutorial with 2 draw calls per frame** |
| Vertex source | N/A | N/A | Shader (`gl_VertexIndex`) | Vertex buffer | Vertex buffer | Vertex buffer | Vertex buffer | Vertex + index buffer (icosphere) | Vertex + index buffer (**tessellated terrain grid**) | Vertex+index buffer (tube, swept-ring mesh from `Math::CubicCurve`) + vertex buffer (line, control polygon) |
| Per-frame CPU work | N/A | N/A | None | None | None | None | None | `vkDeviceWaitIdle` + uniform buffer rewrite (model rotation) | `vkDeviceWaitIdle` + uniform buffer rewrite (**camera** update) | `vkDeviceWaitIdle` + uniform buffer rewrite (camera update) |
| Resize behavior | N/A | Swapchain recreate | Full rebuild (render pass/pipeline/cmd buffers) | Full rebuild | Full rebuild | Full rebuild | Full rebuild (+ UBO re-upload) | Full rebuild (+ depth image) | Full rebuild (+ depth image **+ texture**) | Full rebuild (+ depth image, **2 pipelines + 3 buffers**, no texture) |
| Submit queue | — | Present queue | **Graphics queue** | Graphics queue | Graphics queue | Graphics queue | Graphics queue | Graphics queue | Graphics queue | Graphics queue |
| Submit wait stage | — | `TRANSFER_BIT` | `COLOR_ATTACHMENT_OUTPUT_BIT` | `COLOR_ATTACHMENT_OUTPUT_BIT` | `COLOR_ATTACHMENT_OUTPUT_BIT` | `COLOR_ATTACHMENT_OUTPUT_BIT` | `COLOR_ATTACHMENT_OUTPUT_BIT` | `COLOR_ATTACHMENT_OUTPUT_BIT` | `COLOR_ATTACHMENT_OUTPUT_BIT` | `COLOR_ATTACHMENT_OUTPUT_BIT` |

> **Resize behavior note:** Tutorial04–10 all now fully rebuild every
> resource `childClear()` destroys on a real window resize (fences,
> semaphores, command pool, pipeline(s), render pass, buffers, and the
> depth image for Tutorial08–10, plus the texture image for Tutorial09,
> plus the second pipeline for Tutorial10).
> Tutorial04–06 previously had a no-op `childOnWindowSizeChanged()` and
> Tutorial07's only re-ran `copyUniformBufferData()` — both left dangling
> handles after a resize and crashed on the next `draw()`. Fixed across
> the board; see git log for the fix commits. Tutorial09 and Tutorial10
> were both built with the fix already in place, so neither ever had this
> bug.
>
> **Push constants note:** Tutorial10's `VkPushConstantRange` is declared
> once on the shared `VkPipelineLayout` (fragment stage, a single `vec4`
> color) and used only by the line pipeline — the tube pipeline's shaders
> simply never reference it, which Vulkan permits. This keeps both
> pipelines on one layout instead of needing two.
