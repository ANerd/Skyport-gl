#include <stdio.h>
#include <SDL.h>
#include "textlib.h"

#define WIDTH 800
#define HEIGHT 800

void print_surface_properties(SDL_Surface *surf, const char *name);

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
  SDL_FillRect(mainsurf, NULL, SDL_MapRGB(mainsurf->format, 128, 128, 128));
  textlib_initialize();
  textlib_set_font(72, NULL);
  textlib_set_quality(2);
  SDL_Surface *text = textlib_get_text("foobar", 255, 255, 255, 255);
  print_surface_properties(text, "text-surface");
  SDL_BlitSurface(text, NULL, mainsurf, NULL); 
  SDL_Surface *nametag = textlib_get_nametag("Anders Halfhealth", 0.5f);
  print_surface_properties(nametag, "nametag-surface");
  SDL_Rect location = {10, text->h + 10, nametag->w, nametag->h};
  SDL_BlitSurface(nametag, NULL, mainsurf, &location);
  
  SDL_Surface *nametag2 = textlib_get_nametag("Amadiro Quarterhealth", 0.25f);
  SDL_Rect location2 = {10, text->h + nametag->h + 100, nametag2->w, nametag2->h};
  SDL_BlitSurface(nametag2, NULL, mainsurf, &location2);
  
  for(int i = 0;i<240;i++){
    SDL_UpdateRect(mainsurf, 0, 0, 0, 0);
    SDL_Delay(16);
  }
  textlib_quit();
}

void print_surface_properties(SDL_Surface *surf, const char *name){
  printf("'%s' stats: \n\twidth: %d\n\theight: %d\n\talpha: %s\n\t"
	 "scanline length: %d\n\tpixel format:\n"
	 "\t\tbpp: %d\n"
	 "\t\tred mask:\t%8x\n"
	 "\t\tgreen mask:\t%8x\n"
	 "\t\tblue mask:\t%8x\n"
	 "\t\talpha mask:\t%8x\n"
	 "\tswsurface: %s\n"
	 "\thwsurface: %s\n"
	 "\thwaccell: %s\n"
	 "\tsrccolorkey: %s\n"
	 "\trleaccel: %s\n",
	 name,
	 surf->w,
	 surf->h,
	 ((surf->flags & SDL_SRCALPHA) > 0 ? "true" : "false"),
	 surf->pitch,
	 surf->format->BitsPerPixel,
	 surf->format->Rmask,
	 surf->format->Gmask,
	 surf->format->Bmask,
	 surf->format->Amask,
	 ((surf->flags & SDL_SWSURFACE) > 0 ? "true" : "false"),
	 ((surf->flags & SDL_HWSURFACE) > 0 ? "true" : "false"),
	 ((surf->flags & SDL_HWACCEL) > 0 ? "true" : "false"),
	 ((surf->flags & SDL_SRCCOLORKEY) > 0 ? "true" : "false"),
	 ((surf->flags & SDL_RLEACCEL) > 0 ? "true" : "false"));
}
