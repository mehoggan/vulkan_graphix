#ifndef ITEM_DOUBLE_ACTION_H
#define ITEM_DOUBLE_ACTION_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Item.h"

class ItemDoubleAction : public Item {
public:
    ItemDoubleAction();
    ItemDoubleAction(int id);
    ~ItemDoubleAction() override;
    ItemDoubleAction* getItemInstance() override;
    bool causeEffectToTank(Tank* tank) override;
};

#endif