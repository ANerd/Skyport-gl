#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include "GameState.h"
#include "entity/Service.h"
#include "sndlib/sndlib.h"

using namespace anengine;

struct SoundEventCodes
{
    enum SoundEventCodes_t
    {
        Play
    };
};

enum class Sound
{
    Laser,
    MotarFire,
    MotarAir,
    MotarImpact,
    DroidFire,
    DroidStep,
    DroidImpact,
    RobotMining,
    RobotDestruction,
    RobotRespawn
};

class SoundEvent : public Event
{
    Sound mySound;
    real myDuration;

    public:
    SoundEvent(uint event, Entity *source, Sound sound, real duration=0)
        : Event(SkyportEventClass::Sound, event, source),
        mySound(sound), myDuration(duration) { }
    virtual ~SoundEvent() { }

    Sound GetSound()
    {
        return mySound;
    }

    real GetDuration()
    {
        return myDuration;
    }
};

class SoundManager : public Service
{
    bool OnSoundEvent(Event &event, InPin pin);
    protected:
    virtual void OnInitialize()
    {
        sndlib_init();
        sndlib_play_wind();
    }
    virtual void OnUninitialize()
    {
        sndlib_stop_wind();
        sndlib_quit();
    }
    public:
    SoundManager()
    {
        RegisterInPin(SkyportEventClass::Sound, "Sound", 
                static_cast<EventCallback>(&SoundManager::OnSoundEvent));
    }
    virtual ~SoundManager() { }
};

#endif
