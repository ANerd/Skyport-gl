#ifndef TEXTLIB_H_
#define TEXTLIB_H_
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif
  
#define DEFAULT_FONT_FILE "assets/fonts/cowboys/cowboys.ttf"
#define DEFAULT_FONT_DPI 10
#define DEFAULT_FONT_QUALITY_LEVEL 0

/**
 * Lowest-quality font rendering setting.
 * Uses no shading or blending. Fastest.
 */
#define TEXT_QUALITY_LOW 0

/**
 * Medium-quality font rendering setting.
 * Uses Shading, so it requires you to
 * use textlib_set_background_color()
 * first to set a background-color.
 */
#define TEXT_QUALITY_MEDIUM 1

/**
 * High-quality font rendering setting.
 * Uses blending, so it returns a 32-bit
 * SDL_Surface with per-pixel alpha
 * transparency values. Slowest.
 */
#define TEXT_QUALITY_HIGH 2

/**
 * Initializes the library. Needs to be called before doing
 * any other text-related operations.
 */
void textlib_initialize(void);

/**
 * Quits the library. Should be called at the end of your
 * program.
 */
void textlib_quit(void);

/**
 * Sets the used font-size and font-file. font-size
 * is in points relative to 72dpi. The font-file
 * can be any .ttf file or NULL, in which case
 * a default font file specified in DEFAULT_FONT_FILE
 * is used instead.
 */
void textlib_set_font(int, const char*);

/**
 * Sets the background color to use for text-rendering.
 * This has only an effect if the text-rendering quality
 * level is set to TEXT_QUALITY_SHADED.
 */
void textlib_set_background_color(Uint8, Uint8, Uint8);

/**
 * Sets the quality to use for rendering
 * Possible settings:
 * TEXT_QUALITY_LOW, TEXT_QUALITY_MEDIUM,
 * TEXT_QUALITY_HIGH.
 * Note that TEXT_QUALITY_MEDIUM requires
 * you to set a background-color.
 */
void textlib_set_quality(unsigned int the_quality);

/**
 * Renders a C string onto a SDL_Surface
 * with the given settings.
 */
SDL_Surface *textlib_get_text(const char *, Uint8, Uint8, Uint8);

/**
 * Renders a nametag + health-bar to
 * a SDL_Surface. The name should not
 * contain more than 16 letters, and
 * health is a float 0 <= health <= 1.
 */
SDL_Surface *textlib_get_nametag(const char *name, float health);

/**
 * Renders a table of statistics about
 * each player to an opaque surface.
 * 
 *
 */
SDL_Surface *textlib_get_stats(unsigned int players, const char **names,
			       int *points, const char **primary_weapon, const char **secondary_weapon, int screen_width);

/**
 * Renders a fullscreen scoreboard.
 *
 */
  SDL_Surface *textlib_get_finalscreen(unsigned int players, const char **names, int *points, char **primary_weapons, int *primary_weapon_lvls, char **secondary_weapons, int *secondary_weapon_lvls, int w, int h);
  
#ifdef __cplusplus
}
#endif
  
#endif
