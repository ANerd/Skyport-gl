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
#include "SoundManager.h"

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
    OutPin mySoundPin;

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

    Movable myMortarMov;
    Billboard myMortar;
    bool myInMortar;

    Movable myDroidMov;
    Billboard myDroid;
    int myDroidSequenceCounter;

    Movable myExplosionMov;
    Billboard myExplosion;

    VectorF4 myDefaultLookat;
    VectorF4 myDefaultCamera;
    VectorF4 myCameraTarget;

    void SetCurrentPlayer();
    bool StateUpdate(Event &event, InPin pin);
    void PlayAnimation();
    void MoveCamera(real time = 1, real dragTime = 0.5);
    void ForceMoveCamera(real time = 1, real dragTime = 0.5);
    void PlaySound(Sound sound, real duration = 0);

    virtual void AnimationDone();
    public:
    GameStateService(MultiContainer *container, Hexmap *map,
            AssetRef<Texture> figureTexture, AssetRef<Texture> laserTexture,
            AssetRef<Texture> mortarTexture, AssetRef<Texture> droidTexture,
            AssetRef<Texture> explosionTexture,
            Camera *camera);

    virtual ~GameStateService();
    
    virtual void OnUpdate(FrameTime time)
    {
        myAnimations.Update(time);
    }
};

#endif
