#include <contest.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
    Session* session1 = new_Session("", 1);
    puts(Session_isValide(session1) ? "true" : "false");
    puts(Session_lastError(session1));
    delete_Session(session1);

    Session* session2 = new_Session("127.0.0.1", 1080);
    puts(Session_isValide(session2) ? "true" : "false");
    puts(Session_lastError(session2));
    delete_Session(session2);

    return EXIT_SUCCESS;
}

