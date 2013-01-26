#include <stdio.h>
#include <SDL.h>
#include "textlib.h"

#define WIDTH 800
#define HEIGHT 800

int main(void){
  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    printf("Error initializing SDL: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  SDL_Surface *mainsurf = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_SWSURFACE);
  if(mainsurf == NULL){
    printf("Error creating SDL screen: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  SDL_FillRect(mainsurf, NULL, SDL_MapRGB(mainsurf->format, 0, 0, 0));
  textlib_initialize();
  textlib_set_font_size(72);
  textlib_set_background_color(0, 255, 255);
  textlib_set_quality(2);
  for(int i = 0;i<1000;i++){
    SDL_Surface *text = textlib_get_text((char*)"foobar");
    SDL_BlitSurface(text, NULL, mainsurf, NULL); 
    SDL_UpdateRect(mainsurf, 0, 0, 0, 0);
  }
  textlib_quit();
}
