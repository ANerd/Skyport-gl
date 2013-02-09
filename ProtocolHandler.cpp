
#include <json.h>
#include <sstream>
#include "ProtocolHandler.h"
#include "core/Error.h"
#include "core/Debug.h"

using namespace anengine;

struct JsonRoot
{
    json_object *Object;
    JsonRoot(json_object *object)
        : Object(object) { }
    ~JsonRoot()
    {
        if(Object != NULL)
            json_object_put(Object);
    }
};

VectorI2 ParseVector(std::string str)
{
    std::istringstream s(str);
    int j,k;
    char sep;
    s>>j>>sep>>k;
    if(sep != ',')
        throw Error(Error::InvalidValue, "Invalid vector separator");
    return VectorI2(j,k);
}

void ProtocolHandler::Initialize()
{
    if(myState != ProtocolState::Uninitialized)
        throw Error(Error::InvalidState, "Double initialize");
    myTransport->Send(Generate(MessageType::Connect));
    myState = ProtocolState::WaitingForHandshake;
}
void ProtocolHandler::Uninitialize()
{
    myTransport->Disconnect();
    myState = ProtocolState::Uninitialized;
}
//void ProtocolHandler::OnUpdate(FrameTime time)
//{
//}

bool ProtocolHandler::InternalMessage(MessageType type)
{
    return type != MessageType::GameState && type != MessageType::TurnDone;
}

bool ProtocolHandler::UpdateGamesate(GameState &gamestate)
{
    while(true)
    {
        std::string line = myTransport->Recv('\n');
        if(line.size() == 0)
            return false;
        MessageType t = Parse(line, gamestate);
        if(!InternalMessage(t))
        {
            return t == MessageType::GameState;
        }
    }
}

void ProtocolHandler::NotifyDone()
{
    myTransport->Send(Generate(MessageType::AnimationDone));
    myState = ProtocolState::WaitingForGamestate;
}

