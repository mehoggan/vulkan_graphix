# Tutorial12 — Generated terrain

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial09](09_tessellated_terrain.md)'s shaders, on
[VulkanCommon](vulkancommon.md) · Next:
[Tutorial13](13_projectile_model.md)

A diamond-square height field from `TerrainGenerator` (the same code
vulkan_earth's `TerrainMaker` now calls), lit and textured with Tutorial09's
shaders.

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
    TEX["<b>loadRawTexture</b><br/>Rocky.raw 2048² · REPEAT<br/>↩ Tutorial09's tiling"]:::changed
    UBO["↩ Tutorial09<br/>createUniformBuffer<br/>matrices + light, rewritten per frame"]:::reused
    DESC["↩ Tutorial09<br/>bindings: 0 UBO (vert + frag), 1 sampler (frag)"]:::reused
    DEPTH["↩ Tutorial08<br/>createDepthImage (D32_SFLOAT)"]:::reused
    RP["<b>createRenderPass</b><br/>color + depth"]:::shared
    PIPE["↩ Tutorial09<br/>shader.12 = Tutorial09's GLSL<br/>position + normal + texcoord<br/>depth test LESS · cull NONE"]:::reused
    VB["<b>createDeviceLocalBuffer</b><br/>TerrainGenerator grid → triangle list<br/>no index buffer"]:::new
    FL["↩ VulkanCommon<br/>FrameLoop::create(depth view)"]:::shared
    DRAW(["<b>draw()</b><br/>write UBO from OrbitCamera<br/>clear color + depth 1.0<br/>bind · vkCmdDraw(vertex count)"]):::frame

    BASE --> RC --> TEX --> UBO --> DESC --> DEPTH --> RP --> PIPE --> VB --> FL --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| createRenderPass | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create(depth view) | [VulkanCommon](vulkancommon.md) |
| loadRawTexture | [Tutorial09](09_tessellated_terrain.md) |
| createUniformBuffer | [Tutorial09](09_tessellated_terrain.md) |
| bindings: 0 UBO (vert + frag), 1 sampler (frag) | [Tutorial09](09_tessellated_terrain.md) |
| shader.12 = Tutorial09's GLSL | [Tutorial09](09_tessellated_terrain.md) |
| createDepthImage (D32_SFLOAT) | [Tutorial08](08_phong_sphere.md) |

## What's new

- First library-built tutorial with a depth attachment, passed to
  `FrameLoop::create()` as the framebuffers' second view.
- `.raw` texture loading (`ResourceContext::loadRawTexture`).
- Geometry from `TerrainGenerator`. [Tutorial21](21_world_scene.md) reuses
  this terrain and pipeline.

Source: `bin/12_generated_terrain/Tutorial12.cpp`
