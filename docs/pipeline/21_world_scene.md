# Tutorial21 — World scene

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial11](11_skybox.md) (skybox), [Tutorial12](12_generated_terrain.md)
(terrain), and [Tutorial16](16_tank.md) (tank) · Next:
[Tutorial22](22_menu_tank_preview.md)

vulkan_earth's `GameState::draw()` order in one scene: skybox, then
terrain, then a tank placed on the terrain at the height the generator
reports. Three pipelines share one render pass and one depth buffer.

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
    TEX["<b>Three textures</b><br/>Rocky.raw, REPEAT (↩ Tutorial12)<br/>TestImage.raw (↩ Tutorial16)<br/>SkyBox.jpg (↩ Tutorial11)"]:::reused
    UBO["<b>Two uniform buffers</b><br/>terrain: matrices + light (↩ Tutorial12)<br/>object: view + projection (↩ Tutorial16)"]:::changed
    DESC["<b>Two set layouts, pool of 3 sets</b><br/>terrain set ↩ Tutorial12's bindings<br/>tank set + skybox set: object layout,<br/>same UBO, different texture"]:::changed
    DEPTH["↩ Tutorial12<br/>createDepthImage · render pass color + depth"]:::reused
    PLL["<b>Two pipeline layouts</b><br/>terrain: none (↩ Tutorial12)<br/>object: push constant mat4 model (↩ Tutorial16)"]:::reused
    PTER["↩ Tutorial12<br/>terrain pipeline: shader.21_terrain = Tutorial12's GLSL<br/>depth test + write"]:::reused
    POBJ["↩ Tutorial16<br/>object pipeline: shader.21_object = Tutorial16's GLSL<br/>depth test + write"]:::reused
    PSKY["<b>skybox pipeline</b><br/>object description again, depth <i>off</i><br/>(ALWAYS, no write)"]:::new
    GEO["<b>Geometry</b><br/>terrain VB: TerrainGenerator (↩ Tutorial12)<br/>skybox VB + IB: SkyboxGeometry (↩ Tutorial11)<br/>tank parts × 3 (↩ Tutorial16)"]:::reused
    FL["↩ VulkanCommon<br/>FrameLoop::create(depth view)"]:::shared
    DRAW(["<b>draw()</b><br/>write both UBOs from OrbitCamera<br/>1 skybox pipeline: push model · vkCmdDrawIndexed<br/>2 terrain pipeline: vkCmdDraw<br/>3 object pipeline: per part push model<br/>(placed via TerrainGenerator::heightAt) · vkCmdDraw"]):::frame

    BASE --> RC --> TEX --> UBO --> DESC --> DEPTH --> PLL --> PTER --> POBJ --> PSKY --> GEO --> FL --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create(depth view) | [VulkanCommon](vulkancommon.md) |
| createDepthImage · render pass color + depth | [Tutorial12](12_generated_terrain.md) |
| terrain pipeline: shader.21_terrain = Tutorial12's GLSL | [Tutorial12](12_generated_terrain.md) |
| object pipeline: shader.21_object = Tutorial16's GLSL | [Tutorial16](16_tank.md) |
| Three textures | [Tutorial11](11_skybox.md) |
| Geometry | [Tutorial12](12_generated_terrain.md) |

## What's new

- Three pipelines in one render pass with a shared depth buffer.
- The skybox reuses the object pipeline's description with depth testing
  turned off. At this world scale its far corners sat close enough to the
  far plane that depth tests failed intermittently.
- One object's placement comes from another's data: the tank's height is
  a `TerrainGenerator::heightAt()` query.

Source: `bin/21_world_scene/Tutorial21.cpp`
