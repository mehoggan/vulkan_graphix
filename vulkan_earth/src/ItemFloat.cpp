#include "Item.h"
#include "ItemFloat.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemFloat::ItemFloat(){}
ItemFloat::ItemFloat(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->packageNum = 2;
	this->maxStack = 10;
	this->remaining = 2;
	this->imageFileName = "ItemFloat.raw";
	this->description = "Float:     Allows the player to float (uses 1 turn).";
	this->price = 80;
	this->specialNum = 5;
}
ItemFloat::~ItemFloat(){
}

ItemFloat* ItemFloat::getItemInstance(){
	return new ItemFloat(this->UNIQUEIDENTIFIER);
}

bool ItemFloat::causeEffectToTank(Tank* tank){
	tank->setDurationFloat(specialNum+1);
	return true;
}

void ItemFloat::playUseSFX(){
	playSFX(ITEM_USE_FLOAT);
}