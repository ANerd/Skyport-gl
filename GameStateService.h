#ifndef GAMESTATESERVICE_H_
#define GAMESTATESERVICE_H_

#include "entity/Service.h"
#include "util/AnimationHelper.h"
#include "assets/Texture.h"
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
    void Update(const GameState &state);

    AnimationHelper myAnimations;
    OutPin myDonePin;

    int Turn;
    std::vector<Player> Players;
    MultiContainer *myContainer;
    Hexmap *myMap;
    AssetRef<Texture> myFigureTexture;

    bool StateUpdate(Event &event, InPin pin);
    public:
    GameStateService(MultiContainer *container, Hexmap *map,
            AssetRef<Texture> figureTexture) 
        : Turn(-1), myContainer(container), myMap(map), 
        myFigureTexture(figureTexture)
    {
        RegisterInPin(SkyportEventClass::GameState, "StateUpdates", 
                static_cast<EventCallback>(&GameStateService::StateUpdate));
        myDonePin = RegisterOutPin(SkyportEventClass::GameState, "Done");
    }
    virtual ~GameStateService();
    
    virtual void Update(FrameTime time)
    {
        myAnimations.Update(time);
    }
};

#endif
