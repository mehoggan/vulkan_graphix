#include "ItemAntiAcid.h"
#include "Item.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemAntiAcid::ItemAntiAcid() = default;
ItemAntiAcid::ItemAntiAcid(int ID) {
    UNIQUEIDENTIFIER = ID;
    packageNum = 3;
    maxStack = 12;
    remaining = 3;
    imageFileName = "ItemAntiAcid.raw";
    description = "Anti-Acid:     Cures acid status.";
    price = 70;
    specialNum = 0;
}
ItemAntiAcid::~ItemAntiAcid() = default;

ItemAntiAcid* ItemAntiAcid::getItemInstance() {
    return new ItemAntiAcid(UNIQUEIDENTIFIER);
}

bool ItemAntiAcid::causeEffectToTank(Tank* tank) {
    tank->setDurationAcid(specialNum);
    return false;
}

void ItemAntiAcid::playUseSFX() { playSFX(ITEM_USE3); }