# Tutorial10 — Spline polyline

[Index](../vulkan_pipeline_breakdown.md) · Builds on:
[Tutorial09](09_tessellated_terrain.md) · Next:
[Tutorial11](11_skybox.md)

A Catmull-Rom spline sampled by `Math::CubicCurve` and drawn as a thick
line strip, with its control polygon drawn thinner from the same pipeline.
The first tutorial with line topology, push constants, and a dynamic line
width.

```mermaid
%%{init: {"flowchart": {"wrappingWidth": 400}}}%%
flowchart TD
    classDef reused fill:#eceff1,stroke:#607d8b,stroke-dasharray:4 3,color:#263238
    classDef changed fill:#fff3e0,stroke:#ef6c00,color:#3e2723
    classDef new fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20
    classDef frame fill:#f3e5f5,stroke:#6a1b9a,color:#311b92

    BASE["↩ TutorialBase<br/>prepareVulkan()<br/>(enables wideLines when supported)"]:::reused
    RR["↩ Tutorial04<br/>createRenderingResources"]:::reused
    STG["↩ Tutorial06<br/>createStagingBuffer"]:::reused
    DEPTH["↩ Tutorial08<br/>createDepthResources"]:::reused
    UBO["<b>createUniformBuffer</b> (changed)<br/>↩ Tutorial08 strategy<br/>view + projection only: no model, no lighting"]:::changed
    DSL["<b>createDescriptorSetLayout</b> (changed)<br/>binding 0: UNIFORM_BUFFER, <i>vertex only</i><br/>no sampler"]:::changed
    POOL["↩ Tutorial08<br/>createDescriptorPool + allocate + update<br/>1 uniform descriptor"]:::reused
    RP["↩ Tutorial08<br/>createRenderPass (color + depth)"]:::reused
    PLL["<b>createPipelineLayout</b> (changed)<br/>set layout<br/>+ push-constant range: fragment, vec4 color"]:::changed
    PIPE["<b>createLinePipeline</b><br/>position only · LINE_STRIP<br/>dynamic viewport, scissor, <i>LINE_WIDTH</i><br/>depth test on"]:::new
    CVB["<b>createCurveVertexBuffer</b><br/>CubicCurve CatmullRom<br/>computeSamplesAdaptive → polyline points<br/>device-local via staging"]:::new
    PVB["<b>createControlPolygonVertexBuffer</b><br/>the raw control points, in order"]:::new
    DRAW(["<b>draw()</b> (changed)<br/>↩ Tutorial09 loop + OrbitCamera<br/>curve: push color · vkCmdSetLineWidth(wide) · vkCmdDraw<br/>polygon: push color · vkCmdSetLineWidth(1) · vkCmdDraw<br/>no index buffer"]):::frame

    BASE --> RR --> STG --> DEPTH --> UBO --> DSL --> POOL --> RP --> PLL --> PIPE --> CVB --> PVB --> DRAW

    click BASE "./tutorialbase.md" "TutorialBase"
    click RR "./04_vertex_attributes.md" "Tutorial04"
    click STG "./06_descriptor_sets.md" "Tutorial06"
    click DEPTH "./08_phong_sphere.md" "Tutorial08"
    click POOL "./08_phong_sphere.md" "Tutorial08"
    click RP "./08_phong_sphere.md" "Tutorial08"
```

**Linked steps:** [TutorialBase](tutorialbase.md);
[Tutorial04](04_vertex_attributes.md) (rendering resources);
[Tutorial06](06_descriptor_sets.md) (staging);
[Tutorial08](08_phong_sphere.md) (depth, descriptors, render pass);
[Tutorial09](09_tessellated_terrain.md) (frame loop, orbit camera).

## What's new

- `VK_PRIMITIVE_TOPOLOGY_LINE_STRIP`.
- Push constants: one range on the layout, pushed twice per frame so one
  pipeline draws two colors.
- `VK_DYNAMIC_STATE_LINE_WIDTH`. `getLineWidth()` clamps the requested
  width to the device's `lineWidthRange`, or uses 1.0 without
  `wideLines`.
- Two draws per frame from two vertex buffers.

## Spec links

[`VkPushConstantRange`](https://registry.khronos.org/vulkan/specs/latest/man/html/VkPushConstantRange.html) ·
[`vkCmdPushConstants`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdPushConstants.html) ·
[`vkCmdSetLineWidth`](https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdSetLineWidth.html)

Source: `bin/10_spline_polyline/Tutorial10.cpp`
