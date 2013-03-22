#include "GameState.h"

const ColorF PlayerState::Colors[16] = {
  ColorF(1.000, 0.243, 0.247, 1.00),
  ColorF(1.000, 0.310, 0.882, 1.00),
  ColorF(0.709, 0.313, 1.000, 1.00),
  ColorF(0.313, 0.329, 1.000, 1.00),
  ColorF(0.372, 0.760, 1.000, 1.00),
  ColorF(0.372, 0.996, 1.000, 1.00),
  ColorF(0.274, 1.000, 0.290, 1.00),
  ColorF(0.745, 1.000, 0.274, 1.00),
  ColorF(0.988, 1.000, 0.231, 1.00),
  ColorF(1.000, 0.725, 0.031, 1.00),
  ColorF(0.000, 0.000, 0.000, 1.00),
  ColorF(1.000, 1.000, 1.000, 1.00),
  ColorF(0.847, 0.847, 0.847, 1.00),
  ColorF(0.635, 0.635, 0.635, 1.00),
  ColorF(0.427, 0.427, 0.427, 1.00),
  ColorF(0.278, 0.278, 0.278, 1.00)
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
