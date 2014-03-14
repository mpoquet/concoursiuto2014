#include "contest.hpp"

#include <sstream>
#include <cstdio>
#include <protocole.h>
#include <spnl/Network.hpp>

using namespace Core;
using namespace Network;
using namespace System;
using namespace std;


struct SessionData
{
    TcpSocket socket;
    string lastError;
    GameData gameData;
    int scansOrdersCount;
    int moveOrdersCount;
    int buildOrdersCount;
    string scansOrders;
    string buildOrders;
    string moveOrders;
    bool gameStarted;
};


Mutex libLoadingMutex;


Session::Session() : _data(new SessionData())
{
    // Uniquement utile pour Windows en pratique
    libLoadingMutex.lock();
    load();
    libLoadingMutex.unlock();

    _data->gameStarted = false;
}


Session::~Session()
{
    delete _data;
}


bool Session::connect(String host, int port)
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
    catch(runtime_error& err)
    {
        setError(string("Session::connect: ") + err.what());
        return false;
    }
}


void Session::disconnect()
{
    try
    {
        _data->socket.finish();
    }
    catch(runtime_error& err)
    {

    }
}


LoginResult Session::login(String pseudo)
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

        const GameInfos infos = {
                                    toInt(messageParts[6]),
                                    toInt(messageParts[5]),
                                    toInt(messageParts[2]),
                                    0,
                                    toInt(messageParts[0]),
                                    0,
                                    toInt(messageParts[3]),
                                    toInt(messageParts[4])
                                };

        _data->gameData._gameInfos = infos;
        _data->gameData._distances = toIntArray(messageParts[1], SSEP);

        _data->gameStarted = true;
        clearOrders();
        return true;
    }
    catch(runtime_error& err)
    {
        setError(string("Session::waitInit: ") + err.what());
        return false;
    }
}


RoundState Session::waitRoundStarting()
{
    try
    {
        const Message msg = waitMessage(string(1, TURN_PLAYER) + string(1, END_OF_GAME));

        if(msg.first == TURN_PLAYER)
        {
            // RECUPERATION DE DONNEES
            GameData& gameData = _data->gameData;
            
            const vector<string> msgBody = split(msg.second, SEP);
            gameData._gameInfos.currentRoundId = toInt(msgBody[0]);
            gameData._gameInfos.resources = toInt(msgBody[1]);

            const vector<string> planetList = split(msgBody[2], SSEP);
            const int planetListCount = toInt(planetList[0]);
            gameData._planetList.resize(planetListCount);

            for(int i=0 ; i<planetListCount ; ++i)
            {
                const Planet planet =   {
                                            toInt(planetList[i*5+1]),
                                            toInt(planetList[i*5+2]),
                                            toInt(planetList[i*5+3]),
                                            toInt(planetList[i*5+4]),
											toInt(planetList[i*5+5])
                                        };
                gameData._planetList[i] = planet;
            }

            const vector<string> scanResultList = split(msgBody[3], SSEP);
            const int scanResultListCount = toInt(scanResultList[0]);
            gameData._scanResultList.resize(scanResultListCount);

            for(int i=0 ; i<scanResultListCount ; ++i)
            {
                const ScanResult scanResult =   {
                                                    toInt(scanResultList[i*5+1]),
                                                    toInt(scanResultList[i*5+2]),
                                                    toInt(scanResultList[i*5+3]),
                                                    toInt(scanResultList[i*5+4]),
                                                    toInt(scanResultList[i*5+5])
                                                };
                gameData._scanResultList[i] = scanResult;
            }
            
            const vector<string> fleetList = split(msgBody[4], SSEP);
            const int fleetListCount = toInt(fleetList[0]);
            gameData._fleetList.resize(fleetListCount);

            for(int i=0 ; i<fleetListCount ; ++i)
            {
                const Fleet fleet = {
                                        toInt(fleetList[i*4+1]),
                                        toInt(fleetList[i*4+2]),
                                        toInt(fleetList[i*4+3]),
                                        toInt(fleetList[i*4+4])
                                    };
                gameData._fleetList[i] = fleet;
            }
            
            const vector<string> ennemyList = split(msgBody[5], SSEP);
            const int ennemyListCount = toInt(ennemyList[0]);
            gameData._ennemyList.resize(ennemyListCount);

            for(int i=0 ; i<ennemyListCount ; ++i)
            {
                const Ennemy ennemy = {
                                        toInt(ennemyList[i*4+1]),
                                        toInt(ennemyList[i*4+2]),
                                        toInt(ennemyList[i*4+3]),
                                        toInt(ennemyList[i*4+4])
                                    };
                gameData._ennemyList[i] = ennemy;
            }
            
            const vector<string> fightReportList = split(msgBody[6], SSEP);
            const int fightReportListCount = toInt(fightReportList[0]);
            gameData._fightReportList.resize(fightReportListCount);

            for(int i=0 ; i<fightReportListCount ; ++i)
            {
                const FightReport fightReport = {
                                        toInt(fightReportList[i*4+1]),
                                        toInt(fightReportList[i*4+2]),
                                        toInt(fightReportList[i*4+3]),
                                        toInt(fightReportList[i*4+4])
                                    };
                gameData._fightReportList[i] = fightReport;
            }

            return ROUND_NORMAL;
        }

        _data->gameStarted = false;
        _data->socket.finish();
        return ROUND_END_OF_GAME;
    }
    catch(runtime_error& err)
    {
        setError(string("Session::waitRoundBeginning: ") + err.what());
        return ROUND_NETWORK_ERROR;
    }
}


