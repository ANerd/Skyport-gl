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
        int Turn;
        std::vector<Player> Players;

        Game() : Turn(-1) { }

        void Update(const GameState &state, Hexmap *map);
    };

    AnimationHelper myAnimations;
    MultiContainer *myContainer;
    Hexmap *myMap;
    Game myGameState;
    OutPin myDonePin;
    bool StateUpdate(Event &event, InPin pin);
    public:
    GameStateService(MultiContainer *container, Hexmap *map) 
        : myContainer(container), myMap(map) 
    {
        RegisterInPin(SkyportEventClass::GameState, "StateUpdates", 
                static_cast<EventCallback>(&GameStateService::StateUpdate));
        myDonePin = RegisterOutPin(SkyportEventClass::GameState, "Done");
    }
    
    virtual void Update(FrameTime time)
    {
        myAnimations.Update(time);
    }
};

#endif
