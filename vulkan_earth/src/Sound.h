#ifndef SOUND_H
#define SOUND_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

enum SFX {
    BIG_CLICK,
    SMALL_CLICK,
    ITEM_SELECTED,
    INVALID_CLICK,
    TRANSACTION,
    KEYTYPING,
    INVENTORY_ACCESS,
    ITEM_USE1,
    ITEM_USE2,
    ITEM_USE3,
    ITEM_USE4,
    ITEM_USE_FLOAT,
    ITEM_USE_SHIELD,
    WEAPON_LOAD,
    WEAPON_UNLOAD,
    SHIELD,
    WAVE1 = 30,
    WAVE2,
    WAVE3,
    SEAGULLS1,
    SEAGULLS2,
    SEAGULLS3,
    SEAGULLS4,
    SEAGULLS5,
    TANK_CONTROL1 = 40,
    TANK_CONTROL2,
    TANK_STUCK,
    TANK_FIRE1,
    TANK_FIRE2,
    TANK_FIRE3,
    TANK_FIRE4,
    TANK_FIRE5,
    BOMB_FLY,
    ELECTRIC_ZAP1 = 60,
    ELECTRIC_ZAP2,
    EXPLOSION1,
    EXPLOSION2,
    EXPLOSION3,
    EXPLOSION_ACID,
    EXPLOSION_THOR,
    EXPLOSION_REVIVE,
    EFFECT1 = 80,
    EFFECT_ACID,
    MANUAL
};

const int max_sfx_files = 100;

const int mainmenu = 0;
const int readymenu_start = 1;
const int readymenu_loop = 2;
const int shopmenu = 3;
const int gamestate_rock = 4;
const int gamestate_snow = 5;
const int gamestate_ice = 6;
const int gamestate_mars = 7;
const int gamestate_beach_start = 8;
const int gamestate_beach_loop = 9;
const int gamestate_desert = 10;
const int gamestate_lava = 11;
const int total_music_files = 12;

#endif