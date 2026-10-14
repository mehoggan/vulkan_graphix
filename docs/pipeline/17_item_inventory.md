# Tutorial17 — Item inventory

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial15](15_text_and_button.md) · Next:
[Tutorial18](18_player_tanks_hud.md)

A titled bevel panel holding a grid of vulkan_earth's 8 items. Click a
cell to read that item's description. Two textures (the font atlas and an
icon atlas) share one pipeline by using two descriptor sets from the same
layout.

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
    FONT["↩ Tutorial15<br/>BitmapFont atlas → createTexture"]:::reused
    ICON["<b>createIconAtlas</b><br/>ImageAtlas: 8 ItemXxx.raw tiles<br/>→ createTexture"]:::new
    UBO["↩ Tutorial15<br/>orthographic mat4 UBO"]:::reused
    DESC["<b>Descriptors</b> (changed)<br/>↩ Tutorial15's layout: 0 UBO, 1 sampler<br/>pool sized for <i>2 sets</i><br/>font set + icon set, same UBO in both"]:::changed
    RP["↩ Tutorial15<br/>createRenderPass (color only)"]:::reused
    PIPE["↩ Tutorial15<br/>shader.17 = Tutorial15's GLSL<br/>alpha blend"]:::reused
    VB["↩ Tutorial15<br/>createHostVisibleBuffer<br/>text + bevel quads first, icon quads after"]:::reused
    FL["↩ VulkanCommon<br/>FrameLoop::create"]:::shared
    DRAW(["<b>draw()</b> (changed)<br/>rebuild quads: panel + GridLayout cells<br/>+ wrapped description text<br/>pass 1: bind font set · draw text/flat quads<br/>pass 2: bind icon set + offset · draw icons"]):::frame

    BASE --> RC --> FONT --> ICON --> UBO --> DESC --> RP --> PIPE --> VB --> FL --> DRAW
```

**Where the reused boxes come from:**

| Box | Defined in |
|---|---|
| prepareVulkan() | [TutorialBase](tutorialbase.md) |
| ResourceContext::initialize | [VulkanCommon](vulkancommon.md) |
| FrameLoop::create | [VulkanCommon](vulkancommon.md) |
| BitmapFont atlas → createTexture | [Tutorial15](15_text_and_button.md) |
| orthographic mat4 UBO | [Tutorial15](15_text_and_button.md) |
| createRenderPass (color only) | [Tutorial15](15_text_and_button.md) |
| shader.17 = Tutorial15's GLSL | [Tutorial15](15_text_and_button.md) |
| createHostVisibleBuffer | [Tutorial15](15_text_and_button.md) |

## What's new

- Two descriptor sets allocated from one layout, rebound between draws.
  This is how later tutorials switch textures without a second pipeline.
- Two draws from one vertex buffer at different offsets.
- `ImageAtlas`, `GridLayout`, and `wrapText()` from `lib/`; item data from
  `GameCatalog`.

Source: `bin/17_item_inventory/Tutorial17.cpp`
