#ifndef PLAYER_CPU_H
#define PLAYER_CPU_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
#include "Player.h"

class Tank;
class Item;
class Weapon;
class PlayerFactory;

class PlayerCPU : public Player {
	public:
		PlayerCPU();
		PlayerCPU(GLfloat red,GLfloat green,GLfloat blue);
		PlayerCPU(GLfloat red,GLfloat green,GLfloat blue,char* tankType,char* ai_type,char* name, char teamName, char* player_type,int startingCash);
		~PlayerCPU();
		void updateTank(/* Pass in all paramaters that are associated with a tank */);
		Tank* getCurrentTank();
		char* getTankType();
		void setTankType(char* tankType);
		Item** getCurrentItems();
		void setItems(Item** itemSet);
		Weapon** getCurrentWeapons();
		void setWeapons(Weapon** weaponSet);
		char* getAI_Type();
		void setAI_Type(char* ai_type);
		char* getPlayer_Type();
		char* getPlayerName();
		void setPlayerName(char* name);
		void setPlayer_Type(char* player_type);
		int getCash();
		void setCash(int money);
		float getCurrentWait();
		void setCurrentWait(float time);
		GLfloat getRed();
		GLfloat getGreen();
		GLfloat getBlue();
		char getTeamLabel();
		void setTeamLabel(char t);
		Weapon* getLoadedWeapon();
		void setLoadedWeapon(Weapon* wpn);

	private:
		Tank* currentTank;
		int currentCash;
		char tankType[25];
		Item* currentItems[PLAYER_MAX_ITEMS];
		Weapon* currentWeapons[PLAYER_MAX_WEAPONS];
		char ai_type[128];
		char name[128];
		char player_type[128];
		GLfloat color[4];
		float currentWait;
		char teamLabel;
		Weapon* loadedWeapon;
};

#endif /*	PLAYER_CPU_H	*/