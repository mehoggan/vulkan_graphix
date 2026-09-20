#include "sound.h"
#include "macro_crtdbg.h"

Mix_Chunk *sfx[MAX_SFX_FILES];
Mix_Music *music[TOTAL_MUSIC_FILES];

void initSound() {
    int audio_rate = 48000;
    Uint16 audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 4096;

    if (SDL_Init( SDL_INIT_AUDIO)!=0) {
    	printf("Unable to initialize SDL: %s\n", SDL_GetError());
	}
    if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)!=0) {
    	printf("Unable to initialize audio: %s\n", Mix_GetError());
	}
	
	for (int i=0 ; i<MAX_SFX_FILES ; i++) {
		sfx[i] = NULL;
	}
	for (int i=0 ; i<TOTAL_MUSIC_FILES ; i++) {
		music[i] = NULL;
	}
	
	sfx[BIG_CLICK] = Mix_LoadWAV("Sounds/SFX/big_click.wav");
	sfx[SMALL_CLICK] = Mix_LoadWAV("Sounds/SFX/small_click.wav");
	sfx[ITEM_SELECTED] = Mix_LoadWAV("Sounds/SFX/item_selected.wav");
	sfx[INVALID_CLICK] = Mix_LoadWAV("Sounds/SFX/invalid_click.wav");
	sfx[TRANSACTION] = Mix_LoadWAV("Sounds/SFX/transaction.wav");
	sfx[KEYTYPING] = Mix_LoadWAV("Sounds/SFX/keytyping.wav");
	sfx[INVENTORY_ACCESS] = Mix_LoadWAV("Sounds/SFX/inventory.wav");
	sfx[ITEM_USE1] = Mix_LoadWAV("Sounds/SFX/item_use1.wav");
	sfx[ITEM_USE2] = Mix_LoadWAV("Sounds/SFX/item_use2.wav");
	sfx[ITEM_USE3] = Mix_LoadWAV("Sounds/SFX/item_use3.wav");
	sfx[ITEM_USE4] = Mix_LoadWAV("Sounds/SFX/item_use4.wav");
	sfx[ITEM_USE_FLOAT] = Mix_LoadWAV("Sounds/SFX/item_use_float.wav");
	sfx[ITEM_USE_SHIELD] = Mix_LoadWAV("Sounds/SFX/item_use_shield.wav");
	sfx[WEAPON_LOAD] = Mix_LoadWAV("Sounds/SFX/weapon_load.wav");
	sfx[WEAPON_UNLOAD] = Mix_LoadWAV("Sounds/SFX/Weapon_unload.wav");
	sfx[SHIELD] = Mix_LoadWAV("Sounds/SFX/shield.wav");
	sfx[WAVE1] = Mix_LoadWAV("Sounds/SFX/wave1.wav");
	sfx[WAVE2] = Mix_LoadWAV("Sounds/SFX/wave2.wav");
	sfx[WAVE3] = Mix_LoadWAV("Sounds/SFX/wave3.wav");
	sfx[SEAGULLS1] = Mix_LoadWAV("Sounds/SFX/seagulls1.wav");
	sfx[SEAGULLS2] = Mix_LoadWAV("Sounds/SFX/seagulls2.wav");
	sfx[SEAGULLS3] = Mix_LoadWAV("Sounds/SFX/seagulls3.wav");
	sfx[SEAGULLS4] = Mix_LoadWAV("Sounds/SFX/seagulls4.wav");
	sfx[SEAGULLS5] = Mix_LoadWAV("Sounds/SFX/seagulls5.wav");
	sfx[TANK_CONTROL1] = Mix_LoadWAV("Sounds/SFX/tank_control1.wav");
	sfx[TANK_CONTROL2] = Mix_LoadWAV("Sounds/SFX/tank_control2.wav");
	sfx[TANK_STUCK] = Mix_LoadWAV("Sounds/SFX/tank_stuck.wav");
	sfx[TANK_FIRE1] = Mix_LoadWAV("Sounds/SFX/tank_fire1.wav");
	sfx[TANK_FIRE2] = Mix_LoadWAV("Sounds/SFX/tank_fire2.wav");
	sfx[TANK_FIRE3] = Mix_LoadWAV("Sounds/SFX/tank_fire3.wav");
	sfx[TANK_FIRE4] = Mix_LoadWAV("Sounds/SFX/tank_fire4.wav");
	sfx[TANK_FIRE5] = Mix_LoadWAV("Sounds/SFX/tank_fire5.wav");
	sfx[BOMB_FLY] = Mix_LoadWAV("Sounds/SFX/bomb_fly.wav");
	sfx[ELECTRIC_ZAP1] = Mix_LoadWAV("Sounds/SFX/electric_zap1.wav");
	sfx[ELECTRIC_ZAP2] = Mix_LoadWAV("Sounds/SFX/electric_zap2.wav");
	sfx[EXPLOSION1] = Mix_LoadWAV("Sounds/SFX/explosion1.wav");
	sfx[EXPLOSION2] = Mix_LoadWAV("Sounds/SFX/explosion2.wav");
	sfx[EXPLOSION3] = Mix_LoadWAV("Sounds/SFX/explosion3.wav");
	sfx[EXPLOSION_ACID] = Mix_LoadWAV("Sounds/SFX/explosion_acid.wav");
	sfx[EXPLOSION_THOR] = Mix_LoadWAV("Sounds/SFX/explosion_thor.wav");
	sfx[EXPLOSION_REVIVE] = Mix_LoadWAV("Sounds/SFX/explosion_revive.wav");
	sfx[EFFECT1] = Mix_LoadWAV("Sounds/SFX/effect1.wav");
	sfx[EFFECT_ACID] = Mix_LoadWAV("Sounds/SFX/effect_acid.wav");
	sfx[MANUAL] = Mix_LoadWAV("Sounds/SFX/manual.wav");

	music[0] = Mix_LoadMUS("Sounds/Music/main_menu.mp3");
	music[1] = Mix_LoadMUS("Sounds/Music/ready_menu_start.mp3");
	music[2] = Mix_LoadMUS("Sounds/Music/ready_menu_loop.mp3");
	music[3] = Mix_LoadMUS("Sounds/Music/shop_menu.mp3");
	music[4] = Mix_LoadMUS("Sounds/Music/gamestate_rock.mp3");
	music[5] = Mix_LoadMUS("Sounds/Music/gamestate_snow.mp3");
	music[6] = Mix_LoadMUS("Sounds/Music/gamestate_ice.mp3");
	music[7] = Mix_LoadMUS("Sounds/Music/gamestate_mars.mp3");
	music[8] = Mix_LoadMUS("Sounds/Music/gamestate_beach_start.mp3");
	music[9] = Mix_LoadMUS("Sounds/Music/gamestate_beach_loop.mp3");
	music[10] = Mix_LoadMUS("Sounds/Music/gamestate_desert.mp3");
	music[11] = Mix_LoadMUS("Sounds/Music/gamestate_lava.mp3");
	for(int i=0 ; i<TOTAL_MUSIC_FILES ; i++) {
		if(music[i] == NULL) {
            printf("Unable to load music: %s\n", Mix_GetError());
		}
	}
}

