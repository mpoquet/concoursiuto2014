# -*- coding: utf-8 -*-

# Génère des vaisseaux sur ses planètes les plus faibles
# Attaque les planètes les plus proches

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

    planets = [i for i in range(0,infos.planetCount)]
    nearest = [(i+1)%len(planets) for i in xrange(len(planets))]

    distances = [[10000 for x in xrange(len(planets))] for x in xrange(len(planets))]

    for i in xrange(0, len(planets)):
        for j in xrange(0, len(planets)):
            if i < j:
                d = data.distance(i, j)
                distances[i][j] = d
                distances[j][i] = d

    for i in xrange(0, len(planets)):
        nearest[i] = [z for (y,z) in sorted([(distances[i][k], k) for k in xrange(0, len(planets))])]

    return session, nearest


def gameRound(session, nearest):
    data = session.gameData()
    infos = data.globalInformations()
    planets = data.planets()
    ennemies = set([i for i in xrange(0,infos.planetCount)]) - set([j.planetId for j in planets])

    print "Avancement:", infos.currentRoundId, "/", infos.totalRoundCount, "tours"
    print "Contrôle:", len(planets), "/", infos.planetCount, "planètes:", [i.planetId for i in planets]
    print "Ressources:", infos.resources
    print "Vaisseaux:", sum([i.shipCount for i in planets])
    print

    # Déplacements
    for p in planets:
        if p.shipCount > 2:
            for i in nearest[p.planetId]:
                if i in ennemies:
                    session.orderMove(p.planetId, i, p.shipCount/2)
                    break

    # Génération de vaisseaux
    l = sorted([(p.shipCount, p.planetId) for p in planets])
    for (c,i) in l:
        session.orderBuild(i, 100)

    session.sendOrders()

def game():
    session, nearest = gameInit('Struts')

    # pré-calculs

    print 'Attente du début du jeu...'

    while True:
        status = session.waitRoundStarting()

        if status == ROUND_NETWORK_ERROR: gameError(session)
        elif status == ROUND_END_OF_GAME: break

        gameRound(session, nearest)

    print 'End of the game'


game()


