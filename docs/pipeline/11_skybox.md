# Tutorial11 — Skybox

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial07](07_uniform_buffers.md)'s shaders, rebuilt on
[VulkanCommon](vulkancommon.md) · Next:
[Tutorial12](12_generated_terrain.md)

An indexed, textured cube using vulkan_earth's skybox faces, viewed with
the orbit camera. This is the first tutorial built on `ResourceContext`
and `FrameLoop`, so everything Tutorial03–10 wrote by hand is a library
call here.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef changed fill:#fff3e0,stroke:#ef6c00,color:#3e2723
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef shared fill:#e3f2fd,stroke:#1565c0,color:#0d47a1
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()"]:::reused
    RC["↩ VulkanCommon<br/>ResourceContext::initialize"]:::shared
    TEX["<b>loadTexture</b><br/>SkyBox.jpg · CLAMP_TO_EDGE"]:::new
    UBO["<b>createUniformBuffer</b><br/>one mat4: projection · view · model<br/>↩ Tutorial08's per-frame strategy"]:::changed
    DESC["↩ Tutorial07<br/>bindings: 0 sampler (frag), 1 UBO (vert)<br/>1 set from a 1-set pool"]:::reused
    RP["<b>createRenderPass</b><br/>color only, no depth"]:::shared
    PIPE["↩ Tutorial07<br/>shader.11 = Tutorial07's GLSL, unchanged<br/>position vec4 + texcoord vec2<br/>cull NONE (camera can be inside the cube)"]:::reused
    GEO["<b>Vertex + index buffers</b><br/>SkyboxGeometry: 24 vertices, 36 indices<br/>createDeviceLocalBuffer × 2"]:::new
    FL["↩ VulkanCommon<br/>FrameLoop::create"]:::shared
    DRAW(["<b>draw()</b><br/>vkDeviceWaitIdle · write UBO from OrbitCamera<br/>FrameLoop::draw: bind pipeline, VB, IB, set<br/>vkCmdDrawIndexed"]):::frame

    BASE --> RC --> TEX --> UBO --> DESC --> RP --> PIPE --> GEO --> FL --> DRAW

    click BASE "./tutorialbase.md" "TutorialBase"
    click RC "./vulkancommon.md" "VulkanCommon"
    click FL "./vulkancommon.md" "VulkanCommon"
    click RP "./vulkancommon.md" "VulkanCommon"
    click UBO "./08_phong_sphere.md" "Tutorial08"
    click DESC "./07_uniform_buffers.md" "Tutorial07"
    click PIPE "./07_uniform_buffers.md" "Tutorial07"
```

**Linked steps:** [TutorialBase](tutorialbase.md);
[VulkanCommon](vulkancommon.md) (context, render pass, frame loop);
[Tutorial07](07_uniform_buffers.md) (bindings and shaders);
[Tutorial08](08_phong_sphere.md) (per-frame uniform).

## What's new

- The library path: one `createResources()` builds everything, `draw()`
  only records, `childClear()` is `m_frames.destroy();
  m_resources.releaseAll();`.
- `SkyboxGeometry`, shared with vulkan_earth's `SkyboxFactory` and
  [Tutorial21](21_world_scene.md).

Source: `bin/11_skybox/Tutorial11.cpp`
