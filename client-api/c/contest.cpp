#include "contest.h"
#include <Network.hpp>

using namespace Network;
using namespace std;


typedef struct Session
{
    TcpSocket socket;
    string lastError;
} Session;


Session* new_Session(const char* host, int port)
{
    Session* session = new Session();

    try
    {
        if(port < 0 || port >= 1<<16)
            throw runtime_error("Bad port (must be between 0 and 65535)");

        session->socket.connect(IpEndPoint(IpAddress(host), port));
    }
    catch(runtime_error& err)
    {
        cerr << "ERROR : " << err.what() << endl;
        session->lastError = err.what();
    }
    
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


