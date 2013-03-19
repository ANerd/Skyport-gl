#include "textlib.h"
#include <stdio.h>
#include <math.h>

// these values are hardcoded for 16-char nametags.
// If we want to allow longer names, they need adjusting.
#define NAMETAG_WIDTH 256
#define NAMETAG_HEIGHT 32

static TTF_Font *font;
static unsigned int quality;
static SDL_Color bgcolor;

void textlib_initialize(void){
  font = NULL;
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
  if(font != NULL)
  {
    TTF_CloseFont(font);
    font = NULL;
  }
  if(the_font == NULL){
    the_font = (char*)DEFAULT_FONT_FILE;
  }
  font = TTF_OpenFont(the_font, dpi);
  if(font == NULL){
    printf("textlib: Error loading font '%s': %s\n", the_font, TTF_GetError());
  }
}

void textlib_quit(void){
  if(font != NULL)
  {
    TTF_CloseFont(font);
    font = NULL;
  }
  TTF_Quit();
}

SDL_Surface *textlib_get_text(const char *text, Uint8 r, Uint8 g, Uint8 b){
  SDL_Color color = {r, g, b, 255};
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
  
  font = NULL;
  textlib_set_font(19, NULL);
  textlib_set_quality(TEXT_QUALITY_HIGH);
  SDL_Surface *nametag = textlib_get_text(name, 0, 0, 0);
  SDL_Surface *bg = SDL_CreateRGBSurface(SDL_SWSURFACE,
					 //nametag->w+10, nametag->h+10,
					 NAMETAG_WIDTH, NAMETAG_HEIGHT,
					 nametag->format->BitsPerPixel,
					 nametag->format->Rmask, nametag->format->Gmask,
					 nametag->format->Bmask, nametag->format->Amask);
  unsigned int pixel_fill_boundary = (unsigned int)round(health*(NAMETAG_WIDTH));
  SDL_FillRect(bg, NULL, SDL_MapRGBA(bg->format, 0, 0, 0, 255));
  
  SDL_Rect interior = {2, 2, NAMETAG_WIDTH - 4, NAMETAG_HEIGHT - 4};
  SDL_FillRect(bg, &interior, SDL_MapRGBA(bg->format, 255, 100, 100, 255));
  
  SDL_Rect health_rect = {2, 2, pixel_fill_boundary - 4, NAMETAG_HEIGHT - 4};
  SDL_FillRect(bg, &health_rect, SDL_MapRGBA(bg->format, 100, 255, 100, 255));

  SDL_Rect name_rect = {(int)round(NAMETAG_WIDTH/2.0 - (nametag->w)/2), 6, nametag->w, nametag->h};
  SDL_BlitSurface(nametag, NULL, bg, &name_rect);
  SDL_FreeSurface(nametag);
  
  TTF_CloseFont(font);
  font = oldfont;
  quality = oldquality;
  return bg;
}

SDL_Surface *textlib_get_stats(unsigned int players, const char **names,
			       int *points, int *primary_weapon, int *secondary_weapon){
  // TODO
  return NULL;
}
