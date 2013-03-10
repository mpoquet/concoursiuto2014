#ifndef NETWORK_SOCKETSET_H
#define NETWORK_SOCKETSET_H


#ifndef _WIN32
	#include <linux/version.h>
	
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,44)
		#define EPOLL_ENABLED
		#include <sys/epoll.h>
	#endif
#endif

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>

#include "Errors.hpp"
#include "Socket.hpp"
#include "NetworkException.hpp"

#define EPOLL_MAX_EVENTS 1024



namespace Network
{
	//! @brief Classe fournissant un moyen de manipuler un ensemble de 
	//! sockets et permettant de déterminer l'état de plusieurs sockets.
	//!
	//! Notez que cette classe fourni un moyen portable d'attendre un 
	//! un ou plusieurs événement sur une ou plusieurs sockets mais 
	//! elle n'est pas faite pour écouter un grand nombre de sockets.
	//! Pour pouvoir écouter un grand nombre de socket, il est fortement
	//! conseillé d'utiliser la classe EpollSocketSet (disponnible 
	//! sur des systèmes relativements récent).
	//!
	//! Il n'est pas possible de manipuler un même SelectSocketSet dans 
	//! plusieurs threads différents en même temps. Cependant, il est 
	//! possible d'utiliser la classe dans plusieurs threads différents.
	//! L'écoute d'une même socket dans plusieurs threads différents
	//! donne un résultat imprévisible.
	class SelectSocketSet
	{
		fd_set _fdSet;
		
		#ifndef _WIN32
			SOCKET _maxiSock;
		#endif
		
		
		public:
		
		//! @brief Construit un ensemble natif de sockets vide.
		SelectSocketSet();
		
		//! @brief Construit un ensemble natif de sockets à partir 
		//! d'un vecteur de sockets.
		//! @param sockets Vecteur à copier dans l'ensemble
		SelectSocketSet(const std::vector<Socket*>& sockets);
		
		//! @brief Construit un ensemble natif de sockets à partir 
		//! d'une liste chainée de sockets.
		//! @param sockets Liste chainée à copier dans l'ensemble
		SelectSocketSet(const std::list<Socket*>& sockets);
		
		//! @brief Construit un ensemble natif de sockets à partir 
		//! d'un ensemble de sockets.
		//! @param sockets Ensemble à copier dans l'ensemble natif
		SelectSocketSet(const std::set<Socket*>& sockets);
		
		//! @brief Vide l'ensemble.
		void clear();
		
		//! @brief Ajoute une socket dans l'ensemble.
		//! @param sock Socket à ajouter dans l'ensemble
		void add(const Socket& sock);
		
		//! @brief Supprime une socket de l'ensemble.
		//! @param sock Socket à supprimer de l'ensemble
		void remove(const Socket& sock);
		
		//! @brief Test si une socket fait partie de l'ensemble.
		//! @param sock Socket recherchée dans l'ensemble
		//! @return True si la socket spécifiée est dans l'ensemble.
		//!			False dans le cas contraire.
		bool isset(const Socket& sock) const;
		
		//! @brief Attend des changements d'état sur plusieurs sockets. 
		//!
		//! Il y a trois ensembles indépendants de sockets qui sont 
		//! surveillés simultanément. Ces ensembles sont modifiés 
		//! pour indiquer les sockets qui ont changé de statut. 
		//! @param checkRead Ensemble de sockets surveillés pour 
		//! 					vérifier si des bytes deviennent 
		//!						disponibles en lecture sur une socket.
		//! @param checkWrite Ensemble de sockets surveillés pour 
		//! 					vérifier si une écriture ne bloquera 
		//!						pas sur une socket.
		//! @param checkError Ensemble de sockets surveillés pour 
		//! 					détecter l'occurence de conditions 
		//!						exceptionnelles (par exemple l'arrivée 
		//!						de données hors-bande)
		//! @param timeout Limite de temps d'exécution de la méthode, 
		//!					en microseconds. La valeur -1 signifie que 
		//!					la méthode n'a pas de limite de temps.
		//! @return True si au moins une socket à changée d'état dans 
		//!			un des trois ensemble de socket. False si aucun 
		//!			changement n'a été detecté durant le temps spécifié.
		//!
		//! Method thread-safe si elle est appelée avec des parammètres 
		//! différent entre les threads.
		static bool select(SelectSocketSet& checkRead, 
							SelectSocketSet& checkWrite, 
							SelectSocketSet& checkError, 
							int timeout = -1) throw(NetworkException);
	};
	
