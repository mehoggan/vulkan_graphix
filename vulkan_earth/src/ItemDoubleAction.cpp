#include "ItemDoubleAction.h"
#include "Item.h"
#include "MacroCrtdbg.h"

ItemDoubleAction::ItemDoubleAction() = default;
ItemDoubleAction::ItemDoubleAction(int id) {
    uniqueidentifier = id;
    package_num = 1;
    max_stack = 5;
    remaining = 1;
    image_file_name = "ItemDoubleAction.raw";
    description =
            "Double Action:     Allows the player to perform an action twice "
            "in one turn.";
    price = 120;
    special_num = 1;
}
ItemDoubleAction::~ItemDoubleAction() = default;

ItemDoubleAction* ItemDoubleAction::getItemInstance() {
    return new ItemDoubleAction(uniqueidentifier);
}

bool ItemDoubleAction::causeEffectToTank(Tank* tank) {
    tank->setDurationDoubleAction(special_num);
    return false;
}