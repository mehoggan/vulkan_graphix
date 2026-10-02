#ifndef VULKAN_EARTH_RENDER_UI_BUILDERS_H
#define VULKAN_EARTH_RENDER_UI_BUILDERS_H

// Small helpers the ported UI classes share while turning their former
// glBegin()/glEnd() code into UiMesh geometry, in the game's own menu
// space: pixels around the window's center, y up, with a per-control z.

#include "vulkan_earth/render/Mesh.h"
#include "vulkan_earth/render/RenderTypes.h"

namespace vulkan_earth::render {

// The raised/pressed beveled rectangle the game drew for every button,
// panel, and dialog (ControlItemButton::draw(), LoadingScreen::draw(),
// ...): top_left is (x, y) with y up, the face extends width right and
// height down, at depth z. Built by libvulkan_graphix's
// UiGeometry::buildButtonBevel() - the one shared copy - and mirrored
// into this y-up space.
void appendBevel(UiMesh& mesh,
                 float x,
                 float y,
                 float z,
                 float width,
                 float height,
                 Vec4 const& color,
                 bool pressed,
                 float bevel_size = 3.0f);

// The same five-quad frame shape with explicitly chosen colors, for the
// game's other bevel schemes (e.g. the sunken -0.2/+0.4 frame its
// selection boxes, check boxes, and sliders use): top wedge and left wedge
// in top_left, the face, then bottom and right wedges in bottom_right -
// the original draw order.
void appendFrame(UiMesh& mesh,
                 float x,
                 float y,
                 float z,
                 float width,
                 float height,
                 Vec4 const& top_left,
                 Vec4 const& face,
                 Vec4 const& bottom_right,
                 float border = 3.0f);

// The whole-window background panel MainMenu, ReadyMenu, and ShopMenu
// all open with: centered on the origin, a percent_border * height border
// in five shades of gray (left 0.85, top 0.80, face 0.75, bottom 0.45,
// right 0.40), each quad's corners going upper left -> lower left -> lower
// right -> upper right, at z = 0.
void appendMenuPanel(UiMesh& mesh,
                     float width,
                     float height,
                     float percent_border);

// A GL_QUADS quad (corners in drawing order).
void appendQuad(UiMesh& mesh,
                Vec3 const& v0,
                Vec3 const& v1,
                Vec3 const& v2,
                Vec3 const& v3,
                Vec4 const& color);

}  // namespace vulkan_earth::render

#endif  // VULKAN_EARTH_RENDER_UI_BUILDERS_H
