#ifndef VULKAN_EARTH_ITEMSMALLREPAIR_H
#define VULKAN_EARTH_ITEMSMALLREPAIR_H

#include <cstdint>
#include "vulkan_earth/Item.h"

class ItemSmallRepair : public Item {
public:
  ItemSmallRepair();
  ItemSmallRepair(std::int32_t id);
  ~ItemSmallRepair() override;
  ItemSmallRepair* getItemInstance() override;
  bool causeEffectToTank(Tank* tank) override;
  void playUseSFX() override;
};

#endif