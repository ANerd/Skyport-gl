#include "sndlib.h"
#include <assert.h>

#define SNDLIB_STUB(string) do{printf("STUB! sndlib.c:%d, in function %s: %s\n", __LINE__, __FUNCTION__, string);}while(0)

#ifndef NO_SNDLIB
// Sound is globally enabled.

static Mix_Chunk *snd_wind;
static Mix_Chunk *snd_laser;
static Mix_Chunk *snd_steps;
static Mix_Chunk *snd_mortar_shot;
static Mix_Chunk *snd_mortar_hit;
static Mix_Chunk *snd_robot_destruction;


void _sndlib_load_samples(void);
void _sndlib_load_samples(void){
  snd_wind = Mix_LoadWAV(SNDLIB_SOUND_WIND);
  snd_laser = Mix_LoadWAV(SNDLIB_SOUND_LASER);
  snd_steps = Mix_LoadWAV(SNDLIB_SOUND_ROBOT_MOVEMENT);
  snd_mortar_shot = Mix_LoadWAV(SNDLIB_SOUND_MORTAR_FIRE);
  snd_mortar_hit = Mix_LoadWAV(SNDLIB_SOUND_MORTAR_IMPACT);
  snd_robot_destruction = Mix_LoadWAV(SNDLIB_SOUND_ROBOT_DESTRUCTION);
  assert(snd_wind != NULL);
  assert(snd_laser != NULL);
  assert(snd_steps != NULL);
  assert(snd_mortar_shot != NULL);
  assert(snd_mortar_hit != NULL);
  assert(snd_robot_destruction != NULL);
}

void sndlib_init(void){
  assert(SDL_Init(SDL_INIT_AUDIO) != -1);
  assert(Mix_OpenAudio(22050, AUDIO_S16, 2, 4096) != -1);
  _sndlib_load_samples();
}
void sndlib_quit(void){
  Mix_CloseAudio();
}

void sndlib_play_wind(void){
  assert(Mix_PlayChannel(-1, snd_wind, -1) != -1);
  //Mix_PlayMusic(snd_wind, 0);
}

void sndlib_stop_wind(void){
  SNDLIB_STUB("register globals yada yada");
}

void sndlib_play_laser(int milliseconds){
  Mix_PlayChannelTimed(-1, snd_laser, 0, milliseconds);
  //Mix_PlayMusic(snd_laser, 0);
}

void sndlib_play_mortar_fire(void){
  //int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int loops)
  Mix_PlayChannel(-1, snd_mortar_shot, 0);
}
void sndlib_play_mortar_air(int milliseconds){
  SNDLIB_STUB("add sound");
}
void sndlib_play_mortar_impact(void){
  Mix_PlayChannel(-1, snd_mortar_hit, 0);
}
void sndlib_play_droid_fire(void){
  SNDLIB_STUB("add sound");
}
void sndlib_play_droid_step(void){
  SNDLIB_STUB("add sound");
}
void sndlib_play_droid_impact(void){
  Mix_PlayChannel(-1, snd_mortar_hit, 0);
}
void sndlib_play_robot_destruction(void){
  Mix_PlayChannel(-1, snd_robot_destruction, 0);
}
void sndlib_play_robot_respawn(void){
  SNDLIB_STUB("add sound");
}
void sndlib_play_robot_mining(void){
  SNDLIB_STUB("add sound");
}

#else
// Sound is globally disabled.

void sndlib_init(void){
  printf("sndlib.c: Compiled with -DNO_SNDLIB set, won't play any sound or load any files.\n");
}
void sndlib_quit(void){}
void sndlib_play_wind(void){}
void sndlib_stop_wind(void){}
void sndlib_play_laser(int milliseconds){}
void sndlib_play_mortar_fire(void){}
void sndlib_play_mortar_air(int milliseconds){}
void sndlib_play_mortar_impact(void){}
void sndlib_play_droid_fire(void){}
void sndlib_play_droid_step(void){}
void sndlib_play_droid_impact(void){}
void sndlib_play_robot_destruction(void){}
void sndlib_play_robot_respawn(void){}
void sndlib_play_robot_mining(void){}
#endif
