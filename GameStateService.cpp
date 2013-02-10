#include "GameStateService.h"
#include "entity/Billboard.h"

void GameStateService::Player::Update(const PlayerState &other)
{
    if(Name != other.Name)
        throw Error(Error::InvalidValue, "Updating name of player");

    StatsDirty = other.Health != Health || other.Score != Score;
    StateDirty = other.Position != Position;
    if(StatsDirty)
    {
        Health = other.Health;
        Score = other.Score;
    }
    if(StateDirty)
    {
        Position = other.Position;
        VectorF2 pos(
                Hexmap::jOffset[X]*Position[X]+Hexmap::kOffset[X]*Position[Y],
                Hexmap::jOffset[Y]*Position[X]+Hexmap::kOffset[Y]*Position[Y]);
        PlayerMovable->Transform.Set(
                MatrixF4::Translation(VectorF4(pos[X],0.5,pos[Y])));
    }
}

GameStateService::~GameStateService()
{
    for(auto pit = Players.begin(); 
            pit != Players.end(); pit++)
    {
        delete pit->PlayerMovable;
        delete pit->PlayerVisual;
    }
}

void GameStateService::Update(const GameState &state)
{
    VectorI2 mapSize = state.GetMap().GetSize();
    if(Turn == -1)
    {
        for(auto pit = state.Players_begin(); 
                pit != state.Players_end(); pit++)
        {
            Movable *mov = new Movable();
            Billboard *bill = new Billboard(myFigureTexture);
            mov->SetChild(bill);
            myContainer->AddChild(mov);
            bill->ProgramState().SetUniform("Z", -0.05f);
            Players.push_back(Player(pit->Name,mov,bill));
            Players.back().Update(*pit);
        }
        myMap->Create(mapSize[X],mapSize[Y]);
    }
    else
    {
        int i = 0;
        for(auto pit = state.Players_begin(); 
                pit != state.Players_end(); pit++)
        {
            Players[i++].Update(*pit);
        }
    }
    for(int j = 0;  j < mapSize[X]; j++)
    {
        for(int k = 0;  k < mapSize[Y]; k++)
        {
            myMap->SetTileType(j,k,state.GetMap()(j,k));
        }
    }
    Turn = state.GetTurn();
}

bool GameStateService::StateUpdate(Event &event, InPin pin)
{
    GameStateEvent &gevent = dynamic_cast<GameStateEvent&>(event);
    Update(gevent.GetState());
    Debug("Updated gamesate");
    Event nevent(SkyportEventClass::GameState, 
            GameStateEventCodes::StateProcessed, this);
    myDonePin.Send(nevent);
    return true;
}
