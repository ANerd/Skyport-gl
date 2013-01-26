#include "textlib.h"
#include <stdio.h>
#include <math.h>

static TTF_Font *font;
static unsigned int quality;
static SDL_Color bgcolor;

void textlib_initialize(void){
  if(TTF_Init() == -1){
    printf("textlib: error initializing SDL_ttf: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }
  textlib_set_font(DEFAULT_FONT_DPI, NULL);
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

void textlib_set_font(int dpi, const char *the_font){
  if(the_font == NULL){
    the_font = (char*)DEFAULT_FONT_FILE;
  }
  font = TTF_OpenFont(the_font, dpi);
  if(font == NULL){
    printf("textlib: Error loading font '%s': %s\n", the_font, TTF_GetError());
  }
}

void textlib_quit(void){
  TTF_Quit();
}

SDL_Surface *textlib_get_text(const char *text, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
  SDL_Color color = {r, g, b, a};
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

SDL_Surface *textlib_get_nametag(const char *name, float health){
  TTF_Font *oldfont = font;
  unsigned int oldquality = quality;
  
  textlib_set_font(16, NULL);
  textlib_set_quality(2);
  SDL_Surface *nametag = textlib_get_text(name, 0, 0, 0, 255);
  SDL_Surface *bg = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA,
					 nametag->w+10, nametag->h+10,
					 nametag->format->BitsPerPixel,
					 nametag->format->Rmask, nametag->format->Gmask,
					 nametag->format->Bmask, nametag->format->Amask);
  unsigned int pixel_fill_boundary = round(health*(nametag->w));
  SDL_FillRect(bg, NULL, SDL_MapRGBA(bg->format, 0, 0, 0, 255));
  
  SDL_Rect interior = {2, 2, nametag->w + 10 - 4, nametag->h + 10 - 4};
  SDL_FillRect(bg, &interior, SDL_MapRGBA(bg->format, 255, 64, 64, 128));
  
  SDL_Rect health_rect = {2, 2, pixel_fill_boundary - 2, nametag->h + 10 - 4};
  SDL_FillRect(bg, &health_rect, SDL_MapRGBA(bg->format, 64, 255, 64, 128));

  SDL_SetAlpha(nametag, SDL_SRCALPHA, 0);
  SDL_Rect name_rect = {5, 5, nametag->w, nametag->h};
  SDL_BlitSurface(nametag, NULL, bg, &name_rect);
  //SDL_SetAlpha(bg, SDL_SRCALPHA, 255);
  
  
  font = oldfont;
  quality = oldquality;
  return bg;
}
