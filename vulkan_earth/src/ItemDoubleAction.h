#ifndef ITEM_DOUBLE_ACTION_H
#define ITEM_DOUBLE_ACTION_H

#include <GL/freeglut.h>

class ItemDoubleAction : public Item{
public:
	ItemDoubleAction();
	ItemDoubleAction(int ID);
	~ItemDoubleAction();
	ItemDoubleAction* getItemInstance();
	bool causeEffectToTank(Tank* tank);
};

#endif