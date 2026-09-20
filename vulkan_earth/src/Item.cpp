#include "Item.h"
#include "ImageObject.h"
#include <string>
#include "sound.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

using namespace std;

Item::Item(){}
Item::~Item(){
}
/*GETTERS*/
int Item::getUNIQUEIDENTIFIER()			{return this->UNIQUEIDENTIFIER;}
int Item::getRemaining()						{return this->remaining;}
const char* Item::getImageFileName()	{return this->imageFileName;}
const char* Item::getDescription()		{return this->description;}
int Item::getPrice()								{return this->price;}
int Item::getPackageNum()					{return this->packageNum;}
int Item::getMaxStack()							{return this->maxStack;}
/*SETTERS*/
void Item::setRemaining(int r)						{this->remaining = r;}

void Item::playUseSFX(){
	playSFX(ITEM_USE1);
}