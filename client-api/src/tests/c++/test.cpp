#include <contest.hpp>
#include <iostream>
#include <cstdlib>


using namespace std;


// IA qui ne donne aucuns ordres


void gameError(Session& session)
{
    cerr << "ERROR: " << session.lastError() << endl;
    exit(-1);
}


Session gameInit(const string& login, const string& ip = "127.0.0.1", short port = 4242)
{
    cout << "Création de la sessions..." << endl;
    Session session;

    cout << "Connexion..." << endl;
    if(!session.connect(ip, port))
        gameError(session);

    cout << "Login..." << endl;
    if(session.login(login) != LOGIN_OK)
        gameError(session);

    cout << "Attente de l'initialisation jeu..." << endl;
    if(!session.waitInit())
        gameError(session);

    GameData data = session.gameData();
    GameInfos infos = data.globalInformations();
    cout << "Joueur " << infos.playerId << " dans une partie de " << infos.playerCount << " joueurs" << endl;

    return session;
}


void gameRound(Session& session)
{
    GameData data = session.gameData();
    GameInfos infos = data.globalInformations();
    PlanetList planets = data.planets();

    cout << "Avancement: " << infos.currentRoundId << " / " << infos.totalRoundCount << " tours" << endl;
    cout << "Contrôle: " << planets.size() << " / " << infos.planetCount << " planètes: ";
    cout << "[";
    for(int i=0 ; i<planets.size() ; i++)
        cout << planets[i].planetId << (i < planets.size()) ? ", " : "";
    cout << "]" << endl;
    cout << "Ressources: " << infos.resources << endl;
    int shipSum = 0;
    for(int i=0; i<planets.size(); i++)
        shipSum += planets[i].shipCount;
    cout << "Vaisseaux: " << shipSum << endl;
    cout << endl;
}


int main(int argc, char* argv[])
{
    Session session = gameInit("observer");

    // pré-calculs ici

    cout << "Attente du début du jeu..." << endl;

    while(true)
    {
        RoundState status = session.waitRoundStarting();

        if(status == ROUND_NETWORK_ERROR)
            gameError(session);

        else if(status == ROUND_END_OF_GAME)
            break;

        gameRound(session);
    }

    cout << "End of the game" << endl;

    return EXIT_SUCCESS;
}


