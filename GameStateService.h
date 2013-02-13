#ifndef GAMESTATESERVICE_H_
#define GAMESTATESERVICE_H_

#include "entity/Service.h"
#include "util/AnimationHelper.h"
#include "assets/Texture.h"
#include "entity/Container.h"
#include "GameState.h"
#include "Hexmap.h"

using namespace anengine;

class GameStateService : public Service, public AnimationHelperListner
{
    struct Player
    {
        uint Index;
        bool StatsDirty;
        bool StateDirty;
        std::string Name;
        uint Health;
        uint Score;
        anengine::VectorI2 Position;
        Movable *PlayerMovable;
        Visual *PlayerVisual;

        Player(uint index, std::string name, Movable *playerMovable, Visual *playerVisual)
            : Index(index), StatsDirty(true), StateDirty(true), Name(name), 
            Health(0), Score(0), Position(ZeroI2), 
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
    std::vector<Player>::iterator myCurrentPlayer;
    AssetRef<Texture> myFigureTexture;
    uint myActionCount;
    uint myActionCursor;
    ActionState myActionStates[3];

    void SetCurrentPlayer();
    bool StateUpdate(Event &event, InPin pin);
    void PlayAnimation();

    virtual void AnimationDone();
    public:
    GameStateService(MultiContainer *container, Hexmap *map,
            AssetRef<Texture> figureTexture) 
        : myAnimations(this), Turn(-1), myContainer(container), myMap(map), 
        myFigureTexture(figureTexture), myActionCount(0), myActionCursor(0)
    {
        RegisterInPin(SkyportEventClass::GameState, "StateUpdates", 
                static_cast<EventCallback>(&GameStateService::StateUpdate));
        myDonePin = RegisterOutPin(SkyportEventClass::GameState, "Done");
    }
    virtual ~GameStateService();
    
    virtual void OnUpdate(FrameTime time)
    {
        myAnimations.Update(time);
    }
};

#endif
