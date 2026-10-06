#ifndef VULKAN_EARTH_SOUND_H
#define VULKAN_EARTH_SOUND_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <cstdint>

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

const std::int32_t max_sfx_files = 100;

const std::int32_t mainmenu = 0;
const std::int32_t readymenu_start = 1;
const std::int32_t readymenu_loop = 2;
const std::int32_t shopmenu = 3;
const std::int32_t gamestate_rock = 4;
const std::int32_t gamestate_snow = 5;
const std::int32_t gamestate_ice = 6;
const std::int32_t gamestate_mars = 7;
const std::int32_t gamestate_beach_start = 8;
const std::int32_t gamestate_beach_loop = 9;
const std::int32_t gamestate_desert = 10;
const std::int32_t gamestate_lava = 11;
const std::int32_t total_music_files = 12;

#endif