	#ifdef EPOLL_ENABLED
		//! @brief Classe fournissant un moyen de manipuler un ensemble de 
		//! sockets et de déterminer leur état plus rapidement qu'avec 
		//! la classe SelectSocketSet.
		//! 
		//! Cette classe permet un traitement avancé des évenements 
		//! survenant sur un ensemble de socket. Contrairement à la classe
		//! SelectSocketSet qui va manipuler un esemble de sockets qui 
		//! devra être recréer à chaque fois qu'on execute la méthode 
		//! statique select, EpollSocketSet va permettre de créer un 
		//! ensemble de sockets, puis de les suveiller selon 
		//! certains critères (attente en lecture, écriture, etc.) sans 
		//! avoir à récréer l'ensemble suveillé. De plus, cette méthode 
		//! permet de connaître directement les sockets qui ont changé 
		//! d'état. Alors que SelectSocketSet requiert un boucle qui 
		//! parcourt les sockets des ensembles surveillés pour savoir si
		//! elle ont changé d'état ou non.
		//! La précision de la méthode EpollSocketSet::wait est moins 
		//! grande que celle de SelectSocketSet::select mais la 
		//! complexité de EpollSocketSet::wait dépend du nombre de 
		//! descripteur actif (sortant) alors que celle de 
		//! SelectSocketSet::select dépend du nombre de sockets
		//! surveillée (entrante). EpollSocketSet::wait est donc de 
		//! complexité optimale.
		//! Attention : Cette fonctionnalité n'est présente que sous 
		//! Linux (version 2.6.8 ou plus). 
		//! Cette classe peut être utilisée si EPOLL_ENABLED est défini.
		//! Evenement les plus utilisées : 
		//! @li EPOLLIN : Evenement dégagé lorsque on est prêt 
		//! 	à recevoir des données.
		//! @li EPOLLOUT : Evenement dégagé lorsque on est prêt 
		//! 	à envoyer des données.
		//! @li EPOLLHUP : Evenement dégagé lorsque la socket 
		//! 	est fermée localement.
		//! @li EPOLLRDHUP : Evenement dégagé lorsque la socket 
		//! 	est fermée à distance.
		//! @li EPOLLERR : Evenement dégagé lorsqu'une erreur 
		//! 	survient sur la socket.
		//!
		//! Il n'est pas possible d'utiliser un même EpollSocketSet dans 
		//! plusieurs threads différents. Cependant, il est possible 
		//! d'utiliser la classe dans plusieurs threads différents.
		//! L'écoute d'une même socket dans plusieurs threads différents
		//! donne un résultat imprévisible.
		class EpollSocketSet
		{
			private:
			
			int epollFd;
			epoll_event* eventBuff;
			
			
			public:
			
			EpollSocketSet();
			~EpollSocketSet();
			
			//! @brief Ajoute une socket dans l'ensemble.
			//! @param sock Socket à ajouter dans l'ensemble
			//! @param events Flags d'evenements asociés à la socket
			void add(Socket* sock, int events) throw(NetworkException);
			
			//! @brief Modifie les évenements associés à une socket 
			//! contenue dans l'ensemble.
			//! @param sock Socket à supprimer de l'ensemble
			//! @param events Flags d'evenements asociés à la socket
			void modify(Socket* sock, int events) throw(NetworkException);
			
			//! @brief Supprime une socket de l'ensemble.
			//! @param sock Socket à supprimer de l'ensemble
			void remove(const Socket* sock) throw(NetworkException);
			
			//! ...
			//! @param timeout Limite de temps d'exécution de la méthode, 
			//!					en miliseconds. La valeur -1 signifie que 
			//!					la méthode n'a pas de limite de temps.
			std::vector< std::pair<Socket*, int> > wait(int timeout = -1) throw(NetworkException);
		};
	#endif
	
	#ifdef IO_COMPLETION_PORT_ENABLED
		class CompletionPortSocketSet
		{
			private:
			
			HANDLE completionPort;
			
			
			public:
			
			CompletionPortSocketSet() throw(NetworkException);
			~CompletionPortSocketSet();
			
			void add(Socket* sock, int events) throw(NetworkException);
		};
	#endif
}


#endif


