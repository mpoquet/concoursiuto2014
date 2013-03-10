%module contest


%{
    #include "contest.h"
%}

typedef struct Session
{
    %extend
    {
        Session(const char* host, int port);
        ~Session();
        
        bool isValid();
        const char* lastError();
    }
} Session;

