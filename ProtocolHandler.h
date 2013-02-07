#ifndef PROTOCOLHANDLER_H_
#define PROTOCOLHANDLER_H_

#include "GameState.h"
#include "NetworkTransport.h"
#include "entity/Service.h"

class ProtocolHandler
{
    enum ProtocolState
    {
        Uninitialized,
        WaitingForHandshake,
        WaitingForGamestate,
        InFrame
    } myState;

    enum class MessageType
    {
        Connect,
        AnimationDone,
        GameState,
        FrameDone
    };
    bool InternalMessage(MessageType type);

    NetworkTransport *myTransport;
    public:
    ProtocolHandler(NetworkTransport *transport)
        : myState(ProtocolState::Uninitialized), myTransport(transport) { }

    void Initialize();
    void Uninitialize();

    /** Update gamesate.
     * \returns \c true if gamesate was updated. \c false if no more gamestate
     * is available and frame is ended.
     */
    bool UpdateGamesate(GameState &gamestate);
    /** Send notification that visualization of frame is done.
     */
    void NotifyDone();

    MessageType Parse(std::string str, GameState &state);
    std::string Generate(MessageType type);
};

#endif
