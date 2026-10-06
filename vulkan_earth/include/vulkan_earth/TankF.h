#ifndef VULKAN_EARTH_TANKF_H
#define VULKAN_EARTH_TANKF_H

#include <cstdint>
#include "vulkan_earth/Tank.h"

class TankF : public Tank {
public:
  TankF();
  TankF(float x, float y, float z);
  ~TankF() override;

  std::int32_t getBaseHP() override;
  std::int32_t getBasePower() override;
  std::int32_t getBaseArmor() override;
  std::int32_t getBaseSpeed() override;
  std::string getName() override;
  void buildList();
};

#endif