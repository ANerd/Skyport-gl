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
  textlib_set_background_color(0, 0, 0);
  textlib_set_quality(2);
  SDL_Surface *text = textlib_get_text((char*)"foobar");
  printf("Text stats: \n\twidth: %d\n\theight: %d\n\talpha: %s\n\t"
	 "scanline length: %d\n\tpixel format:\n"
	 "\t\tbpp: %d\n"
	 "\t\tred mask:\t%8x\n"
	 "\t\tgreen mask:\t%8x\n"
	 "\t\tblue mask:\t%8x\n"
	 "\t\talpha mask:\t%8x\n",
	 text->w, text->h, ((text->flags & SDL_SRCALPHA) > 0 ? "true" : "false"),
	 text->pitch,
	 text->format->BitsPerPixel,
	 text->format->Rmask,
	 text->format->Gmask,
	 text->format->Bmask,
	 text->format->Amask);
  for(int i = 0;i<240;i++){
    SDL_BlitSurface(text, NULL, mainsurf, NULL); 
    SDL_UpdateRect(mainsurf, 0, 0, 0, 0);
    SDL_Delay(16);
  }
  textlib_quit();
}
