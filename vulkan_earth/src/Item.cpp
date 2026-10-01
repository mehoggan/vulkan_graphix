#include "vulkan_earth/Item.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/ImageObject.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

using namespace std;

Item::Item() = default;
Item::~Item() = default;
/*GETTERS*/
std::int32_t Item::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
std::int32_t Item::getRemaining() { return remaining; }
std::string Item::getImageFileName() { return image_file_name; }
std::string Item::getDescription() { return description; }
std::int32_t Item::getPrice() { return price; }
std::int32_t Item::getPackageNum() { return package_num; }
std::int32_t Item::getMaxStack() { return max_stack; }
/*SETTERS*/
void Item::setRemaining(std::int32_t r) { remaining = r; }

void Item::playUseSFX() { playSFX(ITEM_USE1); }