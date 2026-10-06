#ifndef VULKAN_EARTH_ITEM_H
#define VULKAN_EARTH_ITEM_H

#include <cstdint>
#include <string>
#include "vulkan_earth/Tank.h"
#include "vulkan_graphix/GameCatalog.h"

class Item {
public:
  Item();
  virtual ~Item();
  /*	GETTERS AND SETTERS	*/
  virtual Item* getItemInstance() = 0;
  std::int32_t getUNIQUEIDENTIFIER();
  std::int32_t getRemaining();
  void setRemaining(std::int32_t r);
  std::string getImageFileName();
  std::string getDescription();
  std::int32_t getPrice();
  std::int32_t getPackageNum();
  std::int32_t getMaxStack();
  virtual bool causeEffectToTank(Tank* tank) = 0;
  virtual void playUseSFX();

protected:
  // Every field but the id, from the game's catalog.
  void loadSpec(const vulkan_graphix::GameCatalog::ItemSpec& spec);

  std::int32_t m_uniqueidentifier;
  std::string m_image_file_name;
  std::string m_description;
  std::int32_t m_price;
  std::int32_t m_package_num;
  std::int32_t m_max_stack;
  std::int32_t m_remaining;
  std::int32_t m_special_num;
};

#endif  // VULKAN_EARTH_ITEM_H