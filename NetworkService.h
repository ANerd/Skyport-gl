#ifndef NETWORKSERVICE_H_
#define NETWORKSERVICE_H_

#include <pthread.h>
#include "entity/Service.h"
#include "ProtocolHandler.h"
#include "NetworkTransport.h"

using namespace anengine;

class NetworkService : public Service
{
    // Shared values
    pthread_t myNetworkThread;
    pthread_mutex_t myGameSateLock;
    pthread_cond_t myDoneCond;
    bool myNewGameState;
    bool myQuit;
    bool myDone;

    // Game-side values
    GameState myGameState;
    OutPin myGameStatePin;

    // Network-side values
    std::string myHost;
    std::string myPort;
    NetworkTransport myTransport;
    ProtocolHandler myProtocol;

    static void *sNetworkMain(void *arg);
    void *NetworkMain();

    bool DoneUpdate(Event &event, InPin pin);
    public:
    NetworkService(std::string host, std::string port)
        : myNewGameState(false), myQuit(false), myDone(false), myHost(host), 
        myPort(port), myProtocol(&myTransport)
    {
        myGameStatePin = RegisterOutPin(SkyportEventClass::GameState, "GameStates");

        RegisterInPin(SkyportEventClass::GameState, "Done", 
                static_cast<EventCallback>(&NetworkService::DoneUpdate));

        int r = pthread_mutex_init(&myGameSateLock, NULL);
        r |= pthread_cond_init(&myDoneCond, NULL);
        if(r != 0)
            throw Error(Error::InternalError, "Failed to create matrix");
    }

    virtual ~NetworkService() 
    {
        pthread_cond_destroy(&myDoneCond);
        pthread_mutex_destroy(&myGameSateLock); 
    }

    virtual void OnInitialize();
    virtual void OnUninitialize();
    virtual void OnUpdate(FrameTime time);
};

#endif
