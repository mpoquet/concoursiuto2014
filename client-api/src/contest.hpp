#include <string>
#include <vector>


#define List std::vector


struct SessionData;


enum LoginResult
{
    LOGIN_OK,
    LOGIN_NO_MORE_ROOM,
    LOGIN_INVALID,
    LOGIN_ALREADY_USED,
    LOGIN_NETWORK_ERROR
};

enum RoundState
{
    ROUND_NORMAL,
    ROUND_END_OF_GAME,
    ROUND_NETWORK_ERROR
};


/// \brief Structure contenant toutes les données nécessaire à propos du message initial du serveur. 
/// Seul le champ currentRoundId sera à mis à jour par l’API lors de chaque tours.
struct GameInfos
{
    /// Identifiant du joueur courant
    int playerId;

    /// Nombre de joueurs audébut de partie
    int playerCount;

    /// Nombre max de tours
    int totalRoundCount;

    /// Identifiant du tour actuel entre 1 et totalRoundCount (0 lors de l'initialisation)
    int currentRoundId;

    /// Nombre total de planètes dans la partie
    int planetCount;

    /// Ressources globales actuelles (0 lors de l'initialisation)
    int resources;


    /// Nombre maximal de scans autorisés par tour
    int scanCountLimit;

    /// Coût en ressources d’un vaisseaux
    int shipCost;
};


/// \brief Structure contenant les informations sur l’une de nos planètes.
struct Planet
{
    /// Identifiant de la planète
    int planetId;

    /// Ressources accumulées à chaque tours grâce à cette planète
    int resources;

    /// Nombre limite de vaisseaux pouvant être construit à chaque tours autours de la planète
    int shipBuildCountLimit;

    /// Nombre de vaisseaux actuellement présents autours de la planète
    int shipCount;
};


/// \brief Structure contenant le résultat d’un scan.
struct ScanResult
{
    /// Identifiant de la planète scannée
    int planetId;

    /// Identifiant du joueur scanné
    int playerId;

    /// Ressources accumulées par le joueur à chaque tours grâce à cette planète
    int resources;

    /// Nombre limite de vaisseaux pouvant être construit à chaque tours autours de la planète
    int shipBuildCountLimit;

    /// Nombre de vaisseaux actuellement présents autours de la planète
    int shipCount;
};


/// \brief Structure contenant les informations sur nos flottes en cours de déplacement 
/// que ce soit pour l'attaque comme pour le déplacement.
struct Fleet
{
    /// Identifiant de la planête vers laquelle les vaisseaux proviennent
    int sourcePlanetId;

    /// Identifiant de la planête vers laquelle les vaisseaux se dirigent
    int destinationPlanetId;

    /// Nombre de vaisseaux présents dans la flotte
    int shipCount;

    /// Nombre de tour restant avant l'arrivée des vaisseaux
    int remainingRound;
};


/// \brief Structure contenant les informations sur les flottes ennemies arrivant vers le joueur.
struct Ennemy
{
    /// Identifiant de la planête vers laquelle les vaisseaux proviennent
    int sourcePlanetId;

    /// Identifiant de la planête vers laquelle les vaisseaux se dirigent
    int destinationPlanetId;

    /// Nombre de vaisseaux présents dans la flotte (ce nombre est une estimation)
    int shipCount;

    /// Nombre de tour restant avant l'arrivée des vaisseaux
    int remainingRound;
};


/// \brief Structure contenant les informations sur les rapports de combat.
struct FightReport
{
    /// Identifiant de la planête sur laquelle les vaisseaux ont combattus
    int planetId;

    /// Identifiant du joueur qui a gagné la bataille
    int winnerPlayerId;

    /// Nombre de joueurs présents dans la bataille
    int playerCount;

    /// Nombre de vaisseaux restant du joueur survivant
    int remainingShip;
};


typedef List<Planet> PlanetList;
typedef List<ScanResult> ScanResultList;
typedef List<Fleet> FleetList;
typedef List<Ennemy> EnnemyList;
typedef List<FightReport> FightReportList;
typedef List<int> IntList;


/// Classe permettant d'obtenir de nombreuses informations sur le jeux.
class GameData
{
    private:

    GameInfos _gameInfos;
    PlanetList _planetList;
    ScanResultList _scanResultList;
    FleetList _fleetList;
    EnnemyList _ennemyList;
    FightReportList _fightReportList;
    IntList _distances;


    private:
    
    GameData();


    public:

    /// Récupére les informations générales sur le jeu en cours.
    GameInfos globalInformations();

    /// Récupére une listes de ses planètes.
    PlanetList planets();

    /// Récupére les résultats des scans du dernier tour.
    ScanResultList scanResults();

    /// Récupére toutes les flottes qu’on a actuellement lancées.
    FleetList fleets();

    /// Récupére toutes les flottes ennemies se dirigeant vers une de nos planète.
    EnnemyList enemies();

    /// Récupére tous les rapports de combat envoyé au tour précédent.
    FightReportList reports();

    /// Récupère la distance entre deux planètes.
    int distance(int planetA, int planetB);


    friend struct SessionData;
    friend class Session;
};


/// Classe mère qui permet de créer un bot pour le jeux.
class Session
{
    private:

    SessionData* _data;


    public:

    Session();
    ~Session();

    /// Connecte le client au serveur.
    bool connect(const std::string& host, int port);

    /// Déconnecte le client du serveur.
    void disconnect();

    /// Définie un login pour le client.
    LoginResult login(const std::string& pseudo);

    /// \brief Attend l'initialisation du jeu.
    ///
    /// Les ordres peuvent seulement être données après l'appel de cette fonction.
    /// Avant l'appel, tous les ordres donnés ne sont pas pris en compte.
    bool waitInit();

    /// Attend le début d'un tour
    RoundState waitRoundStarting();

    /// Récupère une classe permettant d'obtenir de nombreuses informations sur le jeux.
    GameData gameData();

    /// Ordonne un scan sur la planète ayant l'identifiant idPlanet.
    void orderScan(int planetId);

    /// Ordonne la construction de nbVaisseaux vaisseaux sur la planète dont l'identifiant est idSource.
    void orderBuild(int planetSourceId, int shipCount);

    /// Ordonne un mouvement de shipCount vaisseaux de planetSourceId à planetDestinationId.
    void orderMove(int planetSourceId, int planetDestinationId, int shipCount);

    /// Envoie d’un coup au serveur tous les ordres précédemment émis.
    void sendOrders();

    /// Efface tous les ordres donnés qui n'ont pas encore été envoyés.
    void clearOrders();

    /// Permet de savoir si on est connecté au serveur.
    bool isConnected();

    /// Récupère la dernière erreur survenue sous la forme d'une chaine lisible humainement.
    std::string lastError();


    private:

    typedef std::pair<char, std::string> Message;
    void sendMessage(const Message& message);
    Message waitMessage(std::string expectedType);
    void setError(const std::string& error);
    std::vector<int> toIntArray(const std::string& str, char delim);
    std::vector<std::string> split(const std::string& str, char delim);
    int toInt(const std::string& str);
};


