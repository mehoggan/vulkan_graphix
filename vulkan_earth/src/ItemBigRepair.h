#ifndef ITEM_BIG_REPAIR_H
#define ITEM_BIG_REPAIR_H

#include <GL/freeglut.h>



class ItemBigRepair : public Item{
public:
	ItemBigRepair();
	ItemBigRepair(int ID);
	~ItemBigRepair();
	ItemBigRepair* getItemInstance();
	bool causeEffectToTank(Tank* tank);
	void playUseSFX();
};

#endif