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
int Item::getUNIQUEIDENTIFIER() { return UNIQUEIDENTIFIER; }
int Item::getRemaining() { return remaining; }
std::string Item::getImageFileName() { return imageFileName; }
std::string Item::getDescription() { return description; }
int Item::getPrice() { return price; }
int Item::getPackageNum() { return packageNum; }
int Item::getMaxStack() { return maxStack; }
/*SETTERS*/
void Item::setRemaining(int r) { remaining = r; }

void Item::playUseSFX() { playSFX(ITEM_USE1); }