#include "GameState.h"

void GameState::SetPlayer(std::string name, uint health, uint score, 
        anengine::VectorI2 pos)
{
    if(myTurn == -1)
        myPlayers.push_back(PlayerState(name, health, score, pos));
    else
    {
        for(auto it = myPlayers.begin(); it != myPlayers.end(); it++)
        {
            if(it->Name == name)
            {
                it->Health = health;
                it->Score = score;
                it->Position = pos;
                break;
            }
        }
    }
}
