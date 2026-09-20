#include "Item.h"
#include "ItemExtraBattery.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemExtraBattery::ItemExtraBattery(){}
ItemExtraBattery::ItemExtraBattery(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->packageNum = 3;
	this->maxStack = 9;
	this->remaining = 3;
	this->imageFileName = "ItemExtraBattery.raw";
	this->description = "Extra Battery:     Recovers from the damage of EMP.";
	this->price = 50;
	this->specialNum = 0;
}
ItemExtraBattery::~ItemExtraBattery(){
}

ItemExtraBattery* ItemExtraBattery::getItemInstance(){
	return new ItemExtraBattery(this->UNIQUEIDENTIFIER);
}

bool ItemExtraBattery::causeEffectToTank(Tank* tank){
	tank->setDurationEMP(specialNum);
	return false;
}

void ItemExtraBattery::playUseSFX(){
	playSFX(ITEM_USE3);
}