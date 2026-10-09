#ifndef VULKAN_GRAPHIX_COLORS_H
#define VULKAN_GRAPHIX_COLORS_H

// Named RGBA colors (opaque - see withAlpha()), the classic GL color table
// vulkan_earth's OpenGLColors.h carried as "r, g, b" macros. That table
// had four broken entries - MediumSlateBlue and MediumSpringGreen lost a
// component, SlateBlue and SpringGreen had a stray "color" token - fixed
// here to their standard values. (GameCatalog keeps the two-component
// MediumSlateBlue the game actually rendered as Thor's explosion color.)

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Colors {

inline constexpr Math::Vec4<float> c_blue(0.0f, 0.0f, 1.0f, 1.0f);
inline constexpr Math::Vec4<float> c_dim_gray(
    0.329412f, 0.329412f, 0.329412f, 1.0f);
inline constexpr Math::Vec4<float> c_gray(
    0.752941f, 0.752941f, 0.752941f, 1.0f);
inline constexpr Math::Vec4<float> c_light_gray(
    0.658824f, 0.658824f, 0.658824f, 1.0f);
inline constexpr Math::Vec4<float> c_vlight_gray(0.80f, 0.80f, 0.80f, 1.0f);
inline constexpr Math::Vec4<float> c_aquamarine(
    0.439216f, 0.858824f, 0.576471f, 1.0f);
inline constexpr Math::Vec4<float> c_violet(
    0.62352f, 0.372549f, 0.623529f, 1.0f);
inline constexpr Math::Vec4<float> c_brown(
    0.647059f, 0.164706f, 0.164706f, 1.0f);
inline constexpr Math::Vec4<float> c_cadet_blue(
    0.372549f, 0.623529f, 0.623529f, 1.0f);
inline constexpr Math::Vec4<float> c_coral(1.0f, 0.498039f, 0.0f, 1.0f);
inline constexpr Math::Vec4<float> c_cornflower_blue(
    0.258824f, 0.258824f, 0.435294f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_green(
    0.184314f, 0.309804f, 0.184314f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_olive_green(
    0.309804f, 0.309804f, 0.184314f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_orchid(0.6f, 0.196078f, 0.8f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_slate_blue(
    0.419608f, 0.137255f, 0.556863f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_slate_gray(
    0.184314f, 0.309804f, 0.309804f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_turquoise(
    0.439216f, 0.576471f, 0.858824f, 1.0f);
inline constexpr Math::Vec4<float> c_firebrick(
    0.556863f, 0.137255f, 0.137255f, 1.0f);
inline constexpr Math::Vec4<float> c_forest_green(
    0.137255f, 0.556863f, 0.137255f, 1.0f);
inline constexpr Math::Vec4<float> c_gold(0.8f, 0.498039f, 0.196078f, 1.0f);
inline constexpr Math::Vec4<float> c_goldenrod(
    0.858824f, 0.858824f, 0.439216f, 1.0f);
inline constexpr Math::Vec4<float> c_green(0.0f, 1.0f, 0.0f, 1.0f);
inline constexpr Math::Vec4<float> c_yellow(1.0f, 1.0f, 0.0f, 1.0f);
inline constexpr Math::Vec4<float> c_indian_red(
    0.309804f, 0.184314f, 0.184314f, 1.0f);
inline constexpr Math::Vec4<float> c_khaki(
    0.623529f, 0.623529f, 0.372549f, 1.0f);
inline constexpr Math::Vec4<float> c_light_blue(
    0.74902f, 0.847059f, 0.847059f, 1.0f);
inline constexpr Math::Vec4<float> c_light_steel_blue(
    0.560784f, 0.560784f, 0.737255f, 1.0f);
inline constexpr Math::Vec4<float> c_lime_green(
    0.196078f, 0.8f, 0.196078f, 1.0f);
inline constexpr Math::Vec4<float> c_maroon(
    0.556863f, 0.137255f, 0.419608f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_aquamarine(
    0.196078f, 0.8f, 0.6f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_blue(
    0.196078f, 0.196078f, 0.8f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_forest_green(
    0.419608f, 0.556863f, 0.137255f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_goldenrod(
    0.917647f, 0.917647f, 0.678431f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_orchid(
    0.576471f, 0.439216f, 0.858824f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_sea_green(
    0.258824f, 0.435294f, 0.258824f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_slate_blue(
    0.498039f, 0.0f, 1.0f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_spring_green(
    0.498039f, 1.0f, 0.0f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_turquoise(
    0.439216f, 0.858824f, 0.858824f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_violet_red(
    0.858824f, 0.439216f, 0.576471f, 1.0f);
inline constexpr Math::Vec4<float> c_midnight_blue(
    0.184314f, 0.184314f, 0.309804f, 1.0f);
inline constexpr Math::Vec4<float> c_navy(
    0.137255f, 0.137255f, 0.556863f, 1.0f);
inline constexpr Math::Vec4<float> c_orange(1.0f, 0.5f, 0.0f, 1.0f);
inline constexpr Math::Vec4<float> c_orchid(
    0.858824f, 0.439216f, 0.858824f, 1.0f);
inline constexpr Math::Vec4<float> c_pale_green(
    0.560784f, 0.737255f, 0.560784f, 1.0f);
inline constexpr Math::Vec4<float> c_pink(
    0.737255f, 0.560784f, 0.560784f, 1.0f);
inline constexpr Math::Vec4<float> c_plum(
    0.917647f, 0.678431f, 0.917647f, 1.0f);
inline constexpr Math::Vec4<float> c_red(1.0f, 0.0f, 0.0f, 1.0f);
inline constexpr Math::Vec4<float> c_salmon(
    0.435294f, 0.258824f, 0.258824f, 1.0f);
inline constexpr Math::Vec4<float> c_sea_green(
    0.137255f, 0.556863f, 0.419608f, 1.0f);
inline constexpr Math::Vec4<float> c_sienna(
    0.556863f, 0.419608f, 0.137255f, 1.0f);
inline constexpr Math::Vec4<float> c_sky_blue(0.196078f, 0.6f, 0.8f, 1.0f);
inline constexpr Math::Vec4<float> c_slate_blue(0.0f, 0.498039f, 1.0f, 1.0f);
inline constexpr Math::Vec4<float> c_spring_green(0.0f, 1.0f, 0.498039f, 1.0f);
inline constexpr Math::Vec4<float> c_steel_blue(
    0.137255f, 0.419608f, 0.556863f, 1.0f);
inline constexpr Math::Vec4<float> c_tan(
    0.858824f, 0.576471f, 0.439216f, 1.0f);
inline constexpr Math::Vec4<float> c_thistle(
    0.847059f, 0.74902f, 0.847059f, 1.0f);
inline constexpr Math::Vec4<float> c_turquoise(
    0.678431f, 0.917647f, 0.917647f, 1.0f);
inline constexpr Math::Vec4<float> c_violet_red(0.8f, 0.196078f, 0.6f, 1.0f);
inline constexpr Math::Vec4<float> c_wheat(
    0.847059f, 0.847059f, 0.74902f, 1.0f);
inline constexpr Math::Vec4<float> c_yellow_green(0.6f, 0.8f, 0.196078f, 1.0f);
inline constexpr Math::Vec4<float> c_summer_sky(0.22f, 0.69f, 0.87f, 1.0f);
inline constexpr Math::Vec4<float> c_rich_blue(0.35f, 0.35f, 0.67f, 1.0f);
inline constexpr Math::Vec4<float> c_brass(0.71f, 0.65f, 0.26f, 1.0f);
inline constexpr Math::Vec4<float> c_copper(0.72f, 0.45f, 0.20f, 1.0f);
inline constexpr Math::Vec4<float> c_bronze(0.55f, 0.47f, 0.14f, 1.0f);
inline constexpr Math::Vec4<float> c_bronze_2(0.65f, 0.49f, 0.24f, 1.0f);
inline constexpr Math::Vec4<float> c_silver(0.90f, 0.91f, 0.98f, 1.0f);
inline constexpr Math::Vec4<float> c_bright_gold(0.85f, 0.85f, 0.10f, 1.0f);
inline constexpr Math::Vec4<float> c_old_gold(0.81f, 0.71f, 0.23f, 1.0f);
inline constexpr Math::Vec4<float> c_feldspar(0.82f, 0.57f, 0.46f, 1.0f);
inline constexpr Math::Vec4<float> c_quartz(0.85f, 0.85f, 0.95f, 1.0f);
inline constexpr Math::Vec4<float> c_neon_pink(1.00f, 0.43f, 0.78f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_purple(0.53f, 0.12f, 0.47f, 1.0f);
inline constexpr Math::Vec4<float> c_neon_blue(0.30f, 0.30f, 1.00f, 1.0f);
inline constexpr Math::Vec4<float> c_cool_copper(0.85f, 0.53f, 0.10f, 1.0f);
inline constexpr Math::Vec4<float> c_mandarin_orange(
    0.89f, 0.47f, 0.20f, 1.0f);
inline constexpr Math::Vec4<float> c_light_wood(0.91f, 0.76f, 0.65f, 1.0f);
inline constexpr Math::Vec4<float> c_medium_wood(0.65f, 0.50f, 0.39f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_wood(0.52f, 0.37f, 0.26f, 1.0f);
inline constexpr Math::Vec4<float> c_spicy_pink(1.00f, 0.11f, 0.68f, 1.0f);
inline constexpr Math::Vec4<float> c_semi_sweet_choc(
    0.42f, 0.26f, 0.15f, 1.0f);
inline constexpr Math::Vec4<float> c_bakers_choc(0.36f, 0.20f, 0.09f, 1.0f);
inline constexpr Math::Vec4<float> c_flesh(0.96f, 0.80f, 0.69f, 1.0f);
inline constexpr Math::Vec4<float> c_new_tan(0.92f, 0.78f, 0.62f, 1.0f);
inline constexpr Math::Vec4<float> c_new_midnight_blue(
    0.00f, 0.00f, 0.61f, 1.0f);
inline constexpr Math::Vec4<float> c_very_dark_brown(
    0.35f, 0.16f, 0.14f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_brown(0.36f, 0.25f, 0.20f, 1.0f);
inline constexpr Math::Vec4<float> c_dark_tan(0.59f, 0.41f, 0.31f, 1.0f);
inline constexpr Math::Vec4<float> c_dk_green_copper(
    0.29f, 0.46f, 0.43f, 1.0f);
inline constexpr Math::Vec4<float> c_dusty_rose(0.52f, 0.39f, 0.39f, 1.0f);
inline constexpr Math::Vec4<float> c_hunters_green(0.13f, 0.37f, 0.31f, 1.0f);
inline constexpr Math::Vec4<float> c_scarlet(0.55f, 0.09f, 0.09f, 1.0f);
inline constexpr Math::Vec4<float> c_med_purple(0.73f, 0.16f, 0.96f, 1.0f);
inline constexpr Math::Vec4<float> c_light_purple(0.87f, 0.58f, 0.98f, 1.0f);
inline constexpr Math::Vec4<float> c_very_light_purple(
    0.94f, 0.81f, 0.99f, 1.0f);
inline constexpr Math::Vec4<float> c_white(1.00f, 1.00f, 1.00f, 1.0f);

// color with its alpha replaced.
constexpr Math::Vec4<float> withAlpha(
    const Math::Vec4<float>& color, float alpha) {
  return Math::Vec4<float>(color.x, color.y, color.z, alpha);
}

}  // namespace vulkan_graphix::Colors

#endif  // VULKAN_GRAPHIX_COLORS_H
