/// \mainpage Documentation de la bibliotèque Contest
///
///     \section intro_sec Introduction
///
/// La bibliothèque Contest sert à développer simplement des clients pour le challenge de programmation.
/// Elle est utilisable dans plusieurs langages tel que C++, Python et Java. 
/// Son utilisation ne diffère que très peu en fonction du langage utilisé.
///
///     \section exemple_sec Exemple
///
/// Voici un exemple minimaliste d'utilisation en C++:
/// \code{.cpp}
///     #include <contest.hpp>
///
///     int main()
///     {
///         Session session;
///
///         session.connect("127.0.0.1", 4242);
///         session.login("DarkVador");
///
///         session.waitInit();
///         compute_preprocessing();
///
///         while(session.waitRoundStarting() == ROUND_NORMAL)
///             compute_one_round(session);
///         
///         return EXIT_SUCCESS;
///     }
/// \endcode
///
/// Voici le même exemple d'utilisation en Python:
/// \code{.python}
///     import contest
///
///     session = contest.Session()
///
///     session.connect('127.0.0.1', 4242)
///     session.login('DarkVador')
///
///     session.waitInit()
///     compute_preprocessing(session)
///
///     while session.waitRoundStarting() == ROUND_NORMAL:
///         compute_one_round(session)
/// \endcode
///
/// Voici encore le même exemple d'utilisation mais en Java:
/// \code{.java}
///     public class Test
///     {
///         static
///         {
///             System.loadLibrary("contest");
///         }
/// 
///         public static void main(String[] args)
///         {
///             Session session = new Session();
/// 
///             session.connect("127.0.0.1", (short)4242);
///             session.login("DarkVador");
/// 
///             session.waitInit();
///             compute_preprocessing();
/// 
///             while(session.waitRoundStarting() == RoundState.ROUND_NORMAL)
///                 compute_one_round(session);
///         }
///     }
/// \endcode
///
///     \section play_sec À vous de jouer !
///
///
///                                                      ____________
///                                       --)-----------|____________|
///                                                     ,'       ,'   
///                       -)------========            ,'  ____ ,'     
///                                `.    `.         ,'  ,'__ ,'       
///                                  `.    `.     ,'       ,'         
///                                    `.    `._,'_______,'__         
///                                      [._ _| ^--      || |         
///                              ____,...-----|__________ll_|\        
///             ,.,..-------"""""     "----'                 ||       
///         .-""  |=========================== ______________ |       
///          "-...l_______________________    |  |'      || |_]       
///                                       [`-.|__________ll_|         
///                                     ,'    ,' `.        `.         
///                                   ,'    ,'     `.    ____`.       
///                       -)---------========        `.  `.____`.     
///                                                    `.        `.   
///                                                      `.________`. 
///                                      --)-------------|___________|
///


#include <string>
#include <vector>


#define List std::vector
#define String std::string


struct SessionData;


/// État du bot après authentification auprès du serveur
enum LoginResult
{
    LOGIN_OK,
    LOGIN_NO_MORE_ROOM,
    LOGIN_INVALID,
    LOGIN_ALREADY_USED,
    LOGIN_NETWORK_ERROR
};


/// État d'un tour du jeux
enum RoundState
{
    ROUND_NORMAL,
    ROUND_END_OF_GAME,
    ROUND_NETWORK_ERROR
};


/// \brief Structure contenant toutes les données nécessaire à propos du message initial du serveur. 
///
/// Seul le champ currentRoundId sera mis à jour par l’API lors de chaque tours.
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


