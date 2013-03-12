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
    sta, stb = a.waitRoundStarting(), b.waitRoundStarting()

    if sta == ROUND_NORMAL and stb == ROUND_NORMAL:
        print "Tour n°"+str(i)+" lancé !"
    elif sta == ROUND_NETWORK_ERROR or stb == ROUND_NETWORK_ERROR:
        print "ERROR:", a.lastError() if a.lastError() != '' else b.lastError()
        exit()

    i += 1

print "End of the game"
#print "Waiting..."
#sleep(3)

print "Arret !"
exit()