GameData Session::gameData()
{
    return _data->gameData;
}


void Session::orderScan(int planetId)
{
    char buff[128];

    if(!_data->gameStarted)
        return;

    sprintf(buff, "%c%d", SSEP, planetId);
    _data->scansOrders += buff;
    _data->scansOrdersCount++;
}


void Session::orderBuild(int planetSourceId, int shipCount)
{
    char buff[128];

    if(!_data->gameStarted)
        return;

    sprintf(buff, "%c%d%c%d", SSEP, planetSourceId, SSEP, shipCount);
    _data->buildOrders += buff;
    _data->buildOrdersCount++;
}


void Session::orderMove(int planetSourceId, int planetDestinationId, int shipCount)
{
    char buff[128];

    if(!_data->gameStarted)
        return;

    sprintf(buff, "%c%d%c%d%c%d", SSEP, planetSourceId, SSEP, planetDestinationId, SSEP, shipCount);
    _data->moveOrders += buff;
    _data->moveOrdersCount++;
}


void Session::sendOrders()
{
    if(!_data->gameStarted)
        return;
    
    ostringstream os;
    os << _data->scansOrdersCount << _data->scansOrders << SEP;
    os << _data->buildOrdersCount << _data->buildOrders << SEP;
    os << _data->moveOrdersCount << _data->moveOrders;
    sendMessage(Message(MOVE_PLAYER, os.str()));
    clearOrders();
}


void Session::clearOrders()
{
    _data->scansOrdersCount = 0;
    _data->buildOrdersCount = 0;
    _data->moveOrdersCount = 0;
    _data->scansOrders = "";
    _data->buildOrders = "";
    _data->moveOrders = "";
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
    if(!isConnected())
        throw runtime_error("session closed");

    const size_t pos = expectedType.find(_data->socket.recvChar());

    if(pos == string::npos)
        throw runtime_error("unexpected network message type");

    ByteArray data;
    _data->socket.recvSep(data, MESSAGE_SEP);
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


GameData::GameData()
{
    
}


GameInfos GameData::globalInformations()
{
    return _gameInfos;
}


PlanetList GameData::planets()
{
    return _planetList;
}


ScanResultList GameData::scanResults()
{
    return _scanResultList;
}


FleetList GameData::fleets()
{
    return _fleetList;
}


EnnemyList GameData::enemies()
{
    return _ennemyList;
}


FightReportList GameData::reports()
{
    return _fightReportList;
}


int GameData::distance(int planetA, int planetB)
{
    if(planetA >= _gameInfos.planetCount || planetA < 0)
        return -1;

    if(planetB >= _gameInfos.planetCount || planetB < 0)
        return -1;

    return _distances[planetA*_gameInfos.planetCount + planetB];
}