/// \brief Classe permettant d'obtenir de nombreuses informations sur le jeux.
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

    /// \brief Récupére les informations générales sur le jeu en cours.
    /// \return Informations générales sur le jeu en cours.
    GameInfos globalInformations();

    /// \brief Récupére une liste des planètes qui vous appartiennent.
    /// \return Liste des planètes qui vous appartiennent.
    PlanetList planets();

    /// \brief Récupére les résultats des scans du dernier tour.
    /// \return Résultats des scans du dernier tour.
    ScanResultList scanResults();

    /// \brief Récupére toutes les flottes qu’on a actuellement lancées.
    /// \return Liste de toutes les flottes qu’on a actuellement lancées.
    FleetList fleets();

    /// \brief Récupére toutes les flottes ennemies se dirigeant vers une de vos planète.
    /// \return Liste de toutes les flottes ennemies se dirigeant vers une de vos planète.
    EnnemyList enemies();

    /// \brief Récupére tous les rapports de combat envoyé au tour précédent.
    /// \return Liste de tous les rapports de combat envoyé au tour précédent.
    FightReportList reports();

    /// \brief Récupère la distance entre deux planètes.
    /// \param planetA Identifiant d'une planète
    /// \param planetB Identifiant d'une planète
    /// \return Distance qui sépare la première planète de la seconde (en nombre de tours).
    int distance(int planetA, int planetB);


    friend struct SessionData;
    friend class Session;
};


/// \brief Classe mère qui permet de créer un bot pour le jeux.
class Session
{
    private:

    SessionData* _data;


    public:

    /// \brief Constructeur de Session
    Session();

    /// \brief Destructeur de Session
    ~Session();

    /// \brief Connecte le client au serveur.
    /// \param host Adresse IP de l'hôte distant auquel le client doit se connecter.
    /// \param port Port de l'hôte distant auquel le client doit se connecter.
    /// \return vrai si la connexion a été établie, faux en cas d'erreur.
    bool connect(String host, int port);

    /// \brief Déconnecte le client du serveur.
    void disconnect();

    /// \brief Définie un login pour le client.
    /// \param pseudo Pseudo à définir.
    /// \return État du client après authentification du serveur.
    LoginResult login(String pseudo);

    /// \brief Attend l'initialisation du jeu.
    ///
    /// Les ordres peuvent seulement être données après l'appel de cette fonction.
    /// Avant l'appel, tous les ordres donnés ne sont pas pris en compte.
    /// \return Vrai si tout c'est bien passé, faux en cas d'erreur.
    bool waitInit();

    /// \brief Attend le début d'un tour.
    /// \return État du tour de jeux.
    RoundState waitRoundStarting();

    /// \brief Récupère une classe permettant d'obtenir de nombreuses informations sur le jeux.
    /// \return Structure informative.
    GameData gameData();

    /// \brief Ordonne un scan sur la planète ayant l'identifiant idPlanet.
    /// \param planetId Identifiant de planète a analyser.
    void orderScan(int planetId);

    /// \brief Ordonne la construction de nbVaisseaux vaisseaux sur la planète dont l'identifiant est idSource.
    /// \param planetSourceId Identifiant de la planète sur laquelle les vaisseaux doivent être construit.
    /// \param shipCount Nombre de vaisseaux à construire.
    void orderBuild(int planetSourceId, int shipCount);

    /// \brief Ordonne un mouvement de shipCount vaisseaux de planetSourceId à planetDestinationId.
    /// \param planetSourceId Identifiant de la planète depuis laquelle les vaisseaux doivent partir.
    /// \param planetDestinationId Identifiant de la planète sur laquelle les vaisseaux doivent arriver.
    /// \param shipCount Nombre de vaisseaux a déplacer.
    void orderMove(int planetSourceId, int planetDestinationId, int shipCount);

    /// \brief Envoie d’un coup au serveur tous les ordres précédemment émis.
    void sendOrders();

    /// \brief Efface tous les ordres donnés qui n'ont pas encore été envoyés.
    void clearOrders();

    /// \brief Permet de savoir si on est connecté au serveur.
    /// \return Vrai si le client est bien connecté au serveur, faux autrement.
    bool isConnected();

    /// \brief Récupère la dernière erreur survenue sous la forme d'une chaine lisible humainement.
    /// \return Chaine d'erreur.
    String lastError();


    private:

    typedef std::pair<char, std::string> Message;
    void sendMessage(const Message& message);
    Message waitMessage(std::string expectedType);
    void setError(const std::string& error);
    std::vector<int> toIntArray(const std::string& str, char delim);
    std::vector<std::string> split(const std::string& str, char delim);
    int toInt(const std::string& str);
};


