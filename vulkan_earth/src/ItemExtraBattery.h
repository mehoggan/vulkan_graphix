#ifndef ITEM_EXTRA_BATTERY_H
#define ITEM_EXTRA_BATTERY_H

#include <GL/freeglut.h>

class ItemExtraBattery : public Item{
public:
	ItemExtraBattery();
	ItemExtraBattery(int ID);
	~ItemExtraBattery();
	ItemExtraBattery* getItemInstance();
	bool causeEffectToTank(Tank* tank);
	void playUseSFX();
};

#endif