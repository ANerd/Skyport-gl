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
  
#define SNDLIB_SOUND_MORTAR_FIRE "mortar-fire.wav"
#define SNDLIB_SOUND_MORTAR_AIR "mortar-air.wav"
#define SNDLIB_SOUND_MORTAR_IMPACT "mortar-impact.wav"
  
#define SNDLIB_SOUND_DROID_LAUNCH "droid-launch.wav"
#define SNDLIB_SOUND_DROID_STEP "droid-step.wav"
#define SNDLIB_SOUND_DROID_IMPACT "droid-impact.wav"
  
#define SNDLIB_SOUND_ROBOT_DESTRUCTION "robot-destruction.wav"
#define SNDLIB_SOUND_ROBOT_MOVEMENT "robot-movement.wav"
#define SNDLIB_SOUND_ROBOT_MINING "robot-mining.wav"

  
  /**
   * Initialize the sound library and load & decode
   * all sounds from disk. May take a while.
   * Use: call before doing any other sndlib calls.
   * May take up to 10 seconds to load all sounds.
   */
  void sndlib_init(void);

  
  /**
   * Finish playing sounds and free up all audio-
   * related resources.
   * Use: call once you're done playing sounds.
   */
  void sndlib_quit(void);

  
  /**
   * Play the wind sound.
   * Duration: loops infinitely
   * Use: Use liberally whenever.
   */
  void sndlib_play_wind(void);

  /**
   * Stop the wind sound.
   */
  void sndlib_stop_wind(void);

  
  /**
   * Play the wind sound.
   * Paramter "milliseconds": number of milliseconds to play this sound.
   * Use: Whenever displaying the laser on the screen, for however long
   *      the laser is visible on the screen.
   */
  void sndlib_play_laser(int milliseconds);

  
  /**
   * Play the mortar firing sound.
   * Duration: ?
   * Use: Play whenever a player just fired the mortar.
   */
  void sndlib_play_mortar_fire(void);

  
  /**
   * Play the mortar air sound.
   * milliseconds: number of milliseconds to play this sound.
   * Use: Play whenever displaying the mortar projective flying
   *      through the air, for however long it is visible.
   */
  void sndlib_play_mortar_air(int milliseconds);

  
  /**
   * play the mortar impact sound.
   * Duration: ?
   * Use: Play whenever the mortar hits. Play only once, not
   *      several times for each AoE-damaged tile.
   */
  void sndlib_play_mortar_impact(void);

  
  /**
   * Play the droid firing sound.
   * Duration: ?
   * Use: Play whenever the droid is released by a player.
   */
  void sndlib_play_droid_fire(void);


  /**
   * Play the droid step sound.
   * Duration: ?
   * Use: Play whenever the droid takes one step (tile.)
   */
  void sndlib_play_droid_step(void);


  /**
   * Play the droid impact sound.
   * Duration: ?
   * Use: Play whenever the droid reaches its destination
   *      and explodes. Play only once, not several times
   *      for each AoE-damaged tile.
   */
  void sndlib_play_droid_impact(void);


  /**
   * Play the robot destruction sound.
   * Duration: ?
   * Use: Play whenever a player reaches 0 health
   */
  void sndlib_play_robot_destruction(void);


  /**
   * Play the robot respawn sound.
   * Duration: ?
   * Use: Play whenever a player is reset to his spawn
   */
  void sndlib_play_robot_respawn(void);


  /**
   * Play the robot mining sound.
   * Duration: ?
   * Use: Play whenever a player mines a resource.
   */
  void sndlib_play_robot_mining(void);
  
#ifdef __cplusplus
}
#endif
  
#endif
