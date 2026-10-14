# Tutorial18 — Player tanks and HUD

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial16](16_tank.md) (3D) and [Tutorial15](15_text_and_button.md)
(HUD) · Next: [Tutorial19](19_weapons_projectiles.md)

Two Hellfire tanks, each tinted with a team color, plus a HUD overlay with
vulkan_earth's health and power bars. The first tutorial with two
pipelines in one render pass: a depth-tested 3D pass, then a 2D overlay
with depth testing off.

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
    TTEX["↩ Tutorial16<br/>tank texture (TestImage.raw)"]:::reused
    FONT["↩ Tutorial15<br/>font atlas texture"]:::reused
    UBO["<b>Two uniform buffers</b><br/>3D: view + projection (↩ Tutorial16)<br/>HUD: orthographic mat4 (↩ Tutorial15)"]:::changed
    DESC["<b>Two set layouts, one pool of 2 sets</b><br/>3D set ↩ Tutorial16's bindings<br/>HUD set ↩ Tutorial15's bindings"]:::changed
    DEPTH["↩ Tutorial12<br/>createDepthImage · render pass color + depth"]:::reused
    PLL["<b>Two pipeline layouts</b><br/>3D: push constant mat4 model + vec4 tint<br/>(vertex + fragment stages)<br/>HUD: no push constants"]:::changed
    P3D["<b>3D pipeline</b><br/>shader.18_3d: Tutorial16's, plus the tint<br/>depth test + write"]:::changed
    PHUD["<b>HUD pipeline</b><br/>shader.18_hud = Tutorial15's GLSL<br/>depth off (ALWAYS, no write) · alpha blend"]:::new
    PARTS["↩ Tutorial16<br/>createPart × 3 (Hellfire meshes)"]:::reused
    HUDVB["<b>HUD vertex buffer</b><br/>buildHudVertexData: bars + labels<br/>device-local, built once"]:::new
    FL["↩ VulkanCommon<br/>FrameLoop::create(depth view)"]:::shared
    DRAW(["<b>draw()</b><br/>write 3D UBO from OrbitCamera · clear color + depth<br/>3D pipeline: per player × per part:<br/>push model + tint · vkCmdDraw<br/>HUD pipeline: bind HUD set + VB · vkCmdDraw"]):::frame

    BASE --> RC --> TTEX --> FONT --> UBO --> DESC --> DEPTH --> PLL --> P3D --> PHUD --> PARTS --> HUDVB --> FL --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create(depth view) | [VulkanCommon](vulkancommon.md) |
| tank texture (TestImage.raw) | [Tutorial16](16_tank.md) |
| font atlas texture | [Tutorial15](15_text_and_button.md) |
| createDepthImage · render pass color + depth | [Tutorial12](12_generated_terrain.md) |
| createPart × 3 (Hellfire meshes) | [Tutorial16](16_tank.md) |

## What's new

- Two pipelines and two pipeline layouts in one render pass, switched
  mid-frame.
- A depth-disabled overlay pipeline (`DepthState{false, false, ALWAYS}`)
  drawn after the 3D pass.
- A push constant used by both shader stages (model for the vertex
  stage, tint for the fragment stage).
- HUD bar colors from `UiGeometry::healthBarColor()` /
  `powerBarColor()`, shared with vulkan_earth.

Source: `bin/18_player_tanks_hud/Tutorial18.cpp`
