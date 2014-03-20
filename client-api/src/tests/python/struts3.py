# -*- coding: utf-8 -*-

# Génère des vaisseaux sur ses planètes les plus faibles
# Attaque les planètes les plus proches

from contest import *
from time import *
from sys import *
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
    print "Coût d'un vaisseau :", infos.shipCost

    planets = [i for i in range(0,infos.planetCount)]
    nearest = [(i+1)%len(planets) for i in xrange(len(planets))]

    distances = [[42 for x in xrange(len(planets))] for x in xrange(len(planets))]

    for i in xrange(0, len(planets)):
        for j in xrange(0, len(planets)):
            if i < j:
                d = data.distance(i, j)
                distances[i][j] = d
                distances[j][i] = d

    averageDistance = sum([sum(l) for l in distances]) / (len(distances)*len(distances))
    maxDistance = max([max(l) for l in distances])

    print 'Distance moyenne : ', averageDistance
    print 'Distance maximum : ', maxDistance

    for i in xrange(0, len(planets)):
        nearest[i] = [z for (y,z) in sorted([(distances[i][k], k) for k in xrange(0, len(planets))])]

    planetsToScan = list(planets)
    knownPlanets = dict()

    return session, nearest, distances, maxDistance, averageDistance, planetsToScan, knownPlanets


