#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include <vector>
#include "math/Vector.h"
#include "event/Event.h"
#include "core/Error.h"

using namespace anengine;
struct SkyportEventClass
{
    enum SkyportEventClass_t
    {
        GameState = 0x40000000,
        Sound = 0x40000001,
    };
};

struct GameStateEventCodes
{
    enum GameStateEventCodes_t
    {
        NewGameState,
        StateProcessed
    };
};

enum class SkyportAction
{
    None,
    Move,
    Pass,
    Upgrade,
    Mine,
    Laser,
    Motar,
    Droid
};

enum class Direction
{
    None,
    Up,
    Down,
    Right_Up,
    Left_Up,
    Left_Down,
    Right_Down
};

enum class Weapon
{
    Laser,
    Motar,
    Droid
};

class ActionState
{
    public:
    static const int MaxDroidCommands = 5;
    private:
    SkyportAction myAction;
    Direction myDirection;
    Weapon myWeapon;
    VectorI2 myCoordinate;
    Direction myCommands[MaxDroidCommands];
    ActionState(SkyportAction action) : myAction(action) { }
    public:
    ActionState() : myAction(SkyportAction::None) { }
    static ActionState CreateMovement(Direction dir)
    {
        ActionState state(SkyportAction::Move);
        state.myDirection = dir;
        return state;
    }
    static ActionState CreatePass()
    {
        return ActionState(SkyportAction::Pass);
    }
    static ActionState CreateUpgrade(Weapon wep)
    {
        ActionState state(SkyportAction::Upgrade);
        state.myWeapon = wep;
        return state;
    }
    static ActionState CreateMine()
    {
        return ActionState(SkyportAction::Mine);
    }
    static ActionState CreateLaser(Direction dir, VectorI2 offset)
    {
        ActionState state(SkyportAction::Laser);
        state.myDirection = dir;
        state.myCoordinate = offset;
        return state;
    }
    static ActionState CreateMotar(VectorI2 coord)
    {
        ActionState state(SkyportAction::Motar);
        state.myCoordinate = coord;
        return state;
    }
    static ActionState CreateDroid(Direction *commands, int count)
    {
        ActionState state(SkyportAction::Droid);
        if(count > MaxDroidCommands)
            throw Error(Error::InvalidValue, "Too many droid actions");
        for(int i = 0; i < count; i++)
            state.myCommands[i] = commands[i];
        for(int i = count; i < MaxDroidCommands; i++)
            state.myCommands[i] = Direction::None;
        return state;
    }

    SkyportAction GetAction() { return myAction; }
    Direction GetDirection() { return myDirection; }
    Weapon GetWeapon() { return myWeapon; }
    VectorI2 GetCoordinate() { return myCoordinate; }
    Direction *GetCommands() { return myCommands; }
};

uint StrSum(std::string name);
class PlayerState
{
    static const ColorF Colors[16];
    public:
    uint Index;
    std::string Name;
    uint Health;
    uint Score;
    Weapon PrimaryWeapon;
    Weapon SecondaryWeapon;
    anengine::VectorI2 Position;
    ColorF Color;
    PlayerState(uint index, std::string name, uint health, uint score,
            Weapon primary, Weapon secondary, anengine::VectorI2 pos)
        : Index(index), Name(name), Health(health), Score(score),
        PrimaryWeapon(primary), SecondaryWeapon(secondary), Position(pos), 
        Color(Colors[StrSum(name) % 16]) { }
    ~PlayerState() { }
};

class MapState
{
    VectorI2 mySize;
    char *myData;
    public:
    MapState(const MapState &other)
        : mySize(other.mySize), myData(new char[mySize[X]*mySize[Y]])
    {
        std::copy(other.myData, other.myData + mySize[X]*mySize[Y], myData);
    }
    MapState(VectorI2 size)
        : mySize(size), myData(new char[size[X]*size[Y]]) { }
    MapState()
        : mySize(ZeroI2), myData(new char[0]) { }
    ~MapState()
    {
        delete [] myData;
    }

    MapState &operator= (const MapState &other)
    {
        delete [] myData;
        mySize = other.mySize;
        myData = new char[mySize[X]*mySize[Y]];
        std::copy(other.myData, other.myData + mySize[X]*mySize[Y], myData);
        return *this;
    }

    VectorI2 GetSize() const
    {
        return mySize;
    }

    char operator ()(int j, int k) const
    {
        return myData[k*mySize[X]+j];
    }
    char &operator ()(int j, int k)
    {
        return myData[k*mySize[X]+j];
    }
};

class GameState
{
    std::vector<PlayerState> myPlayers;
    MapState myMap;
    int myTurn;
    int myActionCount;
    int myPlayerIndex;
    std::string myTitle;
    std::string mySubtitle;
    ActionState myActions[3];
    public:
    typedef std::vector<PlayerState>::const_iterator Players_iterator;
    GameState()
        : myTurn(-1), myActionCount(0), myPlayerIndex(0) { }
    void SetPlayer(std::string name, uint health, uint score, Weapon primary,
            Weapon secondary, anengine::VectorI2 pos);

    void SetTurn(int turn)
    {
        if(myTurn == -1 && turn != 0)
            throw Error(Error::InvalidValue, "Got no turn 0.");
        if(myTurn != -1 && turn == 0)
            throw Error(Error::InvalidValue, "Got multiple turn 0.");
        myTurn = turn;
        myActionCount = 0;
        myPlayerIndex = 0;
    }

    int GetTurn() const
    {
        return myTurn;
    }
    Players_iterator Players_begin() const
    {
        return myPlayers.begin();
    }
    Players_iterator Players_end() const
    {
        return myPlayers.end();
    }
    
    int PlayerCount() const
    {
        return myPlayers.size();
    }

    const MapState &GetMap() const
    {
        return myMap;
    }
    void SetMap(const MapState &map)
    {
        myMap = map;
    }

    void AddAction(ActionState action)
    {
        if(myActionCount >= 3)
            throw Error(Error::InvalidState, "Too many actions in one turn.");
        myActions[myActionCount++] = action;
    }

    uint GetActionCount() const
    {
        return myActionCount;
    }

    const ActionState &GetAction(uint i) const
    {
        Bug(i >= 3, "Request wrong action");
        return myActions[i];
    }

    void SetTitle(std::string title)
    {
        myTitle = title;
    }

    void SetSubtitle(std::string subtitle)
    {
        mySubtitle = subtitle;
    }

    std::string GetTitle() const
    {
        return myTitle;
    }

    std::string GetSubtitle() const
    {
        return mySubtitle;
    }

    bool operator == (const GameState &other) const
    {
        return false;
    }
    bool operator != (const GameState &other) const
    {
        return true;
    }
};

const char *GetWeaponName(Weapon wep);

class GameStateEvent : public Event
{
    GameState myState;
    public:
    GameStateEvent(uint event, Entity *source, const GameState &state)
        : Event(SkyportEventClass::GameState, event, source), myState(state) { }

    const GameState &GetState()
    {
        return myState;
    }
};

#endif
