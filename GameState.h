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
        GameState = 0x40000000
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

class PlayerState
{
    public:
    std::string Name;
    uint Health;
    uint Score;
    anengine::VectorI2 Position;
    PlayerState(std::string name, uint health, uint score, anengine::VectorI2 pos)
        : Name(name), Health(health), Score(score), Position(pos) { }
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

    MapState &operator= (const MapState &other)
    {
        delete [] myData;
        mySize = other.mySize;
        myData = new char[mySize[X]*mySize[Y]];
        std::copy(other.myData, other.myData + mySize[X]*mySize[Y], myData);
        return *this;
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
    int myFrame;
    public:
    typedef std::vector<PlayerState>::const_iterator Players_iterator;
    GameState()
        : myFrame(-1) { }
    void SetPlayer(std::string name, uint health, uint score, 
            anengine::VectorI2 pos);

    void BeginFrame(int frame)
    {
        myFrame = frame;
    }
    void EndFrame()
    {
    }

    int GetFrame() const
    {
        return myFrame;
    }
    Players_iterator Players_begin() const
    {
        return myPlayers.begin();
    }
    Players_iterator Players_end() const
    {
        return myPlayers.end();
    }
    const MapState &GetMap() const
    {
        return myMap;
    }
    void SetMap(const MapState &map)
    {
        myMap = map;
    }
};

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
