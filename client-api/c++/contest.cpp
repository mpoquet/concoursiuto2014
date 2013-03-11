#include "contest.hpp"

#include <protocole.h>
#include <Network.hpp>

using namespace Core;
using namespace Network;
using namespace std;


struct SessionData
{
    TcpSocket socket;
    string lastError;
};


Session::Session() : _data(new SessionData())
{
    
}


Session::~Session()
{
    delete _data;
}


bool Session::connect(const string& host, int port)
{
    if(port < 0 || port >= 1<<16)
    {
        setError("Session::connect: invalid port (must be between 0 and 65535)");
        return false;
    }

    try
    {
        _data->socket.connect(IpEndPoint(IpAddress(host), port));
        return true;
    }
    catch(NetworkException& err)
    {
        setError(string("Session::connect: ") + err.what());
        return false;
    }
}


void Session::disconnect()
{
    _data->socket.finish();
}


int Session::login(const string& pseudo)
{
    try
    {
        sendMessage(Message(LOGIN_PLAYER, pseudo));

        char res = waitMessage(string(1, LOGIN_PLAYER_ACK)).second[0];

        switch(res)
        {
            case OK: return LOGIN_OK;
            case NO_MORE_ROOM: setError("Session::login: no more room"); return LOGIN_NO_MORE_ROOM;
            case INVALID_NICKNAME: setError("Session::login: invalid nickname"); return LOGIN_INVALID;
            case ALREADY_LOGGED: setError("Session::login: already logged"); return LOGIN_ALREADY_USED;
            default: setError("Session::login: unexpected network message body"); return LOGIN_NETWORK_ERROR;
        }
    }
    catch(runtime_error& err)
    {
        setError(string("Session::login: ") + err.what());
        return LOGIN_NETWORK_ERROR;
    }
}


bool Session::waitInit()
{
    try
    {
        waitMessage(string(1, INIT_PLAYER));
        return true;
    }
    catch(runtime_error& err)
    {
        setError(string("Session::waitInit: ") + err.what());
        return false;
    }
}


int Session::waitRoundStarting()
{
    try
    {
        Message msg = waitMessage(string(1, TURN_PLAYER) + string(1, END_OF_GAME));

        if(msg.first == TURN_PLAYER)
        {
            const char* messageBody = msg.second.c_str();
            
            // RECUPERATION DE DONNEES

            return ROUND_NORMAL;
        }

        _data->socket.finish();
        return ROUND_END_OF_GAME;
    }
    catch(runtime_error& err)
    {
        setError(string("Session::waitRoundBeginning: ") + err.what());
        return ROUND_NETWORK_ERROR;
    }
}


bool Session::isConnected()
{
    return !_data->socket.finished();
}


string Session::lastError()
{
    return _data->lastError.c_str();
}


void Session::sendMessage(const Message& message)
{
    _data->socket.send((message.first + message.second + MESSAGE_SEP).c_str(), message.second.size()+2);
}


pair<char, string> Session::waitMessage(string expectedType)
{
    size_t pos = expectedType.find(_data->socket.recvChar());

    if(pos == string::npos)
        throw runtime_error("unexpected network message type");

    ByteArray data;
    _data->socket.recv(data, string(1, MESSAGE_SEP));
    return make_pair(expectedType[pos], data.toString());
}


void Session::setError(const std::string& error)
{
    _data->lastError = error;
}


