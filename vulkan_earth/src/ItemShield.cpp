#include "Item.h"
#include "ItemShield.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemShield::ItemShield(){}
ItemShield::ItemShield(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->packageNum = 1;
	this->maxStack = 5;
	this->remaining = 1;
	this->imageFileName = "ItemShield.raw";
	this->description = "Shield:     Neutralize the damage taken for 5 times (uses 1 turn).";
	this->price = 150;
	this->specialNum = 5;
}
ItemShield::~ItemShield(){
}

ItemShield* ItemShield::getItemInstance(){
	return new ItemShield(this->UNIQUEIDENTIFIER);
}

bool ItemShield::causeEffectToTank(Tank* tank){
	tank->setDurationShield(specialNum);
	return true;
}

void ItemShield::playUseSFX(){
	playSFX(ITEM_USE_SHIELD);
}