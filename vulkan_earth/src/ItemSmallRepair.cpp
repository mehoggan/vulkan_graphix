#include "Item.h"
#include "ItemSmallRepair.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemSmallRepair::ItemSmallRepair(){}
ItemSmallRepair::ItemSmallRepair(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->packageNum = 3;
	this->maxStack = 15;
	this->remaining = 3;
	this->imageFileName = "ItemSmallRepair.raw";
	this->description = "Small Repair:     Heals 200 damage.";
	this->price = 50;
	this->specialNum = 200;
}
ItemSmallRepair::~ItemSmallRepair(){
}

ItemSmallRepair* ItemSmallRepair::getItemInstance(){
	return new ItemSmallRepair(this->UNIQUEIDENTIFIER);
}

bool ItemSmallRepair::causeEffectToTank(Tank* tank){
	tank->setHP(tank->getHP()+specialNum);
	if(tank->getHP() > tank->getArmor()*100){
		tank->setHP(tank->getArmor()*100);
	}
	return false;
}

void ItemSmallRepair::playUseSFX(){
	playSFX(ITEM_USE3);
}