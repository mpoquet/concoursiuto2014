# -*- coding: utf-8 -*-

# Génère des vaisseaux sur ses planètes les plus faibles
# Attaque les planètes les plus proches

from contest import *
from time import *
import random

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

    averageDistance = sum([sum(l) for l in distances]) / len(distances)
    maxDistance = max([max(l) for l in distances])

    for i in xrange(0, len(planets)):
        nearest[i] = [z for (y,z) in sorted([(distances[i][k], k) for k in xrange(0, len(planets))])]

    return session, nearest, distances, maxDistance, averageDistance


def gameRound(session, nearest, distances, maxDistance, averageDistance):
    data = session.gameData()
    infos = data.globalInformations()
    planets = data.planets()
    friends = data.fleets()
    enemies = data.enemies()
    resources = infos.resources

    startTime = time()

    planetMap = dict()
    for p in planets:
        planetMap[p.planetId] = p

    otherPlanets = set([i for i in xrange(0,infos.planetCount)]) - set([j.planetId for j in planets])

    print "Tour", infos.currentRoundId, "/", infos.totalRoundCount
    print "Contrôle:", len(planets), "/", infos.planetCount, "planètes:"
    print

    # Calcul des énergies
    energy = dict()
    localEnergy = dict()
    for p in planets:
        energy[p.planetId] = p.shipCount * maxDistance
        localEnergy[p.planetId] = p.shipCount * maxDistance

    for f in friends:
        if f.destinationPlanetId in planetMap:
            energy[f.destinationPlanetId] = energy[f.destinationPlanetId] + f.shipCount * (maxDistance - f.remainingRound)
            if f.remainingRound <= averageDistance:
                localEnergy[f.destinationPlanetId] = localEnergy[f.destinationPlanetId] + f.shipCount * (maxDistance - f.remainingRound)

    for e in enemies:
        if e.destinationPlanetId in planetMap:
            energy[e.destinationPlanetId] = energy[e.destinationPlanetId] - e.shipCount * (maxDistance - e.remainingRound)
            if e.remainingRound <= averageDistance:
                localEnergy[e.destinationPlanetId] = localEnergy[e.destinationPlanetId] - e.shipCount * (maxDistance - e.remainingRound)

    sortedEnergy = sorted([(energy[i], i) for i in energy])
    badPlanets = [e[1] for e in sortedEnergy if e[0] < 0]

    distribBuild = [1 - i*i*0.02 for i in range(-4, 5)]
    distribAttack = [1 - i*i*0.02 for i in range(-4, 5)]

    # Création des vaisseaux
    for pid in badPlanets:
        buildCount = int(planetMap[pid].shipBuildCountLimit * random.sample(distribBuild, 1)[0])

        while buildCount * infos.shipCost > resources:
            buildCount = buildCount - 1

        resources = resources - buildCount * infos.shipCost
        session.orderBuild(pid, buildCount)
        energy[pid] = energy[pid] + buildCount * maxDistance
        localEnergy[pid] = localEnergy[pid] + buildCount * maxDistance
        if resources < infos.shipCost:
            break

    maxShipBuildPerPlanet = resources / ((len(planets) - len(badPlanets) * 1.0))

    if resources > infos.shipCost:
        goodPlanets = [e[1] for e in sortedEnergy if e[0] >= 0]
        for pid in goodPlanets:
            buildCount = maxShipBuildPerPlanet
            if buildCount > planetMap[pid].shipBuildCountLimit:
                buildCount = planetMap[pid].shipBuildCountLimit
            else:
                buildCount = int(buildCount)

            while buildCount * infos.shipCost > resources:
                buildCount = buildCount - 1

            resources = resources - buildCount * infos.shipCost
            session.orderBuild(pid, buildCount)
            energy[pid] = energy[pid] + buildCount * maxDistance
            localEnergy[pid] = localEnergy[pid] + buildCount * maxDistance

            if resources < infos.shipCost:
                break

    # Équilibre des énergies grâce à des déplacements entre planètes proches
    sortedEnergy = sorted([(energy[i], i) for i in energy])
    badPlanets = list()
    goodPlanetsID = list()
    for e in sortedEnergy:
        if e[0] < 0:
            badPlanets.append(e)
        else:
            goodPlanetsID.append(e[1])

    for (badEnergy, badPID) in badPlanets:
        for nearPID in nearest[badPID]:
            if energy[badPID] > 0:
                break
            if nearPID in goodPlanetsID and energy[nearPID] > 0 and localEnergy[nearPID] > 0:
                energyToLose = localEnergy[nearPID] / 2
                shipsToMove = energyToLose / maxDistance

                if shipsToMove > planetMap[nearPID].shipCount - 1:
                    shipsToMove = planetMap[nearPID].shipCount - 1

                if shipsToMove > 0:
                    session.orderMove(nearPID, badPID, shipsToMove)
                    energy[nearPID] = energy[nearPID] - shipsToMove * maxDistance
                    localEnergy[nearPID] = localEnergy[nearPID] - shipsToMove * maxDistance
                    energy[badPID] = energy[badPID] + shipsToMove * distances[badPID][nearPID]

    # Attaque à partir des planètes dont l'énergie locale est positive
    for goodPID in goodPlanetsID:
        if localEnergy[goodPID] > 0:
            energyToLose = int(localEnergy[goodPID] * random.sample(distribAttack, 1)[0])
            shipsToMove = energyToLose / maxDistance

            if shipsToMove > planetMap[goodPID].shipCount - 1:
                shipsToMove = planetMap[goodPID].shipCount - 1

            if shipsToMove > 0:
                r = random.sample([1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4], 1)[0]
                k = 0
                sent = False
                for nearPID in nearest[goodPID]:
                    if nearPID in otherPlanets:
                        k = k + 1
                        if k == r:
                            session.orderMove(goodPID, nearPID, shipsToMove)
                            sent = True
                            break;

                if not sent and k > 0:
                    for nearPID in nearest[goodPID]:
                        if nearPID in otherPlanets:
                            session.orderMove(goodPID, nearPID, shipsToMove)
                            break;



    session.sendOrders()
    print 'Temps de calcul : ', int(1000*(time() - startTime)), 'ms'

def game():
    session, nearest, distances, maxDistance, averageDistance = gameInit('Struts2')

    # pré-calculs

    print 'Attente du début du jeu...'

    while True:
        status = session.waitRoundStarting()

        if status == ROUND_NETWORK_ERROR: gameError(session)
        elif status == ROUND_END_OF_GAME: break

        gameRound(session, nearest, distances, maxDistance, averageDistance)

    print 'End of the game'


game()


