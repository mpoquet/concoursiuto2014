# -*- coding: utf-8 -*-

# IA qui vise les planètes les plus proches ennemi qu'elle voit pour envoyer la moitié de ses vaisseaux dessus
# l'IA tente d'aider ces propres planètes lorsqu'elle sait que l'énemie va les attaquer
# l'IA attaque partout au premier tour

from contest import *
from time import *
from random import *
from sys import *


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
    friends = data.fleets()
    enemies = data.enemies()

    darkPlanets = list(set(range(infos.planetCount))-set([p.planetId for p in planets]))

    if len(darkPlanets) > 0:
        darkPlanetsRandom = darkPlanets[:]
        shuffle(darkPlanetsRandom)
        for p in darkPlanetsRandom[:infos.scanCountLimit]:
            session.orderScan(p)

    if infos.currentRoundId == 1:
        if len(planets) > 0:
            shipCount = planets[0].shipCount

            for p in darkPlanets:
                if shipCount > 0:
                    session.orderMove(planets[0].planetId, p, 1)
                    shipCount -= 1

            session.sendOrders()

    else:
        if len(planets) > 0:
            print
            print "Avancement:", infos.currentRoundId, "/", infos.totalRoundCount, "tours"
            print "Contrôle:", len(planets), "/", infos.planetCount, "planètes:", [p.planetId for p in planets]
            print "Ressources:", infos.resources
            print "Vaisseaux:", sum([p.shipCount for p in planets])

            shipCanBuild = max(infos.resources/infos.shipCost, 0)

            # Calcul de l'écart entre l'état d'équilibre et la situation réel pour chaque planète

            bonus = [0] * infos.planetCount
            malus = [0] * infos.planetCount

            for f in friends:
                bonus[f.destinationPlanetId] += f.shipCount

            for e in enemies:
                malus[e.destinationPlanetId] += e.shipCount

            planetsState = {p.planetId: p.shipCount + bonus[p.planetId] - malus[p.planetId] for p in planets}

            # Equilibrage du système

            avg = sum([planetsState[key] for key in planetsState]) / len(planetsState)
            avg = max(avg, 0)
            safePlanets = sorted([key for key in planetsState if planetsState[key] >= avg], key=lambda k: planetsState[key])
            unsafePlanets = sorted([key for key in planetsState if planetsState[key] < avg], key=lambda k: planetsState[key])
            totalToBuild = sum([avg - planetsState[p] for p in unsafePlanets])

            if totalToBuild <= shipCanBuild:
                shipCanBuild -= totalToBuild
                for p in unsafePlanets:
                    session.orderBuild(p, avg - planetsState[p])
                    shipCanBuild -= avg - planetsState[p]
                for p in planets:
                    if shipCanBuild / len(planets) > 0:
                        session.orderBuild(p.planetId, shipCanBuild / len(planets))
            else:
                for p in unsafePlanets:
                    if shipCanBuild > 0:
                        session.orderBuild(p, avg - planetsState[p])
                        shipCanBuild -= avg - planetsState[p]

            if len(darkPlanets) > 0:
                planetsDico = {p.planetId:p for p in planets}
                for pId in safePlanets:
                    p = planetsDico[pId]
                    if p.shipCount > 1:
                        if randint(0, 2) == 0:
                            rushList = []
                            for scan in data.scanResults():
                                if scan.playerId <= 0:
                                    rushList.append(scan)
                            for scan in sorted(rushList, key=lambda x: data.distance(p.planetId, x.planetId)):
                                if scan.playerId == -1:
                                    session.orderMove(p.planetId, scan.planetId, 1)
                                elif scan.playerId == 0:
                                    if scan.shipCount+1 < p.shipCount:
                                        session.orderMove(p.planetId, scan.planetId, scan.shipCount+1)

                        bestPlanetToRush = min(darkPlanets, key=lambda dp: data.distance(p.planetId, dp))
                        print "Choix > Attaque de la planète numéro", bestPlanetToRush, "avec", p.shipCount/2, "vaisseaux"
                        session.orderMove(p.planetId, bestPlanetToRush, min(p.shipCount/2+1, p.shipCount))

            session.sendOrders()


def game(ip, port):
    session = gameInit("domirush", ip, port)

    # pré-calculs

    print "Attente du début du jeu..."

    while True:
        status = session.waitRoundStarting()

        if status == ROUND_NETWORK_ERROR: gameError(session)
        elif status == ROUND_END_OF_GAME: break

        gameRound(session)

    print "End of the game"


ip = '127.0.0.1'
port = 4242
if len(argv) >= 2:
    ip = argv[1]
if len(argv) >= 3:
    port = int(argv[2])
game(ip, port)


