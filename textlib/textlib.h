#ifndef TEXTLIB_H_
#define TEXTLIB_H_
#include <SDL.h>
#include <SDL_ttf.h>

#define FONT_FILE "fonts/cowboys/cowboys.ttf"
#define FONT_DPI 10
#define FONT_QUALITY_LEVEL 3

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
 * Sets the used font-size in points, based on 72dpi.
 */
void textlib_set_font_size(int);

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
SDL_Surface *textlib_get_text(char *);


#endif
