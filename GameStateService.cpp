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

void GameStateService::Game::Update(const GameState &state)
{
    if(Frame == -1)
    {
        for(auto pit = state.Players_begin(); 
                pit != state.Players_end(); pit++)
        {
            Movable *mov;
            Billboard *bill;
            Players.push_back(Player(pit->Name,pit->Health,
                        pit->Score,pit->Position,mov,bill));
        }
    }
    else
    {
        int i = 0;
        for(auto pit = state.Players_begin(); 
                pit != state.Players_end(); pit++)
            Players[i++].Update(*pit);
    }
    Frame = state.GetFrame();
}

bool GameStateService::StateUpdate(Event &event, InPin pin)
{
    GameStateEvent &gevent = dynamic_cast<GameStateEvent&>(event);
    myGameState.Update(gevent.GetState());
    return true;
}
