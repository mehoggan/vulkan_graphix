#include "vulkan_earth/ItemDoubleAction.h"
#include <cstdint>
#include "vulkan_earth/MacroCrtdbg.h"

ItemDoubleAction::ItemDoubleAction() = default;
ItemDoubleAction::ItemDoubleAction(std::int32_t id) {
  m_uniqueidentifier = id;
  loadSpec(vulkan_graphix::GameCatalog::item(
      vulkan_graphix::GameCatalog::ItemKind::DoubleAction));
}
ItemDoubleAction::~ItemDoubleAction() = default;

ItemDoubleAction* ItemDoubleAction::getItemInstance() {
  return new ItemDoubleAction(m_uniqueidentifier);
}

bool ItemDoubleAction::causeEffectToTank(Tank* tank) {
  tank->setDurationDoubleAction(m_special_num);
  return false;
}