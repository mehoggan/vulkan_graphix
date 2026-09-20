#include "Item.h"
#include "ItemDoubleAction.h"
#include "macro_crtdbg.h"

ItemDoubleAction::ItemDoubleAction(){}
ItemDoubleAction::ItemDoubleAction(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->packageNum = 1;
	this->maxStack = 5;
	this->remaining = 1;
	this->imageFileName = "ItemDoubleAction.raw";
	this->description = "Double Action:     Allows the player to perform an action twice in one turn.";
	this->price = 120;
	this->specialNum = 1;
}
ItemDoubleAction::~ItemDoubleAction(){
}

ItemDoubleAction* ItemDoubleAction::getItemInstance(){
	return new ItemDoubleAction(this->UNIQUEIDENTIFIER);
}

bool ItemDoubleAction::causeEffectToTank(Tank* tank){
	tank->setDurationDoubleAction(specialNum);
	return false;
}