def gameRound(session, nearest, distances, maxDistance, averageDistance, planetsToScan, knownPlanets):
    data = session.gameData()
    infos = data.globalInformations()
    planets = data.planets()
    friends = data.fleets()
    enemies = data.enemies()
    scanResults = data.scanResults()
    reports = data.reports()
    resources = infos.resources

    startTime = time()

    planetMap = dict()
    for p in planets:
        planetMap[p.planetId] = p

    otherPlanets = set([i for i in xrange(0,infos.planetCount)]) - set([j.planetId for j in planets])

    print 'Tour', infos.currentRoundId, '/', infos.totalRoundCount
    print 'Contrôle', len(planets), '/', infos.planetCount, 'planètes :', [p.planetId for p in planets]
    print 'Ressources :', resources

    #######################
    # Calcul des énergies #
    #######################

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
    badPlanets = list()
    goodPlanets = list()
    for (e,pid) in sortedEnergy:
        if e < 0:
            badPlanets.append(pid)
        else:
            goodPlanets.append(pid)

    #####################
    # Gestion des scans #
    #####################
    sc = random.sample(planetsToScan, min(len(planetsToScan), infos.scanCountLimit))

    for s in sc:
        planetsToScan.remove(s)
        session.orderScan(s)

    for s in scanResults:
        alreadyWentOnPlanet = False
        if s in reports:
            alreadyWentOnPlanet = True
        knownPlanets[s.planetId] = (s.shipCount, s.playerId, infos.currentRoundId - 1, alreadyWentOnPlanet)

    ###################################
    # Lecture des rapports de combats #
    ###################################

    for r in reports:
        knownPlanets[r.planetId] = (r.remainingShip, r.winnerPlayerId, infos.currentRoundId - 1, True)

    ##########################
    # Création des vaisseaux #
    ##########################

    badBuild = []
    goodBuild = []

    # Création des vaisseaux sur les planètes en énergie négative...
    for pid in badPlanets:
        buildCount = planetMap[pid].shipBuildCountLimit
        while buildCount * infos.shipCost > resources:
            buildCount = buildCount - 1
        resources = resources - buildCount * infos.shipCost
        session.orderBuild(pid, buildCount)
        energy[pid] = energy[pid] + buildCount * maxDistance
        localEnergy[pid] = localEnergy[pid] + buildCount * maxDistance
        badBuild.append((pid,buildCount))

        if resources < infos.shipCost:
            break

    # Création des vaisseaux sur les planètes en énergie positive...
    maxBuildableShips = sum([planetMap[pid].shipBuildCountLimit for pid in goodPlanets])
    # Si on peut produire à fond sur toutes les planètes
    if resources > maxBuildableShips * infos.shipCost:
        for pid in goodPlanets:
            buildCount = planetMap[pid].shipBuildCountLimit
            resources = resources - buildCount * infos.shipCost
            session.orderBuild(pid, buildCount)
            energy[pid] = energy[pid] + buildCount * maxDistance
            localEnergy[pid] = localEnergy[pid] + buildCount * maxDistance
            goodBuild.append((pid, buildCount))
    # Sinon...
    else:
        # Essayons de trouver, parmi les planètes positives, celles les plus proches des ennemis...
        density = dict()
        for pid in goodPlanets:
            density[pid] = 0
            for nearPID in nearest[pid]:
                if distances[pid][nearPID] > averageDistance:
                    break
                if nearPID in otherPlanets:
                    density[pid] = density[pid] + distances[pid][nearPID]

        density = sorted([(density[pid], pid) for pid in density], reverse = True)

        print 'Density :', density

        shipToBuildPerPlanet = resources / infos.shipCost
        if shipToBuildPerPlanet < 1:
            shipToBuildPerPlanet = 1
        affordableShips = int(resources / infos.shipCost)
        builtShips = 0
        fappingPlanets = [] # Les planètes qui ne produisent pas au maximum
        
        # Essayons de construire un peu partout
        for (density, pid) in density:
            buildCount = min(planetMap[pid].shipBuildCountLimit, int(shipToBuildPerPlanet * random.gauss(1.0, 0.2)))
            while buildCount * infos.shipCost > resources:
                buildCount = buildCount - 1
            resources = resources - buildCount * infos.shipCost
            session.orderBuild(pid, buildCount)
            energy[pid] = energy[pid] + buildCount * maxDistance
            localEnergy[pid] = localEnergy[pid] + buildCount * maxDistance
            goodBuild.append((pid, buildCount))
            builtShips = builtShips + buildCount
            if buildCount < planetMap[pid].shipBuildCountLimit:
                fappingPlanets.append((pid, planetMap[pid].shipBuildCountLimit - buildCount))

        # S'il est possible de créer d'autres vaisseaux, les créer sur les planètes les plus denses
        for (pid, buildable) in fappingPlanets:
            if builtShips >= affordableShips:
                break
            buildCount = buildable
            while buildCount * infos.shipCost > resources:
                buildCount = buildCount - 1
            resources = resources - buildCount * infos.shipCost
            session.orderBuild(pid, buildCount)
            energy[pid] = energy[pid] + buildCount * maxDistance
            localEnergy[pid] = localEnergy[pid] + buildCount * maxDistance
            goodBuild.append((pid, buildCount))
            builtShips = builtShips + buildCount


    #############################
    # Déplacement des vaisseaux #
    #############################

    # Équilibre des énergies grâce à des déplacements entre planètes
    sortedEnergy = sorted([(energy[i], i) for i in energy])
    badPlanets = list()
    goodPlanets = list()
    for e in sortedEnergy:
        if e[0] < 0:
            badPlanets.append(e)
        else:
            goodPlanets.append(e[1])

    for (badEnergy, badPID) in badPlanets:
        for nearPID in nearest[badPID]:
            if energy[badPID] > 0:
                break
            if distances[badPID][nearPID] > averageDistance: # Avoid to help friends too far
                break
            if nearPID in goodPlanets and energy[nearPID] > 0 and localEnergy[nearPID] > 0:
                energyToLose = localEnergy[nearPID] / 2
                shipsToMove = int(energyToLose / maxDistance)

                if shipsToMove > planetMap[nearPID].shipCount - 1:
                    shipsToMove = planetMap[nearPID].shipCount - 1

                if shipsToMove > 0:
                    session.orderMove(nearPID, badPID, shipsToMove)
                    planetMap[nearPID].shipCount = planetMap[nearPID].shipCount - shipsToMove
                    energy[nearPID] = energy[nearPID] - shipsToMove * maxDistance
                    localEnergy[nearPID] = localEnergy[nearPID] - shipsToMove * maxDistance
                    energy[badPID] = energy[badPID] + shipsToMove * distances[badPID][nearPID]

    # Attaques sur les planètes vides trouvées et sur lesquelles nous n'avons pas exploré
        for pid in knownPlanets:
            (nbShip, playerID, roundInfo, alreadyWentOnIt) = knownPlanets[pid]
            if nbShip < 1 and not alreadyWentOnIt:
                for nearPID in nearest[pid]:
                    if nearPID in goodPlanets and planetMap[nearPID].shipCount > 1:
                        session.orderMove(nearPID, pid, 1)
                        planetMap[nearPID].shipCount = planetMap[nearPID].shipCount - 1
                        energy[nearPID] = energy[nearPID] - 1 * maxDistance
                        localEnergy[nearPID] = localEnergy[nearPID] - 1 * maxDistance
                        knownPlanets[pid] = (nbShip, playerID, roundInfo, True)
                        break;

    # Attaque à partir des planètes dont l'énergie locale est positive
    for goodPID in goodPlanets:
        if localEnergy[goodPID] > 0:
            energyToLose = localEnergy[goodPID]
            shipsToMove = int(energyToLose / maxDistance)

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

    #print 'Constructions sur planètes en danger : ', badBuild
    #print 'Constructions sur planètes positives : ', goodBuild
    print 'Temps de calcul : ', int(1000*(time() - startTime)), 'ms'

def game(ip, port):
    session, nearest, distances, maxDistance, averageDistance, planetsToScan, knownPlanets = gameInit('Struts3', ip, port)

    # pré-calculs

    print 'Attente du début du jeu...'

    while True:
        status = session.waitRoundStarting()

        if status == ROUND_NETWORK_ERROR: gameError(session)
        elif status == ROUND_END_OF_GAME: break

        gameRound(session, nearest, distances, maxDistance, averageDistance, planetsToScan, knownPlanets)

    print 'End of the game'


ip = '127.0.0.1'
port = 4242
if len(argv) >= 2:
    ip = argv[1]
if len(argv) >= 3:
    port = int(argv[2])
game(ip, port)


