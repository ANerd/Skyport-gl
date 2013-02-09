#include "GameStateService.h"
#include "entity/Billboard.h"

void GameStateService::Player::Update(const PlayerState &other)
{
    StatsDirty = other.Health != Health || other.Score != Score;
    StateDirty = other.Position != Position;
    if(Name != other.Name)
        throw Error(Error::InvalidValue, "Updating name of player");
    Health = other.Health;
    Score = other.Score;
    Position = other.Position;
}

void GameStateService::Game::Update(const GameState &state, Hexmap *map)
{
    VectorI2 mapSize = state.GetMap().GetSize();
    if(Turn == -1)
    {
        for(auto pit = state.Players_begin(); 
                pit != state.Players_end(); pit++)
        {
            Movable *mov;
            Billboard *bill;
            Players.push_back(Player(pit->Name,pit->Health,
                        pit->Score,pit->Position,mov,bill));
        }
        map->Create(mapSize[X],mapSize[Y]);
    }
    else
    {
        int i = 0;
        for(auto pit = state.Players_begin(); 
                pit != state.Players_end(); pit++)
            Players[i++].Update(*pit);
    }
    for(int j = 0;  j < mapSize[X]; j++)
    {
        for(int k = 0;  k < mapSize[Y]; k++)
        {
            map->SetTileType(j,k,state.GetMap()(j,k));
        }
    }
    Turn = state.GetTurn();
}

bool GameStateService::StateUpdate(Event &event, InPin pin)
{
    GameStateEvent &gevent = dynamic_cast<GameStateEvent&>(event);
    myGameState.Update(gevent.GetState(), myMap);
    Debug("Updated gamesate");
    Event nevent(SkyportEventClass::GameState, 
            GameStateEventCodes::StateProcessed, this);
    myDonePin.Send(nevent);
    return true;
}
