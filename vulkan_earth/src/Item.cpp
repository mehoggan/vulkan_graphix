#include "vulkan_earth/Item.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

using namespace std;

Item::Item() = default;
Item::~Item() = default;
/*GETTERS*/
std::int32_t Item::getUNIQUEIDENTIFIER() { return m_uniqueidentifier; }
std::int32_t Item::getRemaining() { return m_remaining; }
std::string Item::getImageFileName() { return m_image_file_name; }
std::string Item::getDescription() { return m_description; }
std::int32_t Item::getPrice() { return m_price; }
std::int32_t Item::getPackageNum() { return m_package_num; }
std::int32_t Item::getMaxStack() { return m_max_stack; }
/*SETTERS*/
void Item::setRemaining(std::int32_t r) { m_remaining = r; }

void Item::playUseSFX() { playSFX(ITEM_USE1); }

void Item::loadSpec(const vulkan_graphix::GameCatalog::ItemSpec& spec) {
  m_package_num = spec.m_package_num;
  m_max_stack = spec.m_max_stack;
  m_remaining = spec.m_remaining;
  m_image_file_name = spec.m_image_file;
  m_description = spec.m_description;
  m_price = spec.m_price;
  m_special_num = spec.m_special_num;
}
