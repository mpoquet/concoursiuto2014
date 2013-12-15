public class Test
{
    static
    {
        System.loadLibrary("contest");
    }


    public static void gameError(Session session)
    {
        System.err.println("ERROR: " + session.lastError());
        System.exit(-1);
    }


    public static Session gameInit(String login)
    {
        return gameInit(login, "127.0.0.1");
    }


    public static Session gameInit(String login, String ip)
    {
        return gameInit(login, ip, (short)4242);
    }


    public static Session gameInit(String login, String ip, short port)
    {
        System.out.println("Création de la sessions...");
        Session session = new Session();

        System.out.println("Connexion...");
        if(!session.connect(ip, port))
            gameError(session);

        System.out.println("Login...");
        if(session.login(login) != LoginResult.LOGIN_OK)
            gameError(session);

        System.out.println("Attente de l'initialisation jeu...");
        if(!session.waitInit())
            gameError(session);

        GameData data = session.gameData();
        GameInfos infos = data.globalInformations();
        System.out.print("Joueur ");
        System.out.print(infos.getPlayerId());
        System.out.print(" dans une partie de ");
        System.out.print(infos.getPlayerCount());
        System.out.println(" joueurs");

        return session;
    }


    public static void gameRound(Session session)
    {
        GameData data = session.gameData();
        GameInfos infos = data.globalInformations();
        PlanetList planets = data.planets();

        System.out.print("Avancement: ");
        System.out.print(infos.getCurrentRoundId());
        System.out.print(" / ");
        System.out.print(infos.getTotalRoundCount());
        System.out.println(" tours");
        System.out.print("Contrôle: ");
        System.out.print(planets.size());
        System.out.print(" / ");
        System.out.print(infos.getPlanetCount());
        System.out.print(" planètes: ");
        System.out.print("[");
        for(int i=0 ; i<planets.size() ; i++)
        {
            System.out.print(planets.get(i).getPlanetId());
            if(i < planets.size())
                System.out.print(", ");
        }
        System.out.println("]");
        System.out.print("Ressources: ");
        System.out.println(infos.getResources());
        int shipSum = 0;
        for(int i=0; i<planets.size(); i++)
            shipSum += planets.get(i).getShipCount();
        System.out.print("Vaisseaux: ");
        System.out.println(shipSum);
    }


    public static void main(String[] args)
    {
        Session session = gameInit("observer");

        // pré-calculs ici

        System.out.println("Attente du début du jeu...");

        while(true)
        {
            RoundState status = session.waitRoundStarting();

            if(status == RoundState.ROUND_NETWORK_ERROR)
                gameError(session);

            else if(status == RoundState.ROUND_END_OF_GAME)
                break;

            gameRound(session);
        }

        System.out.println("End of the game");
    }
}

