#include <stdio.h>
#include <SDL.h>
#include "textlib.h"

#define WIDTH 1900
#define HEIGHT 1000

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
  textlib_set_quality(TEXT_QUALITY_HIGH);

  SDL_Surface *text = textlib_get_text("foobar", 255, 255, 255);
  print_surface_properties(text, "text-surface");
  SDL_BlitSurface(text, NULL, mainsurf, NULL); 

  SDL_Surface *nametag = textlib_get_nametag("a", 0.5f);
  print_surface_properties(nametag, "nametag-surface");
  SDL_Rect location = {10, text->h + 10, nametag->w, nametag->h};
  SDL_BlitSurface(nametag, NULL, mainsurf, &location);
  
  SDL_Surface *nametag2 = textlib_get_nametag("Foobar", 0.25f);
  SDL_Rect location2 = {10, text->h + nametag->h + 100, nametag2->w, nametag2->h};
  SDL_BlitSurface(nametag2, NULL, mainsurf, &location2);

  SDL_Surface *nametag3 = textlib_get_nametag("Jonny 16-letters", 0.75f);
  SDL_Rect location3 = {10, text->h + nametag->h + nametag2->h + 200, nametag3->w, nametag3->h};
  SDL_BlitSurface(nametag3, NULL, mainsurf, &location3);
  print_surface_properties(nametag3, "nametag-16letters");

  SDL_Surface *nametag4 = textlib_get_nametag("0123456789ABCDEF", 1.00f);
  SDL_Rect location4 = {10, text->h + nametag->h + nametag2->h + nametag3->h + 300,
			nametag4->w, nametag4->h};
  SDL_BlitSurface(nametag4, NULL, mainsurf, &location4);
  print_surface_properties(nametag4, "nametag-16letters");

  const char *names[] = {"0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF"};
  int points[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600};
  const char *primaries[] = {"laser", "mortar", "droid", "laser", "mortar", "droid", "laser", "droid", "mortar", "laser", "mortar", "droid", "laser", "laser", "laser", "laser"};
  const char *secondaries[] = {"laser", "mortar", "droid", "laser", "mortar", "droid", "laser", "droid", "mortar", "laser", "mortar", "droid", "laser", "laser", "laser", "laser"};


  SDL_Surface *stats = textlib_get_stats(8, names, points, primaries, secondaries, 1600);
  SDL_Rect statsloc = {10, text->h + nametag->h + nametag2->h + nametag3->h + nametag4->h + 400,
			stats->w, stats->h};
  SDL_BlitSurface(stats, NULL, mainsurf, &statsloc);

  SDL_Surface *stats2 = textlib_get_stats(16, names, points, primaries, secondaries, 1800);
  SDL_Rect statsloc2 = {10, text->h + nametag->h + nametag2->h + nametag3->h + nametag4->h + stats->h + 450,
			stats2->w, stats2->h};
  SDL_BlitSurface(stats2, NULL, mainsurf, &statsloc2);

  SDL_Surface *stats3 = textlib_get_stats(4, names, points, primaries, secondaries, 1200);
  SDL_Rect statsloc3 = {10, text->h + nametag->h + nametag2->h + nametag3->h + nametag4->h + stats->h + stats2->h + 500,
			stats3->w, stats3->h};
  SDL_BlitSurface(stats3, NULL, mainsurf, &statsloc3);

  
  for(int i = 0; i < 480; i++){
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
