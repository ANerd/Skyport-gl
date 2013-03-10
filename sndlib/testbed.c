#include <stdio.h>
#include <sys/time.h>
#include "sndlib.h"

float _now(void);
float _now(void){
  struct timeval t;
  gettimeofday(&t, NULL);
  long int now = (t.tv_sec*1000000 + (t.tv_usec));
  return (float)now/1000000.0f;
}
#define BENCHMARK_INIT float __benchmark_tick;
#define BENCHMARK_TICK __benchmark_tick = _now();
#define BENCHMARK_TOCK(string) do {float now = _now(); printf("%s completed in %f milliseconds.\n", string, (now - __benchmark_tick)*1000);} while(0);



int main(void){
  BENCHMARK_INIT;

  BENCHMARK_TICK;
  sndlib_init();
  BENCHMARK_TOCK("initialization");

  BENCHMARK_TICK;
  sndlib_play_wind();
  BENCHMARK_TOCK("playing wind sound");
  
  SDL_Delay(1000);

  BENCHMARK_TICK;
  sndlib_play_laser(800);
  BENCHMARK_TOCK("playing laser sound");
  
sndlib_play_mortar_fire();

  SDL_Delay(50000);
  sndlib_quit();
}
