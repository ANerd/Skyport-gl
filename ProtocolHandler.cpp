
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

bool ProtocolHandler::InternalMessage(MessageType type)
{
    return type != MessageType::GameState && type != MessageType::ActionsDone;
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

Direction ParseDirection(std::string name)
{
    if(name == "up")
        return Direction::Up;
    if(name == "down")
        return Direction::Down;
    if(name == "right-up")
        return Direction::Right_Up;
    if(name == "left-up")
        return Direction::Left_Up;
    if(name == "right-down")
        return Direction::Right_Down;
    if(name == "left-down")
        return Direction::Left_Down;
    AbsBug("Unknown direction: "+name);
}

Weapon ParseWeapon(std::string name)
{
    if(name == "laser")
        return Weapon::Laser;
    if(name == "motar")
        return Weapon::Motar;
    if(name == "droid")
        return Weapon::Droid;
    AbsBug("Unknown weapon: " + name);
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
    else if(message == "title" || message == "subtitle")
    {
        std::string text;
        json_object *textObject;
        if(json_object_object_get_ex(root, "text", &textObject))
            text = json_object_get_string(textObject);
        if(message == "title")
            state.SetTitle(text);
        else
            state.SetSubtitle(text);
        type = MessageType::GameState;
    }
    else if(message == "gamestate")
    {
        if(myState != ProtocolState::WaitingForGamestate)
            throw Error(Error::InvalidState, "Got unexpected gamesate");

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
        for(int i = 0; i < playerCount; i++)
        {
            json_object * playerObject =
                json_object_array_get_idx(playersObject, i);

            json_object * nameObject;
            if(!json_object_object_get_ex(playerObject, "name", &nameObject))
                throw Error(Error::InvalidValue, "Player has no name");

            json_object * healthObject;
            int health = 100;
            if(json_object_object_get_ex(playerObject, "health", &healthObject))
                health = json_object_get_int(healthObject);

            json_object * scoreObject;
            int score = 0;
            if(json_object_object_get_ex(playerObject, "score", &scoreObject))
                score = json_object_get_int(scoreObject);

            json_object * positionObject;
            VectorI2 position;
            if(json_object_object_get_ex(playerObject, "position", &positionObject))
                position = ParseVector(json_object_get_string(positionObject));

            state.SetPlayer(json_object_get_string(nameObject), 
                    health, score, position);
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
    else if (message == "endactions")
    {
        if(myState != ProtocolState::InTurn)
            throw Error(Error::InvalidState, "Got unexpected endturn");
        myState = ProtocolState::WaitingForDone;
        type = MessageType::ActionsDone;
    }
    else if (message == "endturn")
    {
        type = MessageType::TurnDone;
    }
    else if(message == "action")
    {
        if(myState != ProtocolState::InTurn)
            throw Error(Error::InvalidState, "Got unexpected action");

        json_object *typeObject;
        if(!json_object_object_get_ex(root, "type", &typeObject))
            throw Error(Error::InvalidValue, "Action has no type");
        
        std::string atype(json_object_get_string(typeObject));
        if(atype == "move")
        {
            json_object *directionObject;
            if(!json_object_object_get_ex(root, "direction", &directionObject))
                throw Error(Error::InvalidValue, "Move has no direction");
            state.AddAction(ActionState::CreateMovement(ParseDirection(
                            json_object_get_string(directionObject))));
        }
        else if(atype == "pass")
        {
            state.AddAction(ActionState::CreatePass());
        }
        else if(atype == "upgrade")
        {
            json_object *weaponObject;
            if(!json_object_object_get_ex(root, "weapon", &weaponObject))
                throw Error(Error::InvalidValue, "Upgrade has no weapon");
            state.AddAction(ActionState::CreateUpgrade(ParseWeapon(
                            json_object_get_string(weaponObject))));
        }
        else if(atype == "mine")
        {
            state.AddAction(ActionState::CreateMine());
        }
        else if(atype == "laser")
        {
            json_object *directionObject;
            if(!json_object_object_get_ex(root, "direction", &directionObject))
                throw Error(Error::InvalidValue, "Laser has no direction");
            json_object *startObject;
            if(!json_object_object_get_ex(root, "start", &startObject))
                throw Error(Error::InvalidValue, "Laser has no start");
            json_object *stopObject;
            if(!json_object_object_get_ex(root, "stop", &stopObject))
                throw Error(Error::InvalidValue, "Laser has no stop");
            state.AddAction(ActionState::CreateLaser(ParseDirection(
                            json_object_get_string(directionObject)),
                            ParseVector(json_object_get_string(startObject)) - 
                                ParseVector(json_object_get_string(stopObject))));
        }
        else if(atype == "motar")
        {

            json_object *coordinateObject;
            if(!json_object_object_get_ex(root, "coordinates", &coordinateObject))
                throw Error(Error::InvalidValue, "Motar has no coordinate");
            state.AddAction(ActionState::CreateMotar(ParseVector(
                            json_object_get_string(coordinateObject))));
        }
        else if(atype == "droid")
        {
            Direction commands[ActionState::MaxDroidCommands];
            json_object *sequenceObject;
            if(!json_object_object_get_ex(root, "sequence", &sequenceObject))
                throw Error(Error::InvalidValue, "Droid has no sequence");

            int sequenceLength = json_object_array_length(sequenceObject);
            for(int i = 0; i < sequenceLength; i++)
            {
                json_object *commandObject = 
                    json_object_array_get_idx(sequenceObject, i);
                commands[i] = 
                    ParseDirection(json_object_get_string(commandObject));
            }
            state.AddAction(ActionState::CreateDroid(commands, sequenceLength));
        }
        else
            throw Error(Error::InvalidValue, "Unknown action: "+atype);
        type = MessageType::GameState;
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
                json_object *password = json_object_new_string("supersecretpassword");
                json_object_object_add(root, "password", password);
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
