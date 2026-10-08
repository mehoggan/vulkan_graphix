#include "vulkan_earth/ItemAntiAcid.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"

extern void playSFX(std::int32_t sfx);

ItemAntiAcid::ItemAntiAcid() = default;
ItemAntiAcid::ItemAntiAcid(std::int32_t id) {
  m_uniqueidentifier = id;
  loadSpec(vulkan_graphix::GameCatalog::item(
      vulkan_graphix::GameCatalog::ItemKind::AntiAcid));
}
ItemAntiAcid::~ItemAntiAcid() = default;

ItemAntiAcid* ItemAntiAcid::getItemInstance() {
  return new ItemAntiAcid(m_uniqueidentifier);
}

bool ItemAntiAcid::causeEffectToTank(Tank* tank) {
  tank->setDurationAcid(m_special_num);
  return false;
}

void ItemAntiAcid::playUseSFX() { playSFX(ITEM_USE3); }