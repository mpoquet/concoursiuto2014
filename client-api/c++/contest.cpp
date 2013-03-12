#include "contest.hpp"

#include <sstream>
#include <cstdio>
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

        const char res = waitMessage(string(1, LOGIN_PLAYER_ACK)).second[0];

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
        const Message msg = waitMessage(string(1, INIT_PLAYER));

        const vector<string> messageParts = split(msg.second, SEP);

        const int planetCount = toInt(messageParts[0]);
        const vector<int> planetDistances = toIntArray(messageParts[1], SSEP);
        const int roundCount = toInt(messageParts[2]);
        const int playerCount = toInt(messageParts[3]);
        const int playerId = toInt(messageParts[4]);

        // COUT VAISSEAU ???
        // TOURS ECOULE ???

        cout << "planetCount:" << planetCount << endl;
        cout << "size(planetDistances):" << planetDistances.size() << endl;
        cout << "roundCount:" << roundCount << endl;
        cout << "playerCount:" << playerCount << endl;
        cout << "playerId:" << playerId << endl;

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
        const Message msg = waitMessage(string(1, TURN_PLAYER) + string(1, END_OF_GAME));

        if(msg.first == TURN_PLAYER)
        {
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


Data Session::data()
{
    throw runtime_error("not implemented");
}


void Session::orderScan(int planetId)
{
    throw runtime_error("not implemented");
}


void Session::orderMove(int planetSourceId, int planetDestinationId, int shipCount)
{
    throw runtime_error("not implemented");
}


void Session::orderBuild(int planetSourceId, int shipCount)
{
    throw runtime_error("not implemented");
}


void Session::sendOrders()
{
    throw runtime_error("not implemented");
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
    const size_t pos = expectedType.find(_data->socket.recvChar());

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


vector<int> Session::toIntArray(const string& str, char delim)
{
    vector<int> elems;
    stringstream ss(str);
    string item;

    while(getline(ss, item, delim))
        elems.push_back(toInt(item));
    
    return elems;
}


vector<string> Session::split(const string& str, char delim)
{
    vector<string> elems;
    stringstream ss(str);
    string item;

    while(getline(ss, item, delim))
        elems.push_back(item);
    
    return elems;
}


int Session::toInt(const string& str)
{
    return atoi(str.c_str());
}


Data::Data()
{
    
}


GameInfos Data::globalInformations()
{
    throw runtime_error("not implemented");
}


PlanetList Data::planets()
{
    throw runtime_error("not implemented");
}


ScanResultList Data::scanResults()
{
    throw runtime_error("not implemented");
}


FleetList Data::fleets()
{
    throw runtime_error("not implemented");
}


EnnemyList Data::enemies()
{
    throw runtime_error("not implemented");
}


FightReportList Data::reports()
{
    throw runtime_error("not implemented");
}


int Data::distance(int planetA, int planetB)
{
    throw runtime_error("not implemented");
}


