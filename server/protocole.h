#ifndef PROTOCOLE_H
#define PROTOCOLE_H

#define MESSAGE_SEP '\n'
#define SEP '#'
#define SSEP ' '
#define OK 'a'
#define NO_MORE_ROOM 'b'
#define INVALID_NICKNAME 'c'
#define ALREADY_LOGGED 'd'

// Client -> Server
#define LOGIN_PLAYER 'A'
#define LOGIN_DISPLAY 'B'
#define MOVE_PLAYER 'C'

// Server -> Client
#define LOGIN_PLAYER_ACK 'D'
#define LOGIN_DISPLAY_ACK 'E'
#define INIT_PLAYER 'F'
#define INIT_DISPLAY 'G'
#define TURN_PLAYER 'H'
#define TURN_DISPLAY 'I'
#define END_OF_GAME 'J'

#endif // PROTOCOLE_H
