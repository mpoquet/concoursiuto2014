%module contest


%{
    #include "contest.h"
%}


typedef struct Session
{
    %extend
    {
        Session(const char* host, int port, const char* pseudo);
        ~Session();
        
        bool isValid();
        const char* lastError();
    }
} Session;

