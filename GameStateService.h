#ifndef GAMESTATESERVICE_H_
#define GAMESTATESERVICE_H_

#include "entity/Service.h"
#include "util/AnimationHelper.h"
#include "assets/Texture.h"
#include "entity/Container.h"
#include "entity/Marker.h"
#include "entity/PointVisualizer.h"
#include "GameState.h"
#include "Statusbox.h"
#include "Nametag.h"
#include "Textbox.h"
#include "Hexmap.h"
#include "Laser.h"
#include "Meteor.h"

using namespace anengine;

class GameStateService : public Service, public AnimationHelperListner
{
    static const uint MeteorCount = 1;
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
        Movable *NametagMovable;
        MultiContainer *PlayerContainer;
        Nametag *PlayerNametag;

        Player(uint index, std::string name, Movable *playerMovable, 
                Visual *playerVisual, Movable *nametagMovable,
                MultiContainer *playerContainer, Nametag *nametag)
            : Index(index), StatsDirty(true), StateDirty(true), Name(name), 
            Health(0), Score(0), Position(ZeroI2), 
            PlayerMovable(playerMovable), PlayerVisual(playerVisual),
            NametagMovable(nametagMovable), PlayerContainer(playerContainer),
            PlayerNametag(nametag) { }

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
    Statusbox *myStats;
    Textbox myTitle;
    Textbox mySubtitle;
    std::vector<Player>::iterator myCurrentPlayer;
    AssetRef<Texture> myFigureTexture;
    uint myActionCount;
    uint myActionCursor;
    ActionState myActionStates[3];
    AnimationHelper::AnimationIndex mySubtitleAnimation;

    Movable myCamMov;
    Camera *myCamera;
    MultiContainer myCamMarkerC;
    Movable myCamMarkerMov;
    Marker myCamMarker;
    PointVisualizer myCamMarkerViz;

    Meteor myMeteors[MeteorCount];
    Movable myMeteorMovs[MeteorCount];

    Movable myLaserMov;
    Laser myLaser;
    MatrixF4 myLaserBaseTransform;

    Movable myMotarMov;
    Billboard myMotar;

    VectorF4 myDefaultLookat;
    VectorF4 myDefaultCamera;
    VectorF4 myCameraTarget;

    void SetCurrentPlayer();
    bool StateUpdate(Event &event, InPin pin);
    void PlayAnimation();
    void MoveCamera(real time = 1, real dragTime = 0.5);

    virtual void AnimationDone();
    public:
    GameStateService(MultiContainer *container, Hexmap *map,
            AssetRef<Texture> figureTexture, AssetRef<Texture> laserTexture,
            Camera *camera);

    virtual ~GameStateService();
    
    virtual void OnUpdate(FrameTime time)
    {
        myAnimations.Update(time);
    }
};

#endif
