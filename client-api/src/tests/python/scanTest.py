# -*- coding: utf-8 -*-

# IA qui ne donne aucuns ordres

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

    session.orderScan(0)
    session.orderScan(1)
    session.orderScan(2)
    session.orderScan(3)
    session.orderScan(4)
    session.sendOrders()

    print "Avancement:", infos.currentRoundId, "/", infos.totalRoundCount, "tours"
    print "Contrôle:", len(planets), "/", infos.planetCount, "planètes:", [i.planetId for i in planets]
    print "Ressources:", infos.resources
    print "Vaisseaux:", sum([i.shipCount for i in planets])
    print "SCANS RECU:", data.scanResults()
    print


def game():
    session = gameInit("observer")

    # pré-calculs

    print "Attente du début du jeu..."

    while True:
        status = session.waitRoundStarting()

        if status == ROUND_NETWORK_ERROR: gameError(session)
        elif status == ROUND_END_OF_GAME: break

        gameRound(session)

    print "End of the game"


game()


