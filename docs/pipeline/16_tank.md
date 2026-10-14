# Tutorial16 — Tank

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial13](13_projectile_model.md) and
[Tutorial10](10_spline_polyline.md)'s push constants · Next:
[Tutorial17](17_item_inventory.md)

vulkan_earth's "Hellfire" tank: three `.ogl` parts (body, head, turret)
sharing one texture, each placed by its own model matrix sent as a push
constant just before its draw.

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
    TEX["<b>loadRawTexture</b><br/>TestImage.raw 1024² · CLAMP_TO_EDGE"]:::new
    UBO["<b>createUniformBuffer</b> (changed)<br/>view + projection only<br/>(model moves to the push constant)"]:::changed
    DESC["↩ Tutorial11<br/>bindings: 0 sampler (frag), 1 UBO (vert)"]:::reused
    RP["↩ Tutorial13<br/>createRenderPass (color only, no depth)"]:::reused
    PLL["<b>createPipelineLayout</b> (changed)<br/>+ push constant: vertex stage, mat4 model<br/>↩ Tutorial10's push-constant technique"]:::changed
    PIPE["<b>createGraphicsPipeline</b> (changed)<br/>new vertex shader (applies push model)<br/>fragment shader = Tutorial11/13's<br/>position + texcoord"]:::changed
    PARTS["<b>createPart</b> × 3<br/>Hellfire_Body / _Head / _Turret.ogl<br/>↩ Tutorial13's loadOglTexturedMesh"]:::changed
    FL["↩ VulkanCommon<br/>FrameLoop::create"]:::shared
    DRAW(["<b>draw()</b><br/>write UBO from OrbitCamera<br/>bind pipeline + set once, then per part:<br/>bind VB · vkCmdPushConstants(model) · vkCmdDraw<br/>models from HellfireTank / TankPlacement"]):::frame

    BASE --> RC --> TEX --> UBO --> DESC --> RP --> PLL --> PIPE --> PARTS --> FL --> DRAW

    click BASE "./tutorialbase.md" "TutorialBase"
    click RC "./vulkancommon.md" "VulkanCommon"
    click FL "./vulkancommon.md" "VulkanCommon"
    click DESC "./11_skybox.md" "Tutorial11"
    click RP "./13_projectile_model.md" "Tutorial13"
    click PLL "./10_spline_polyline.md" "Tutorial10"
    click PARTS "./13_projectile_model.md" "Tutorial13"
```

**Linked steps:** [TutorialBase](tutorialbase.md);
[VulkanCommon](vulkancommon.md); [Tutorial10](10_spline_polyline.md)
(push constants); [Tutorial11](11_skybox.md) (bindings, fragment shader);
[Tutorial13](13_projectile_model.md) (render pass, `.ogl` loading).

## What's new

- Several meshes per frame, each with its own matrix. Tutorial10's push
  constant carried a color; this one carries a matrix.
- Hierarchical part placement: a child part's offset is rotated through
  its parent's basis (`TankPlacement`, `HellfireTank`). Tutorial18, 21,
  and 22 place the tank the same way.
- Its 3D pipeline is reused by [Tutorial19](19_weapons_projectiles.md),
  [Tutorial21](21_world_scene.md), and
  [Tutorial22](22_menu_tank_preview.md).

Source: `bin/16_tank/Tutorial16.cpp`
