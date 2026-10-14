# Tutorial15 — Text and a button

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial07](07_uniform_buffers.md)'s orthographic idea, on
[VulkanCommon](vulkancommon.md) · Next: [Tutorial16](16_tank.md)

A title and one clickable bevel button with a text label: the two pieces
vulkan_earth's whole menu UI is built from. Text comes from a `BitmapFont`
glyph atlas; the vertex buffer is rebuilt from UI state every frame.

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
    FONT["<b>BitmapFont::load</b><br/>DejaVu TrueType → glyph atlas (STBTrueType)<br/><b>createTexture</b>(atlas pixels) · CLAMP_TO_EDGE"]:::new
    UBO["<b>createUniformBuffer</b><br/>one mat4: getOrthographicProjectionMatrix<br/>↩ Tutorial07's pixel-space idea"]:::changed
    DESC["<b>Descriptors</b><br/>binding 0: UBO (vert) · binding 1: sampler (frag)"]:::new
    RP["<b>createRenderPass</b><br/>color only"]:::shared
    PIPE["<b>createGraphicsPipeline</b><br/>shader.15 · position vec4 + texcoord vec2 + color vec4<br/>alphaBlend() (↩ Tutorial14) · cull NONE"]:::new
    VB["<b>createHostVisibleBuffer</b><br/>room for c_max_vertex_count UI vertices<br/>updateVertexBufferData() fills it"]:::new
    FL["↩ VulkanCommon<br/>FrameLoop::create"]:::shared
    DRAW(["<b>draw()</b><br/>vkDeviceWaitIdle<br/>rebuild quads: UiGeometry::buildButtonBevel<br/>+ glyph quads → writeBuffer<br/>bind · vkCmdDraw(vertex count)"]):::frame
    INPUT["<b>onMouseButton / onMouseMove</b><br/>hover + press state → raised or pressed bevel"]:::new

    BASE --> RC --> FONT --> UBO --> DESC --> RP --> PIPE --> VB --> FL --> DRAW
    INPUT -. mouse .-> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| createRenderPass | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create | [VulkanCommon](vulkancommon.md) |
| createUniformBuffer | [Tutorial07](07_uniform_buffers.md) |

Also builds on: [Tutorial14](14_particle_sphere.md).

## What's new

- A texture built from pixels in memory (the font atlas) rather than a
  file.
- A host-visible vertex buffer rewritten every frame from CPU-side state,
  instead of a device-local one uploaded once.
- Its shaders and pipeline become the 2D UI pipeline for
  [Tutorial17](17_item_inventory.md), [Tutorial18](18_player_tanks_hud.md)'s
  HUD, and [Tutorial22](22_menu_tank_preview.md)'s panel.

Source: `bin/15_text_and_button/Tutorial15.cpp`
