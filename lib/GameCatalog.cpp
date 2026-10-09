#include "vulkan_graphix/GameCatalog.h"

#include "vulkan_graphix/Colors.h"

namespace vulkan_graphix::GameCatalog {

namespace {
using Colors::c_dark_purple;
using Colors::c_dark_slate_blue;
using Colors::c_dim_gray;
using Colors::c_light_gray;
using Colors::c_lime_green;
using Colors::c_medium_blue;
using Colors::c_orange;
using Colors::c_pale_green;
using Colors::c_quartz;
using Colors::c_red;
using Colors::c_sea_green;
using Colors::c_silver;
using Colors::c_violet;
using Colors::c_white;
using Colors::c_yellow;
// The game's old OpenGLColors.h defined MediumSlateBlue with only two
// components, so Thor's explosion - the one place it was used - really
// rendered with a zero blue. Kept as the game showed it, not as
// Colors::c_medium_slate_blue.
constexpr Math::Vec4<float> c_thor_medium_slate_blue(
    0.498039f, 1.0f, 0.0f, 1.0f);
}  // namespace

const std::array<ItemSpec, c_item_count>& items() {
  static const std::array<ItemSpec, c_item_count> data = {{
      {"ItemSmallRepair.raw",
       "Small Repair:     Heals 200 damage.",
       50,
       3,
       15,
       3,
       200},
      {"ItemBigRepair.raw",
       "Big Repair:     Heals 700 damage (uses 1 turn).",
       100,
       3,
       9,
       3,
       700},
      {"ItemAntiAcid.raw",
       "Anti-Acid:     Cures acid status.",
       70,
       3,
       12,
       3,
       0},
      {"ItemDoubleAction.raw",
       "Double Action:     Allows the player to perform an action twice "
       "in one turn.",
       120,
       1,
       5,
       1,
       1},
      {"ItemShield.raw",
       "Shield:     Neutralize the damage taken for 5 times (uses 1 "
       "turn).",
       150,
       1,
       5,
       1,
       5},
      {"ItemExtraBattery.raw",
       "Extra Battery:     Recovers from the damage of EMP.",
       50,
       3,
       9,
       3,
       0},
      {"ItemCloak.raw",
       "Cloak:     Makes the player's tank disappear (users 1 turn).",
       100,
       2,
       10,
       2,
       5},
      {"ItemFloat.raw",
       "Float:     Allows the player to float (uses 1 turn).",
       80,
       2,
       10,
       2,
       5},
  }};
  return data;
}

const ItemSpec& item(ItemKind kind) {
  return items()[static_cast<std::size_t>(kind)];
}

const std::array<WeaponSpec, c_weapon_count>& weapons() {
  static const std::array<WeaponSpec, c_weapon_count> data = {{
      {"WeaponMFB.raw",
       "MFB:     (Medium Force Bomb) Damage:300",
       60,
       2,
       12,
       2,
       60,
       15,
       300,
       0,
       {c_white, c_yellow, c_orange, c_red}},
      {"WeaponBFB.raw",
       "BFB:     (Big Force Bomb) Damage:400",
       100,
       2,
       6,
       2,
       100,
       30,
       400,
       0,
       {c_white, c_yellow, c_orange, c_red}},
      {"WeaponAcid.raw",
       "Acid:     Damage: 150, DOT: 10%% of total HP for 5 turns",
       100,
       2,
       8,
       2,
       60,
       7,
       150,
       5,
       {c_white, c_lime_green, c_pale_green, c_sea_green}},
      {"WeaponThor.raw",
       "Thor:     Damage: 200, Paralyze targets for 1 turn",
       80,
       1,
       6,
       1,
       70,
       6,
       200,
       1,
       {c_white, c_thor_medium_slate_blue, c_white, c_medium_blue}},
      {"WeaponEMP.raw",
       "EMP:     Disrupt tanks in the target area for 5 turns.",
       60,
       2,
       8,
       2,
       40,
       12,
       0,
       5,
       {c_white, c_silver, c_white, c_silver}},
      {"WeaponPadlock.raw",
       "Padlock:     Damage: 50, Locks target's inventory for 4 turns",
       40,
       2,
       8,
       2,
       60,
       7,
       50,
       4,
       {c_dim_gray, c_violet, c_dark_slate_blue, c_dark_purple}},
      {"WeaponRevive.raw",
       "Revive:     Revive/repair tanks in the target area",
       50,
       1,
       6,
       1,
       60,
       6,
       0,
       400,
       {c_white, c_silver, c_white, c_light_gray}},
      {"WeaponTeleport.raw",
       "Teleport:     Teleport to where the projectile lands on.",
       50,
       3,
       15,
       3,
       60,
       0,
       0,
       0,
       {c_white, c_silver, c_silver, c_quartz}},
      {"WeaponAtom.raw",
       "Atom:     Damage: 999, Very small radius.",
       200,
       1,
       5,
       1,
       30,
       1,
       999,
       0,
       {c_white, c_red, c_white, c_red}},
      {"WeaponNuke.raw",
       "Nuke:     Do NOT use this weapon!!",
       500,
       1,
       1,
       1,
       70,
       50,
       800,
       0,
       {c_white, c_silver, c_red, c_red}},
      {"TestImage.raw",
       "Default     (Default Bomb) Damage:100",
       60,
       2,
       12,
       2,
       60,
       5,
       100,
       0,
       {c_white, c_yellow, c_orange, c_red}},
  }};
  return data;
}

const WeaponSpec& weapon(WeaponKind kind) {
  return weapons()[static_cast<std::size_t>(kind)];
}

}  // namespace vulkan_graphix::GameCatalog
