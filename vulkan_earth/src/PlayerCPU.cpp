#include <cstring>
#include "PlayerCPU.h"
#include "PlayerFactory.h"
#include "Tank.h"
#include "TankA.h"
#include "TankB.h"
#include "TankC.h"
#include "TankD.h"
#include "TankE.h"
#include "TankF.h"
#include "TankG.h"
#include "TankH.h"
#include "macro_crtdbg.h"

PlayerCPU::PlayerCPU() {
	
}

PlayerCPU::PlayerCPU(GLfloat red,GLfloat green,GLfloat blue) {
	this->color[0]=red;
	this->color[1]=green;
	this->color[2]=blue;
	this->color[3]=1.0;
	
	this->currentTank = new TankA(0,0,0);

	for(int i=0;i<PLAYER_MAX_ITEMS;i++){
		currentItems[i]=NULL;
	}
	for(int i=0;i<PLAYER_MAX_WEAPONS;i++){
		currentWeapons[i]=NULL;
	}
	
	strcpy(this->tankType,"Rhinoxx");
	strcpy(this->ai_type,"Shooter");
	strcpy(this->player_type,"CPU");
	strcpy(this->name,"");

	this->teamLabel = '-';
	loadedWeapon=NULL;
}


PlayerCPU::PlayerCPU(GLfloat red,GLfloat green,GLfloat blue,char* tankType,char* ai_type,char* name, char teamLabel, char* player_type,int startingCash) {
	this->color[0]=red;
	this->color[1]=green;
	this->color[2]=blue;
	this->color[3]=1.0;

	this->currentCash=startingCash;
	this->currentWait=0;
	this->teamLabel = teamLabel;

	for(int i=0;i<PLAYER_MAX_ITEMS;i++){
		currentItems[i]=NULL;
	}
	for(int i=0;i<PLAYER_MAX_WEAPONS;i++){
		currentWeapons[i]=NULL;
	}
	loadedWeapon=NULL;
	
	strcpy(this->ai_type,ai_type);
	strcpy(this->player_type,player_type);
	strcpy(this->name,name);
	strcpy(this->tankType,tankType);
	if(strcmp(this->tankType, "Rhinoxx")==0)				currentTank = new TankA(0, 0, 0);
	else if(strcmp(this->tankType, "Hellfire")==0)		currentTank = new TankB(0, 0, 0);
	else if(strcmp(this->tankType, "HeavyD")==0)		currentTank = new TankC(0, 0, 0);
	else if(strcmp(this->tankType, "Panzer")==0)		currentTank = new TankD(0, 0, 0);
	else if(strcmp(this->tankType, "Eggroid")==0)		currentTank = new TankE(0, 0, 0);
	else if(strcmp(this->tankType, "Behemoth")==0)		currentTank = new TankF(0, 0, 0);
	else if(strcmp(this->tankType, "Cubix")==0)		currentTank = new TankG(0, 0, 0);
	else if(strcmp(this->tankType, "Predator")==0)		currentTank = new TankH(0, 0, 0);
	else{
		printf("ERROR: Wrong Tank Type!!\n");
		currentTank = NULL;
	}
}

PlayerCPU::~PlayerCPU() {
	delete currentTank;

	// before we delete inventory, where should we store the inventory? figure this out later
	for(int i=0;i<PLAYER_MAX_ITEMS;i++)
			delete currentItems[i];
	for(int i=0;i<PLAYER_MAX_WEAPONS;i++)
			delete currentWeapons[i];
}

void PlayerCPU::updateTank(/* Pass in all paramaters that are associated with a tank */) { }
Tank* PlayerCPU::getCurrentTank() { return this->currentTank; }
char* PlayerCPU::getTankType() { return this->tankType; }
Item** PlayerCPU::getCurrentItems() { return this->currentItems; }
Weapon** PlayerCPU::getCurrentWeapons() { return this->currentWeapons; }
char* PlayerCPU::getAI_Type() { return this->ai_type; }
void PlayerCPU::setAI_Type(char* ai_type) { strcpy(this->ai_type,ai_type); }
char* PlayerCPU::getPlayer_Type() { return this->player_type; }
void PlayerCPU::setPlayer_Type(char* player_type) { strcpy(this->player_type,player_type); }
GLfloat PlayerCPU::getRed() { return this->color[0]; }
GLfloat PlayerCPU::getGreen() { return this->color[1]; }
GLfloat PlayerCPU::getBlue() { return this->color[2]; }
char* PlayerCPU::getPlayerName() { return this->player_type; }
int PlayerCPU::getCash() { return this->currentCash; }
void PlayerCPU::setCash(int cash) { this->currentCash=cash; }
float PlayerCPU::getCurrentWait(){ return this->currentWait; }
void PlayerCPU::setCurrentWait(float time){ this->currentWait=time; }
char PlayerCPU::getTeamLabel(){ return this->teamLabel; }
void PlayerCPU::setTeamLabel(char t){ this->teamLabel = t; }
Weapon* PlayerCPU::getLoadedWeapon(){return loadedWeapon;}
void PlayerCPU::setLoadedWeapon(Weapon* wpn){loadedWeapon = wpn;}
void PlayerCPU::setPlayerName(char* name){
	printf("\nYou're trying to set a name for CPU. It won't happen, sorry.\n");
	strcpy(this->name, "CPU");
}
void PlayerCPU::setWeapons(Weapon** weaponSet) {
	for(int i=0;i<PLAYER_MAX_WEAPONS;i++){
		this->currentWeapons[i]=weaponSet[i];
	}
}
void PlayerCPU::setItems(Item** itemSet) {
	for(int i=0;i<PLAYER_MAX_ITEMS;i++){
		this->currentItems[i]=itemSet[i];
	}
}
void PlayerCPU::setTankType(char* tankType) {
	strcpy(this->tankType,tankType);
	if(currentTank != NULL)		delete currentTank;
	if(strcmp(this->tankType, "Rhinoxx")==0)			currentTank = new TankA(0, 0, 0);
	else if(strcmp(this->tankType, "Hellfire")==0)		currentTank = new TankB(0, 0, 0);
	else if(strcmp(this->tankType, "HeavyD")==0)		currentTank = new TankC(0, 0, 0);
	else if(strcmp(this->tankType, "Panzer")==0)		currentTank = new TankD(0, 0, 0);
	else if(strcmp(this->tankType, "Eggroid")==0)		currentTank = new TankE(0, 0, 0);
	else if(strcmp(this->tankType, "Behemoth")==0)		currentTank = new TankF(0, 0, 0);
	else if(strcmp(this->tankType, "Cubix")==0)			currentTank = new TankG(0, 0, 0);
	else if(strcmp(this->tankType, "Predator")==0)		currentTank = new TankH(0, 0, 0);
	else{
		printf("ERROR: Wrong Tank Type!!\n");
		currentTank = NULL;
	}
}