ProtocolHandler::MessageType ProtocolHandler::Parse(std::string str, GameState &state)
{
    MessageType type;
    json_object *root = json_tokener_parse(str.c_str());

    if(root == NULL)
        throw Error(Error::InvalidValue, "JSON said: "
                +std::string(json_tokener_error_desc(json_tokener_error())));

    JsonRoot r(root);

    if(!json_object_is_type(root, json_type_object))
        throw Error(Error::InvalidValue, "Did not get JSON-object");

    json_object * errorObject;
    if(json_object_object_get_ex(root, "error", &errorObject))
    {   
        std::string serverError;
        if(json_object_is_type(errorObject, json_type_string))
            serverError = json_object_get_string(errorObject);
        throw Error(Error::InvalidValue, "Server said: "+serverError);
    }

    json_object *messageObject;
    if(!json_object_object_get_ex(root, "message", &messageObject))
        throw Error(Error::InvalidValue, "No message field in message");

    std::string message = json_object_get_string(messageObject);
    Debug(std::string("Got message: ")+message);
    if(message == "connect")
    {
        if(myState != ProtocolState::WaitingForHandshake)
            throw Error(Error::InvalidState, "Got unexpected handshake");

        json_object *statusObject;
        if(!json_object_object_get_ex(root, "status", &statusObject))
            throw Error(Error::InvalidValue, "Got invalid handshake response");
        if(!json_object_get_boolean(statusObject))
            throw Error(Error::InvalidValue, "Handshake got status false");
        myState = ProtocolState::WaitingForGamestate;
        type = MessageType::Connect;
    }
    else if(message == "gamestate")
    {
        json_object *turnObject;
        if(!json_object_object_get_ex(root, "turn", &turnObject))
            throw Error(Error::InvalidValue, "Gamestate has no turn");
        int turn = json_object_get_int(turnObject);

        state.SetTurn(turn);

        json_object *playersObject;
        if(!json_object_object_get_ex(root, "players", &playersObject))
            throw Error(Error::InvalidValue, "Gamestate has no players");
        if(!json_object_is_type(playersObject, json_type_array))
            throw Error(Error::InvalidValue, "Players is not array");

        int playerCount = json_object_array_length(playersObject);
        if(playerCount == 0)
            Debug("Empty player list");
        for(int i = 0; i < playerCount; i++)
        {
            json_object * playerObject =
                json_object_array_get_idx(playersObject, i);

            json_object * nameObject;
            if(!json_object_object_get_ex(playerObject, "name", &nameObject))
                throw Error(Error::InvalidValue, "Player has no name");
            json_object * healthObject;
            if(!json_object_object_get_ex(playerObject, "health", &healthObject))
                throw Error(Error::InvalidValue, "Player has no health");
            json_object * scoreObject;
            if(!json_object_object_get_ex(playerObject, "score", &scoreObject))
                throw Error(Error::InvalidValue, "Player has no score");
            json_object * positionObject;
            if(!json_object_object_get_ex(playerObject, "position", &positionObject))
                throw Error(Error::InvalidValue, "Player has no position");
            state.SetPlayer(json_object_get_string(nameObject), 
                    json_object_get_int(healthObject), 
                    json_object_get_int(scoreObject), 
                    ParseVector(json_object_get_string(positionObject)));
            if(state.PlayerCount() == 0)
            {
                Debug("Added players but no players added");
            }
        }
        if(state.PlayerCount() == 0)
        {
            Debug("+New state has no players");
        }

        json_object *mapObject;
        if(!json_object_object_get_ex(root, "map", &mapObject))
            throw Error(Error::InvalidValue, "Gamestate has no map");

        json_object *jsizeObject;
        if(!json_object_object_get_ex(mapObject, "j-length", &jsizeObject))
            throw Error(Error::InvalidValue, "Map has no j-legnth");

        json_object *ksizeObject;
        if(!json_object_object_get_ex(mapObject, "k-length", &ksizeObject))
            throw Error(Error::InvalidValue, "Map has no k-legnth");

        json_object *mapDataObject;
        if(!json_object_object_get_ex(mapObject, "data", &mapDataObject))
            throw Error(Error::InvalidValue, "Map has no data");

        int jsize = json_object_get_int(jsizeObject);
        int ksize = json_object_get_int(ksizeObject);
        MapState newMap(VectorI2(jsize, ksize));
        for(int j = 0; j < jsize; j++)
        {
            json_object *jrow = json_object_array_get_idx(mapDataObject, j);
            for(int k = 0; k < ksize; k++)
            {
                json_object *elementObject = json_object_array_get_idx(jrow,k);
                newMap(j,k) = json_object_get_string(elementObject)[0];
            }
        }
        state.SetMap(newMap);

        myState = ProtocolState::InTurn;
        type = MessageType::GameState;
    }
    else if (message == "endturn")
    {
        myState = ProtocolState::WaitingForDone;
        type = MessageType::TurnDone;
    }
    else
        throw Error(Error::InvalidValue, "Unknown message: "+message);
    return type;
}

std::string ProtocolHandler::Generate(MessageType type)
{
    json_object *root = json_object_new_object();
    switch(type)
    {
        case MessageType::Connect:
            {
                json_object *connectMessage = json_object_new_string("connect");
                json_object_object_add(root, "message", connectMessage);
                json_object *revision = json_object_new_int(1);
                json_object_object_add(root, "revision", revision);
                json_object *name = json_object_new_string("skyport-gl");
                json_object_object_add(root, "name", name);
            }
            break;
        case MessageType::AnimationDone:
            {
                json_object *connectMessage = json_object_new_string("ready");
                json_object_object_add(root, "message", connectMessage);
            }
            break;
        default:
            throw Error(Error::InvalidValue, "Can't generate such message");
    }
    std::string message(json_object_to_json_string(root));
    json_object_put(root);
    return message+'\n';
}
