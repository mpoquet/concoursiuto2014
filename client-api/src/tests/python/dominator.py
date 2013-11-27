# -*- coding: utf-8 -*-

# IA qui vise une planète random ennemi qu'elle voit pour envoyer la moitié de ses vaisseaux dessus
# l'IA tente d'aider ces propres planètes lorsqu'elle sait que l'énemie va les attaquer

from contest import *
from time import *
from random import *


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

    print
    print "Avancement:", infos.currentRoundId, "/", infos.totalRoundCount, "tours"
    print "Contrôle:", len(planets), "/", infos.planetCount, "planètes:", [p.planetId for p in planets]
    print "Ressources:", infos.resources
    print "Vaisseaux:", sum([p.shipCount for p in planets])

    for p in planets:
        if p.shipCount > 1:
            randomId = randint(0, len(darkPlanets)-1)
            print "Choix > Attaque de la planète numéro", darkPlanets[randomId], "avec", p.shipCount/2, "vaisseaux"
            session.orderMove(p.planetId, darkPlanets[randomId], p.shipCount/2)

    # Réserve 5 vaisseaux au moins (contre une attaque probable)
    shipToBuild = max(infos.resources/infos.shipCost - 10, 0)
    shipToBuild = int(shipToBuild/len(planets)) * len(planets)
    print "Choix > Contruction d'un total de", shipToBuild, "vaisseaux"

    for p in planets:
        session.orderBuild(p.planetId, shipToBuild/len(planets))

    # Calcul de l'écart entre l'état d'équilibre et la situation réel pour chaque planète
    planetsState = dict()
    for p in planets:
        base = p.shipCount
        bonus = sum([f.shipCount for f in friends if f.destinationPlanetId == p.planetId])
        malus = sum([e.shipCount for e in enemies if e.destinationPlanetId == p.planetId])
        planetsState[p.planetId] = base + bonus - malus

    # Tente de protéger rapidement la planète qui est la plus en danger en y construisant des vaisseaux
    # s'il reste de l'argent pour en créer
    if infos.resources - infos.shipCost*shipToBuild >= infos.shipCost:
        idPlanetInDanger = min(planetsState, key=lambda k: planetsState[k])
        session.orderBuild(idPlanetInDanger, (infos.resources - infos.shipCost*shipToBuild)/infos.shipCost)

    # Equilibrage du système

    avg = sum([planetsState[key] for key in planetsState]) / len(planetsState)
    safePlanets = [key for key in planetsState if planetsState[key] > avg]
    unsafePlanets = [key for key in planetsState if planetsState[key] < avg]

    while len(safePlanets) > 0 and len(unsafePlanets) > 0:
        shipNeeded = avg - planetsState[unsafePlanets[-1]]
        canGive = planetsState[safePlanets[-1]] - avg
        if canGive-shipNeeded > 0:
            session.orderMove(len(safePlanets)-1, len(unsafePlanets)-1, shipNeeded)
            planetsState[safePlanets[-1]] -= shipNeeded
            unsafePlanets[-1:] = []
        else:
            session.orderMove(len(safePlanets)-1, len(unsafePlanets)-1, canGive)
            if canGive-shipNeeded == 0:
                unsafePlanets[-1:] = []
            else:
                planetsState[unsafePlanets[-1]] += canGive
            safePlanets[-1:] = []

    session.sendOrders()


def game():
    session = gameInit("dominator")

    # pré-calculs

    print "Attente du début du jeu..."

    while True:
        status = session.waitRoundStarting()

        if status == ROUND_NETWORK_ERROR: gameError(session)
        elif status == ROUND_END_OF_GAME: break

        gameRound(session)

    print "End of the game"


game()


