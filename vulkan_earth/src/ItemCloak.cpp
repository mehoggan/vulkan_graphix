#include "Item.h"
#include "ItemCloak.h"
#include "macro_crtdbg.h"

ItemCloak::ItemCloak(){}
ItemCloak::ItemCloak(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->packageNum = 2;
	this->maxStack = 10;
	this->remaining = 2;
	this->imageFileName = "ItemCloak.raw";
	this->description = "Cloak:     Makes the player's tank disappear (users 1 turn).";
	this->price = 100;
	this->specialNum = 5;
}
ItemCloak::~ItemCloak(){
}

ItemCloak* ItemCloak::getItemInstance(){
	return new ItemCloak(this->UNIQUEIDENTIFIER);
}

bool ItemCloak::causeEffectToTank(Tank* tank){
	tank->setDurationCloak(specialNum+1);
	return true;
}