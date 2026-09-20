#include "ItemDoubleAction.h"
#include "Item.h"
#include "macro_crtdbg.h"

ItemDoubleAction::ItemDoubleAction() = default;
ItemDoubleAction::ItemDoubleAction(int ID) {
    UNIQUEIDENTIFIER = ID;
    packageNum = 1;
    maxStack = 5;
    remaining = 1;
    imageFileName = "ItemDoubleAction.raw";
    description =
            "Double Action:     Allows the player to perform an action twice "
            "in one turn.";
    price = 120;
    specialNum = 1;
}
ItemDoubleAction::~ItemDoubleAction() = default;

ItemDoubleAction* ItemDoubleAction::getItemInstance() {
    return new ItemDoubleAction(UNIQUEIDENTIFIER);
}

bool ItemDoubleAction::causeEffectToTank(Tank* tank) {
    tank->setDurationDoubleAction(specialNum);
    return false;
}