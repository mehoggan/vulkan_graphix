#include "vulkan_earth/ItemFloat.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"

extern void playSFX(std::int32_t sfx);

ItemFloat::ItemFloat() = default;
ItemFloat::ItemFloat(std::int32_t id) {
  m_uniqueidentifier = id;
  loadSpec(
      vulkan_graphix::GameCatalog::item(
          vulkan_graphix::GameCatalog::ItemKind::Float));
}
ItemFloat::~ItemFloat() = default;

ItemFloat* ItemFloat::getItemInstance() {
  return new ItemFloat(m_uniqueidentifier);
}

bool ItemFloat::causeEffectToTank(Tank* tank) {
  tank->setDurationFloat(m_special_num + 1);
  return true;
}

void ItemFloat::playUseSFX() { playSFX(ITEM_USE_FLOAT); }