#include "contest.h"

#include <protocole.h>
#include <Network.hpp>

using namespace Network;
using namespace std;


typedef struct Session
{
    TcpSocket socket;
    string lastError;
} Session;


Session* new_Session(const char* host, int port, const char* pseudo)
{
    Session* session = new Session();

    try
    {
        if(port < 0 || port >= 1<<16)
            throw runtime_error("Bad port (must be between 0 and 65535)");

        session->socket.connect(IpEndPoint(IpAddress(host), port));
        
        session->socket.sendChar(LOGIN_PLAYER);
        session->socket.send(pseudo, strlen(pseudo));
        session->socket.sendChar(MESSAGE_SEP);

        if(session->socket.recvChar() != LOGIN_PLAYER_ACK)
            throw runtime_error("unexpected message");

        char res = session->socket.recvChar();

        switch(res)
        {
            case OK:
                break;

            case NO_MORE_ROOM:
            case INVALID_NICKNAME:
            case ALREADY_LOGGED:
                throw runtime_error("bad login");

            default:
                throw runtime_error("unexpected message");
        }

        if(session->socket.recvChar() != MESSAGE_SEP)
            throw runtime_error("unexpected message");
    }
    catch(runtime_error& err)
    {
        cerr << "ERROR : " << err.what() << endl;
        session->lastError = err.what();
        session->socket.finish();
    }

    cout << "[DEBUG::SESSION=VALID]" << endl;
    return session;
}


void delete_Session(Session* session)
{
    delete session;
}


int Session_isValid(Session* session)
{
    if(session && !session->socket.finished())
        return 1;

    return 0;
}


const char* Session_lastError(Session* session)
{
    return session->lastError.c_str();
}


