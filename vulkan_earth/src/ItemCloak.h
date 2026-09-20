#ifndef ITEM_CLOAK_H
#define ITEM_CLOAK_H

#include <GL/freeglut.h>

class ItemCloak : public Item{
public:
	ItemCloak();
	ItemCloak(int ID);
	~ItemCloak();
	ItemCloak* getItemInstance();
	bool causeEffectToTank(Tank* tank);
};

#endif