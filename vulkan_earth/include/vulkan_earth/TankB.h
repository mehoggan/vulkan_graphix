#ifndef VULKAN_EARTH_TANKB_H
#define VULKAN_EARTH_TANKB_H

#include <cstdint>
#include "vulkan_earth/Tank.h"

class TankB : public Tank {
public:
  TankB();
  TankB(float x, float y, float z);
  ~TankB() override;

  std::int32_t getBaseHP() override;
  std::int32_t getBasePower() override;
  std::int32_t getBaseArmor() override;
  std::int32_t getBaseSpeed() override;
  std::string getName() override;
  void buildList();
};

#endif