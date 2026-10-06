#ifndef VULKAN_GRAPHIX_GAMECATALOG_H
#define VULKAN_GRAPHIX_GAMECATALOG_H

// The data behind vulkan_earth's shop: every item and weapon the game
// sells, exactly as their constructors (vulkan_earth/src/ItemXxx.cpp,
// WeaponXxx.cpp) set it - the one copy both the game's Item/Weapon classes
// and the tutorials that show them (Tutorial17, 19, 20) read.

#include <array>
#include <cstddef>
#include <cstdint>

namespace vulkan_graphix::GameCatalog {

// In the shop's own order (ShopMenu's shop_items[0..7]); the value is the
// item's id there.
enum class ItemKind : std::uint8_t {
  SmallRepair,
  BigRepair,
  AntiAcid,
  DoubleAction,
  Shield,
  ExtraBattery,
  Cloak,
  Float,
};
inline constexpr std::size_t c_item_count = 8;

struct ItemSpec {
  const char* m_image_file;
  const char* m_description;
  std::int32_t m_price;
  std::int32_t m_package_num;
  std::int32_t m_max_stack;
  std::int32_t m_remaining;
  // The strength of the item's effect (hit points healed, turns lasted,
  // ...), as each Item subclass's causeEffectToTank() reads it.
  std::int32_t m_special_num;
};

const ItemSpec& item(ItemKind kind);
// Every item, indexed by ItemKind.
const std::array<ItemSpec, c_item_count>& items();

// In the shop's own order (ShopMenu's shop_wpns[0..9]), the value being
// the weapon's id there, then Default (id 10): Projectile's fallback when
// no weapon is loaded, never sold.
enum class WeaponKind : std::uint8_t {
  MFB,
  BFB,
  Acid,
  Thor,
  EMP,
  Padlock,
  Revive,
  Teleport,
  Atom,
  Nuke,
  Default,
};
inline constexpr std::size_t c_weapon_count = 11;
// The weapons the shop sells: the first ten WeaponKinds.
inline constexpr std::size_t c_shop_weapon_count = 10;

struct WeaponSpec {
  const char* m_image_file;
  const char* m_description;
  std::int32_t m_price;
  std::int32_t m_package_num;
  std::int32_t m_max_stack;
  std::int32_t m_remaining;
  // The projectile model's scale in flight.
  float m_scale;
  float m_radius;
  std::int32_t m_damage;
  // The strength of the weapon's side effect (turns, hit points, ...).
  std::int32_t m_special_number;
  // The four colors its explosion cycles through, as the double literals
  // the game's OpenGLColors.h color names expand to (the Weapon classes
  // stored them as floats).
  std::array<std::array<double, 3>, 4> m_explosion_colors;
};

const WeaponSpec& weapon(WeaponKind kind);
// Every weapon, indexed by WeaponKind.
const std::array<WeaponSpec, c_weapon_count>& weapons();

}  // namespace vulkan_graphix::GameCatalog

#endif  // VULKAN_GRAPHIX_GAMECATALOG_H
