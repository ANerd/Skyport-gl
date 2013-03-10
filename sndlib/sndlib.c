#include "sndlib.h"

#define SNDLIB_STUB(string) do{printf("STUB! sndlib.c:%d, in function %s: %s\n", __LINE__, __FUNCTION__, string);}while(0)

#ifndef NO_SNDLIB
// Sound is globally enabled.

static Mix_Chunk *snd_wind;
static Mix_Chunk *snd_laser;

void _sndlib_load_samples(void);
void _sndlib_load_samples(void){
  snd_wind = Mix_LoadWAV(SNDLIB_SOUND_WIND);
  snd_laser = Mix_LoadWAV(SNDLIB_SOUND_LASER);
}

void sndlib_init(void){
  SDL_Init(SDL_INIT_AUDIO);
  Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);
  _sndlib_load_samples();
}
void sndlib_quit(void){
  Mix_CloseAudio();
}

void sndlib_play_wind(void){
  Mix_PlayChannel(-1, snd_wind, -1);
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
  SNDLIB_STUB("add sound");
}
void sndlib_play_mortar_air(int milliseconds){
  SNDLIB_STUB("add sound");
}
void sndlib_play_mortar_impact(void){
  SNDLIB_STUB("add sound");
}
void sndlib_play_droid_fire(void){
  SNDLIB_STUB("add sound");
}
void sndlib_play_droid_step(void){
  SNDLIB_STUB("add sound");
}
void sndlib_play_droid_impact(void){
  SNDLIB_STUB("add sound");
}
void sndlib_play_robot_destruction(void){
  SNDLIB_STUB("add sound");
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
  printf("sndlib.c: Compiled without sndlib support, won't play any sound.\n");
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
