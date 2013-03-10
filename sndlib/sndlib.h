#ifndef TEXTLIB_H_
#define TEXTLIB_H_
#include <SDL.h>
#include <SDL_mixer.h>

#ifdef __cplusplus
extern "C" {
#endif

  // mortar sounds
#define SNDLIB_SOUND_WIND "wind.wav"
#define SNDLIB_SOUND_LASER "laser.wav"
  /*
    #define SNDLIB_SOUND_MORTAR_BEGIN "mortar-begin.wav"
    #define SNDLIB_SOUND_MORTAR_MIDDLE "mortar-main.wav"
    #define SNDLIB_SOUND_MORTAR_END "mortar-end.wav"

    
  */

  /**
   * Initialize the sound library and load & decode
   * all sounds from disk. May take a while.
   */
  void sndlib_init(void);

  /**
   * Finish playing sounds and free up all audio-
   * related resources.
   */
  void sndlib_quit(void);

  /**
   * Play the wind noise.
   * Duration: infinite
   */
  void sndlib_play_wind(void);
  /**
   * Play the wind noise.
   * Duration: 
   */
  void sndlib_play_laser(void);
  
#ifdef __cplusplus
}
#endif
  
#endif