void playSFX(int type) {
	if(type==BIG_CLICK) {
		Mix_PlayChannel(0,sfx[type],0);
	}
	else if(type==SMALL_CLICK){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==ITEM_SELECTED){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==INVALID_CLICK){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==TRANSACTION){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==KEYTYPING){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==WAVE1){
		if(Mix_Playing(6)==0)
			Mix_PlayChannel(6, sfx[type],0);
	}
	else if(type==WAVE2){
		if(Mix_Playing(6)==0)
			Mix_PlayChannel(6, sfx[type],0);
	}
	else if(type==WAVE3){
		if(Mix_Playing(6)==0)
			Mix_PlayChannel(6, sfx[type],0);
	}
	else if(type==SEAGULLS1){
		if(Mix_Playing(7)==0)
			Mix_PlayChannel(7, sfx[type],0);
	}
	else if(type==SEAGULLS2){
		if(Mix_Playing(7)==0)
			Mix_PlayChannel(7, sfx[type],0);
	}
	else if(type==SEAGULLS3){
		if(Mix_Playing(7)==0)
			Mix_PlayChannel(7, sfx[type],0);
	}
	else if(type==SEAGULLS4){
		if(Mix_Playing(7)==0)
			Mix_PlayChannel(7, sfx[type],0);
	}
	else if(type==SEAGULLS5){
		if(Mix_Playing(7)==0)
			Mix_PlayChannel(7, sfx[type],0);
	}
	else if(type==INVENTORY_ACCESS){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==ITEM_USE1){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==ITEM_USE2){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==ITEM_USE3){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==ITEM_USE4){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==ITEM_USE_FLOAT){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==ITEM_USE_SHIELD){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==WEAPON_LOAD){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==WEAPON_UNLOAD){
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==SHIELD){
		Mix_HaltChannel(1);
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==TANK_CONTROL1){
		if(Mix_Playing(2)==0)
			Mix_PlayChannel(2, sfx[type],-1);
	}
	else if(type==TANK_CONTROL2){
		if(Mix_Playing(3)==0)
			Mix_PlayChannel(3, sfx[type],-1);
	}
	else if(type==TANK_STUCK){
		Mix_HaltChannel(2);
		Mix_PlayChannel(4, sfx[type],0);
	}
	else if(type==TANK_FIRE1){
		Mix_HaltChannel(2);
		Mix_HaltChannel(3);
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==TANK_FIRE2){
		Mix_HaltChannel(2);
		Mix_HaltChannel(3);
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==TANK_FIRE3){
		Mix_HaltChannel(2);
		Mix_HaltChannel(3);
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==TANK_FIRE4){
		Mix_HaltChannel(2);
		Mix_HaltChannel(3);
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==TANK_FIRE5){
		Mix_HaltChannel(2);
		Mix_HaltChannel(3);
		Mix_PlayChannel(0, sfx[type],0);
	}
	else if(type==BOMB_FLY){
		if(Mix_Playing(1)==0)
			Mix_PlayChannel(1, sfx[type],0);
	}
	else if(type==ELECTRIC_ZAP2){
		Mix_PlayChannel(1, sfx[type],0);
	}
	else if(type==EXPLOSION1){
		Mix_PlayChannel(1, sfx[type],0);
	}
	else if(type==EXPLOSION2){
		Mix_PlayChannel(1, sfx[type],0);
	}
	else if(type==EXPLOSION3){
		Mix_PlayChannel(1, sfx[type],0);
	}
	else if(type==EXPLOSION_THOR){
		Mix_PlayChannel(1, sfx[type],0);
	}
	else if(type==EXPLOSION_ACID){
		Mix_PlayChannel(1, sfx[type],0);
	}
	else if(type==EXPLOSION_REVIVE){
		Mix_PlayChannel(1, sfx[type],0);
	}
	else if(type==EFFECT1){
		Mix_PlayChannel(4, sfx[type],0);
	}
	else if(type==EFFECT_ACID){
		Mix_PlayChannel(4, sfx[type],0);
	}
	else if(type==MANUAL){
		Mix_HaltChannel(2);
		Mix_HaltChannel(3);
		Mix_PlayChannel(0, sfx[type],0);
	}
}

void playMusic(int type) {
	if(type==MAINMENU) {
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type],-1);
	}
	else if(type==READYMENU_START) {
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type],0);
	}
	else if(type==READYMENU_LOOP) {
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type],-1);
	}
	else if(type==SHOPMENU){
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type],-1);
	}
	else if(type==GAMESTATE_ROCK){
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type], -1);
	}
	else if(type==GAMESTATE_BEACH_START){
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type], 0);
	}
	else if(type==GAMESTATE_BEACH_LOOP){
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type], -1);
	}
	else if(type==GAMESTATE_MARS){
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type], -1);
	}
	else if(type==GAMESTATE_SNOW){
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type], -1);
	}
	else if(type==GAMESTATE_ICE){
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type], -1);
	}
	else if(type==GAMESTATE_LAVA){
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type], -1);
	}
	else if(type==GAMESTATE_DESERT){
		if(Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music[type], -1);
	}
}