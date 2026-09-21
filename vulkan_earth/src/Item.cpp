#include "Item.h"
#include <string>
#include "ImageObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

using namespace std;

Item::Item() = default;
Item::~Item() = default;
/*GETTERS*/
int Item::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
int Item::getRemaining() { return remaining; }
std::string Item::getImageFileName() { return image_file_name; }
std::string Item::getDescription() { return description; }
int Item::getPrice() { return price; }
int Item::getPackageNum() { return package_num; }
int Item::getMaxStack() { return max_stack; }
/*SETTERS*/
void Item::setRemaining(int r) { remaining = r; }

void Item::playUseSFX() { playSFX(ITEM_USE1); }