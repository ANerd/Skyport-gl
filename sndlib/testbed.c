#include <stdio.h>
#include "sndlib.h"

int main(void){
  sndlib_init();
  sndlib_play_wind();
  SDL_Delay(1000);
  sndlib_play_laser();
  SDL_Delay(50000);
  sndlib_quit();
}
