# Tutorial13 — Projectile model

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial11](11_skybox.md) · Next:
[Tutorial14](14_particle_sphere.md)

vulkan_earth's tank-shell mesh, parsed from its `.ogl` file and drawn with
Tutorial11's unlit-textured pipeline.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef shared fill:#e3f2fd,stroke:#1565c0,color:#0d47a1
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()"]:::reused
    RC["↩ VulkanCommon<br/>ResourceContext::initialize"]:::shared
    TEX["<b>loadRawTexture</b><br/>projectileDefault.raw 512² · CLAMP_TO_EDGE"]:::new
    UBO["↩ Tutorial11<br/>createUniformBuffer (one mat4)"]:::reused
    DESC["↩ Tutorial11<br/>bindings: 0 sampler (frag), 1 UBO (vert)"]:::reused
    RP["↩ Tutorial11<br/>createRenderPass (color only)"]:::reused
    PIPE["↩ Tutorial11<br/>shader.13 = Tutorial11's GLSL<br/>(itself Tutorial07's)<br/>position + texcoord"]:::reused
    VB["<b>createDeviceLocalBuffer</b><br/>Tools::loadOglTexturedMesh<br/>projectileDefault.ogl · no index buffer"]:::new
    FL["↩ VulkanCommon<br/>FrameLoop::create"]:::shared
    DRAW(["<b>draw()</b><br/>write UBO from OrbitCamera<br/>bind · vkCmdDraw(vertex count)"]):::frame

    BASE --> RC --> TEX --> UBO --> DESC --> RP --> PIPE --> VB --> FL --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create | [VulkanCommon](vulkancommon.md) |
| createUniformBuffer (one mat4) | [Tutorial11](11_skybox.md) |
| bindings: 0 sampler (frag), 1 UBO (vert) | [Tutorial11](11_skybox.md) |
| createRenderPass (color only) | [Tutorial11](11_skybox.md) |
| shader.13 = Tutorial11's GLSL | [Tutorial11](11_skybox.md) |

## What's new

- Mesh data from `Tools::loadOglMeshData()`, the single `.ogl` parser
  vulkan_earth uses too. Tutorial16, 18, 19, 21, and 22 load their meshes
  the same way.

Source: `bin/13_projectile_model/Tutorial13.cpp`
