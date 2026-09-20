#include "Item.h"
#include "ItemAntiAcid.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemAntiAcid::ItemAntiAcid(){}
ItemAntiAcid::ItemAntiAcid(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->packageNum = 3;
	this->maxStack = 12;
	this->remaining = 3;
	this->imageFileName = "ItemAntiAcid.raw";
	this->description = "Anti-Acid:     Cures acid status.";
	this->price = 70;
	this->specialNum = 0;
}
ItemAntiAcid::~ItemAntiAcid(){
}

ItemAntiAcid* ItemAntiAcid::getItemInstance(){
	return new ItemAntiAcid(this->UNIQUEIDENTIFIER);
}

bool ItemAntiAcid::causeEffectToTank(Tank* tank){
	tank->setDurationAcid(specialNum);
	return false;
}

void ItemAntiAcid::playUseSFX(){
	playSFX(ITEM_USE3);
}