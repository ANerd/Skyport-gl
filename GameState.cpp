#include "GameState.h"

const ColorF PlayerState::Colors[16] = {
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
};

uint StrSum(std::string name)
{
    uint sum = 0;
    for(auto it = name.begin(); it != name.end(); it++)
    {
        sum += *it;
    }
    return sum;
}

void GameState::SetPlayer(std::string name, uint health, uint score, 
        anengine::VectorI2 pos)
{
    if(myTurn == 0)
    {
        myPlayers.push_back(PlayerState(myPlayerIndex++, name, health, score, pos));
    }
    else
    {
        for(auto it = myPlayers.begin(); it != myPlayers.end(); it++)
        {
            if(it->Name == name)
            {
                it->Health = health;
                it->Score = score;
                it->Position = pos;
                it->Index = myPlayerIndex++;
                break;
            }
        }
    }
}
