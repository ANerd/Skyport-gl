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
  textlib_set_font(24, NULL);
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

  SDL_Rect name_rect = {(int)round(NAMETAG_WIDTH/2.0 - (nametag->w)/2), 3, nametag->w, nametag->h};
  SDL_BlitSurface(nametag, NULL, bg, &name_rect);
  
  font = oldfont;
  quality = oldquality;
  SDL_FreeSurface(nametag);
  return bg;
}

SDL_Surface *_textlib_get_playerstats(const char *, int, const char *, int, const char *, int, bool);
SDL_Surface *_textlib_get_playerstats(const char *name, int points, const char *primary, 
				      int primary_lvl, const char *secondary, int secondary_lvl, bool last){
  char string[1024];
  if(last)
    snprintf(string, 1024, "%s: %d pts", name, points); 
  else
    snprintf(string, 1024, "%s: %d pts,", name, points); 
  return textlib_get_text(string, 255, 255, 255);
}

SDL_Surface *textlib_get_stats(unsigned int players, const char **names,
			       int *points, const char **primary_weapon, const char **secondary_weapon, int screen_width){
  if(players == 0){
    return NULL;
  }
  unsigned int extra_padding;
  bool two_lines;
  int font_size = 14;
  int height = 64;

  extra_padding = (unsigned int)(48 - players*2.5);

  if(players <= 4)
    font_size = 28;
  if(players > 4 && players <= 8)
    font_size = 24;
  if(players > 8 && players <= 12)
    font_size = 19;
  if(players > 12 && players <= 14)
    font_size = 16;

  TTF_Font *oldfont = font;
  unsigned int oldquality = quality;
  textlib_set_font(font_size, NULL);
  textlib_set_quality(TEXT_QUALITY_HIGH);

  SDL_Surface *surfaces[players];
  int total_width = 0;
  int extra_width = 0;
  for(int i = 0; i < players; i++){
    surfaces[i] = _textlib_get_playerstats(names[i], points[i], primary_weapon[i], 1, secondary_weapon[i], 1, i == players-1);
    total_width += surfaces[i]->w;
    extra_width += surfaces[i]->w + extra_padding;
  }
  SDL_Surface *bg = SDL_CreateRGBSurface(SDL_SWSURFACE,
					 screen_width, height,
					 surfaces[0]->format->BitsPerPixel,
					 surfaces[0]->format->Rmask, surfaces[0]->format->Gmask,
					 surfaces[0]->format->Bmask, surfaces[0]->format->Amask);
  SDL_Rect inner = {2, 2, screen_width - 4, height - 4};
  SDL_FillRect(bg, NULL, SDL_MapRGBA(bg->format, 0, 0, 0, 255));
  SDL_FillRect(bg, &inner, SDL_MapRGBA(bg->format, 128, 128, 255, 255));
  int first_row_y_displacement = (int)((height - 2*surfaces[0]->h)/3.0);
  int second_row_y_displacement = first_row_y_displacement*2 + surfaces[0]->h;

  if(extra_width > screen_width){ // split in two
    int top_row_players = (int)(players/2.0);
    int first_row_width = 0;
    for(int i = 0; i < top_row_players; i++)
      first_row_width += surfaces[i]->w + extra_padding;
    int base_x_position = (int)((screen_width - first_row_width)/2.0);
    int x_position = base_x_position;
    for(int i = 0; i < top_row_players; i++){
      SDL_Rect placement = {x_position, first_row_y_displacement, surfaces[i]->w, surfaces[i]->h};
      SDL_BlitSurface(surfaces[i], NULL, bg, &placement);
      SDL_FreeSurface(surfaces[i]);
      x_position += surfaces[i]->w + extra_padding;
    }
    
    int second_row_width = 0;
    for(int i = top_row_players; i < players; i++)
      second_row_width += surfaces[i]->w + extra_padding;
    
    x_position = (int)((screen_width - second_row_width)/2.0);
    printf("x_position for second row: %d\n", x_position);
    for(int i = top_row_players; i < players; i++){
      SDL_Rect placement = {x_position, second_row_y_displacement, surfaces[i]->w, surfaces[i]->h};
      SDL_BlitSurface(surfaces[i], NULL, bg, &placement);
      SDL_FreeSurface(surfaces[i]);
      x_position += surfaces[i]->w + extra_padding;
    }
  }
  else { // all players in one line
    int total_width = 0;
    for(int i = 0; i < players; i++){
      total_width += surfaces[i]->w + extra_padding;
    }
    int x_position = (int)((screen_width - total_width)/2.0);
    for(int i = 0; i < players; i++){
      int y_displacement = (int)((height - surfaces[i]->h)/2.0);
      SDL_Rect placement = {x_position, y_displacement, surfaces[i]->w, surfaces[i]->h};
      SDL_BlitSurface(surfaces[i], NULL, bg, &placement);
      SDL_FreeSurface(surfaces[i]);
      x_position += surfaces[i]->w + extra_padding;
    }
  }

  // restore old font settings
  font = oldfont;
  quality = oldquality;
  return bg;
}


SDL_Surface *textlib_get_finalscreen(unsigned int players, const char **names, int *points, 
				     char **primary_weapons, int *primary_weapon_lvls, 
				     char **secondary_weapons, int *secondary_weapon_lvls, 
				     int w, int h){
  SDL_Surface *quickhack = textlib_get_text("foobar", 0, 0, 0);
  SDL_Surface *finalscreen = SDL_CreateRGBSurface(SDL_SWSURFACE,
					 w, h,
					 quickhack->format->BitsPerPixel,
					 quickhack->format->Rmask, quickhack->format->Gmask,
					 quickhack->format->Bmask, quickhack->format->Amask);
  return finalscreen;
}

