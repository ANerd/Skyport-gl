#ifndef GAMESTATESERVICE_H_
#define GAMESTATESERVICE_H_

#include "entity/Service.h"
#include "util/AnimationHelper.h"
#include "entity/Container.h"
#include "GameState.h"
#include "Hexmap.h"

using namespace anengine;

class GameStateService : public Service
{
    struct Player
    {
        bool StatsDirty;
        bool StateDirty;
        std::string Name;
        uint Health;
        uint Score;
        anengine::VectorI2 Position;
        Movable *PlayerMovable;
        Visual *PlayerVisual;

        Player(std::string name, uint health, uint score, VectorI2 pos, 
                Movable *playerMovable, Visual *playerVisual)
            : StatsDirty(true), StateDirty(true), Name(name), 
            Health(health), Score(score), Position(pos), 
            PlayerMovable(playerMovable), PlayerVisual(playerVisual) { }

        ~Player() { }

        void Update(const PlayerState &other);
    };
    struct Game
    {
        int Frame;
        std::vector<Player> Players;

        Game() : Frame(-1) { }

        void Update(const GameState &state);
    };

    AnimationHelper myAnimations;
    MultiContainer *myContainer;
    Hexmap *myMap;
    Game myGameState;
    bool StateUpdate(Event &event, InPin pin);
    public:
    GameStateService(MultiContainer *container, Hexmap *map) 
        : myContainer(container), myMap(map) 
    {
        RegisterInPin(SkyportEventClass::GameState, "StateUpdates", 
                static_cast<EventCallback>(&GameStateService::StateUpdate));
    }
    
    virtual void Update(FrameTime time)
    {
        myAnimations.Update(time);
    }
};

#endif
