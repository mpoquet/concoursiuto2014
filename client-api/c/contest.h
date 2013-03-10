typedef struct Session Session;


// Alloue et initialise une session de jeu
Session* new_Session(const char* host, int port);

// Termine la session de jeu
void delete_Session(Session* session);

// Regarde si la session de jeu est valide
int Session_isValid(Session* session);

// Permet de connaître la dernière erreur qui est survenue
const char* Session_lastError(Session* session);

