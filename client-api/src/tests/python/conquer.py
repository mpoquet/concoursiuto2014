# -*- coding: utf-8 -*-

# IA qui vise la prenière planète ennemi qu'elle voit pour envoyer la moitié de ses vaisseaux dessus

from contest import *
from time import *


def gameError(session):
    print "ERROR:", session.lastError()
    exit(-1)


def gameInit(login, ip='127.0.0.1', port=4242):
    print "Création de la sessions..."
    session = Session()

    print "Connexion..."
    if not session.connect(ip, port):
        gameError(session)

    print "Login..."
    if session.login(login) != LOGIN_OK:
        gameError(session)

    print "Attente de l'initialisation jeu..."
    if not session.waitInit():
        gameError(session)

    data = session.gameData()
    infos = data.globalInformations()
    print "Joueur", infos.playerId, "dans une partie de", infos.playerCount, "joueurs"

    return session


def gameRound(session):
    data = session.gameData()
    infos = data.globalInformations()
    planets = data.planets()

    darkPlanets = list(set(range(infos.planetCount))-set([p.planetId for p in planets]))

    print "Avancement:", infos.currentRoundId, "/", infos.totalRoundCount, "tours"
    print "Contrôle:", len(planets), "/", infos.planetCount, "planètes:", [p.planetId for p in planets]
    print "Ressources:", infos.resources
    print "Vaisseaux:", sum([p.shipCount for p in planets])

    for p in planets:
        if p.shipCount > 1:
            print "Choix > Attaque de la planète numéro", darkPlanets[0], "avec", p.shipCount/2, "vaisseaux"
            session.orderMove(p.planetId, darkPlanets[0], p.shipCount/2)

    shipToBuild = infos.resources/infos.shipCost
    print "Choix > Contruction d'un total de", shipToBuild, "vaisseaux"

    for p in planets:
        session.orderBuild(p.planetId, shipToBuild/len(planets))

    if shipToBuild%len(planets) > 0:
        session.orderBuild(planets[0].planetId, shipToBuild%len(planets))

    session.sendOrders()


def game():
    session = gameInit("conquer")

    # pré-calculs

    print "Attente du début du jeu..."

    while True:
        status = session.waitRoundStarting()

        if status == ROUND_NETWORK_ERROR: gameError(session)
        elif status == ROUND_END_OF_GAME: break

        gameRound(session)

    print "End of the game"


game()


