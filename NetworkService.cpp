#include "NetworkService.h"
#include <unistd.h>
#include <signal.h>

void NetworkService::OnUpdate(FrameTime time)
{
    pthread_mutex_lock(&myGameSateLock);
    bool newGameState(myNewGameState);
    GameState gameState;
    if(myNewGameState)
        gameState = myGameState;
    myDone &= !myNewGameState;
    myNewGameState = false;
    pthread_mutex_unlock(&myGameSateLock);
    if(newGameState)
    {
        GameStateEvent event(GameStateEventCodes::NewGameState, this, gameState);
        myGameStatePin.Send(event);
    }
}

void NetworkService::OnInitialize()
{
    if(pthread_create(&myNetworkThread, NULL, &sNetworkMain, this) != 0)
        throw Error(Error::InternalError, "Failed to create thread");
}
void NetworkService::OnUninitialize()
{
    void *retval;
    Debug("Set quit network");
    pthread_mutex_lock(&myGameSateLock);
    myQuit = true;
    pthread_cond_broadcast(&myDoneCond);;
    pthread_mutex_unlock(&myGameSateLock);
    pthread_kill(myNetworkThread, SIGINT);
    if(pthread_join(myNetworkThread, &retval) != 0)
        throw Error(Error::InternalError, "Failed to join thread");
}

bool NetworkService::StateUpdate(Event &event, InPin pin)
{
    if(event.GetEvent() == GameStateEventCodes::StateProcessed)
    {
        pthread_mutex_lock(&myGameSateLock);
        myDone = true;
        if(!myNewGameState)
            pthread_cond_signal(&myDoneCond);
        pthread_mutex_unlock(&myGameSateLock);
    }
    return true;
}

void *NetworkService::sNetworkMain(void *arg)
{
    return static_cast<NetworkService*>(arg)->NetworkMain();
}
void *NetworkService::NetworkMain()
{
    GameState gameState;
    myTransport.Connect(myHost, myPort);
    myProtocol.Initialize();
    pthread_mutex_lock(&myGameSateLock);
    while(!myQuit)
    {
        pthread_mutex_unlock(&myGameSateLock);
        bool b = myProtocol.UpdateGamesate(gameState);
        pthread_mutex_lock(&myGameSateLock);
        if(!myQuit)
        {
            if(b)
            {
                Debug("N: Got gamesate");
                myNewGameState = true;
            }
            else
            {
                while(!myDone || myNewGameState)
                {
                    if(myQuit)
                        break;
                    Debug("N: Waiting for done");
                    pthread_cond_wait(&myDoneCond, &myGameSateLock);
                }
                if(!myQuit)
                    myProtocol.NotifyDone();
            }
        }
    };
    pthread_mutex_unlock(&myGameSateLock);
    myProtocol.Uninitialize();
    myTransport.Disconnect();
    Debug("N: Network down");
    return 0;
}

