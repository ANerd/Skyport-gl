#include "sndlib.h"

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

void sndlib_play_laser(void){
  Mix_PlayChannel(-1, snd_laser, 0);
  //Mix_PlayMusic(snd_laser, 0);
}
