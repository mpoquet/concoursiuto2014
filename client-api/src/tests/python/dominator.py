# -*- coding: utf-8 -*-

# IA qui vise une planète random ennemi qu'elle voit pour envoyer la moitié de ses vaisseaux dessus
# l'IA tente d'aider ces propres planètes lorsqu'elle sait que l'énemie va les attaquer

# Mettre plus de protection sur les bord
# Eviter de construire vers le centre
# Déplacer les vaisseau du centre plutot vers les bord avant, pour attaquer, évitant ainsi une détection

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

    if len(planets) > 0:
        darkPlanets = list(set(range(infos.planetCount))-set([p.planetId for p in planets]))

        print
        print "Avancement:", infos.currentRoundId, "/", infos.totalRoundCount, "tours"
        print "Contrôle:", len(planets), "/", infos.planetCount, "planètes:", [p.planetId for p in planets]
        print "Ressources:", infos.resources
        print "Vaisseaux:", sum([p.shipCount for p in planets])

        shipCanBuild = max(infos.resources/infos.shipCost, 0)

        # Calcul de l'écart entre l'état d'équilibre et la situation réel pour chaque planète

        planetsState = dict()
        for p in planets:
            base = p.shipCount
            bonus = sum([f.shipCount for f in friends if f.destinationPlanetId == p.planetId])
            malus = sum([e.shipCount for e in enemies if e.destinationPlanetId == p.planetId])
            planetsState[p.planetId] = base + bonus - malus

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
            for pId in safePlanets:
                p = [p2 for p2 in planets if p2.planetId==pId][0]
                if p.shipCount > 1:
                    bestPlanetToRush = min(darkPlanets, key=lambda dp: data.distance(p.planetId, dp))
                    print "Choix > Attaque de la planète numéro", bestPlanetToRush, "avec", p.shipCount/2, "vaisseaux"
                    session.orderMove(p.planetId, bestPlanetToRush, min(p.shipCount/2+1, p.shipCount))

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


