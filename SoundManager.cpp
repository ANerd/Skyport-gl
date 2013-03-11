
#include "SoundManager.h"

bool SoundManager::OnSoundEvent(Event &event, InPin pin)
{
    if(event.GetEvent() == SoundEventCodes::Play)
    {
        SoundEvent &sevent = dynamic_cast<SoundEvent&>(event);
        switch(sevent.GetSound())
        {
            case Sound::Laser:
                sndlib_play_laser(sevent.GetDuration()*1000);
                break;
            case Sound::MotarFire:
                sndlib_play_mortar_fire();
                break;
            case Sound::MotarAir:
                sndlib_play_mortar_air(sevent.GetDuration()*1000);
                break;
            case Sound::MotarImpact:
                sndlib_play_mortar_impact();
                break;
            case Sound::DroidFire:
                sndlib_play_droid_fire();
                break;
            case Sound::DroidStep:
                sndlib_play_droid_step();
                break;
            case Sound::DroidImpact:
                sndlib_play_droid_impact();
                break;
            case Sound::RobotMining:
                sndlib_play_robot_mining();
                break;
            case Sound::RobotRespawn:
                sndlib_play_robot_respawn();
                break;
            case Sound::RobotDestruction:
                sndlib_play_robot_destruction();
                break;
        }
    }
    return true;
}
