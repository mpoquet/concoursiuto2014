# -*- coding: utf-8 -*-

from contest import *
from time import *

print 'Création des sessions...'
a, b = Session(), Session()

print "Connexion..."
if not a.connect('127.0.0.1', 4242) or not b.connect('127.0.0.1', 4242):
    print "ERROR:", a.lastError() if a.lastError() != '' else b.lastError()
    exit()

print "Login..."
if a.login("bot_a") != LOGIN_OK or b.login("bot_b") != LOGIN_OK:
    print "ERROR:", a.lastError() if a.lastError() != '' else b.lastError()
    exit()

print "Attente du début de l'initialisation jeu..."
if not a.waitInit() or not b.waitInit():
    print "ERROR:", a.lastError() if a.lastError() != '' else b.lastError()
    exit()

# pré-calculs

print "Attente du début du jeu..."
i = 1
while True:
    if a != None:
        sta = a.waitRoundStarting()

    if b != None:
        stb = b.waitRoundStarting()

    if sta == ROUND_NORMAL and stb == ROUND_NORMAL:
        print "Tour n°"+str(i)+" lancé !"
    elif sta == ROUND_NETWORK_ERROR or stb == ROUND_NETWORK_ERROR:
        print "ERROR:", a.lastError() if a.lastError() != '' else b.lastError()
        exit()
    elif stb == sta:
        print "End of the game"
        break
    else:
        a = None if sta == ROUND_END_OF_GAME else a
        b = None if stb == ROUND_END_OF_GAME else b

    i += 1

print "Arret !"
exit()

