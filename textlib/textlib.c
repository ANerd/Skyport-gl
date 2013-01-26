#include "textlib.h"
#include <stdio.h>

static TTF_Font *font;
static unsigned int quality;
static SDL_Color bgcolor;

void textlib_initialize(void){
  if(TTF_Init() == -1){
    printf("textlib: error initializing SDL_ttf: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }
  textlib_set_font_size(DEFAULT_FONT_DPI);
  textlib_set_background_color(255, 255, 255);
}
void textlib_set_background_color(Uint8 r, Uint8 g, Uint8 b){
  SDL_Color color = {r, g, b, 0};
  bgcolor = color;
}
void textlib_set_quality(unsigned int the_quality){
  if(quality < 3){
    quality = the_quality;
  }
  else {
    printf("textlib: textlib_set_quality() called with invalid parameter '%u'\n", the_quality);
  }
}

void textlib_set_font_size(int dpi){
  font = TTF_OpenFont(DEFAULT_FONT_FILE, dpi);
  if(font == NULL){
    printf("textlib: Error loading font '%s': %s\n", DEFAULT_FONT_FILE, TTF_GetError());
  }
}

void textlib_quit(void){
  TTF_Quit();
}

SDL_Surface *textlib_get_text(char *text){
  /* Uint32 rmask, gmask, bmask, amask; */
  /* rmask = 0xff000000; // ensure we pack the pixels in RGBA format */
  /* gmask = 0x00ff0000; */
  /* bmask = 0x0000ff00; */
  /* amask = 0x000000ff; */
  /* SDL_Surface *textsurf = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 100, 100, 32,  */
  /* 					       rmask, gmask, bmask, amask); */
  /* SDL_FillRect(textsurf, NULL, SDL_MapRGBA(textsurf->format, 255, 0, 0, 255)); */


  SDL_Color color = {255, 255, 255, 0};
  SDL_Surface *textsurf;
  switch (quality){
  case 2: // highest quality; blending
    textsurf = TTF_RenderText_Blended(font, text, color);
    break;
  case 1: // med. quality; shading
    textsurf = TTF_RenderText_Shaded(font, text, color, bgcolor);
    break;
  default: // lowest quality
    textsurf = TTF_RenderText_Solid(font, text, color);
    break;
  }
  return textsurf;
}

