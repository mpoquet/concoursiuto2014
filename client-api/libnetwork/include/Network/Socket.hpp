#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H


#include <string>

extern "C"
{
	#ifdef _WIN32
		#include <winsock2.h>
		typedef int socklen_t;
	#else
	    #include <unistd.h>
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <sys/ioctl.h>
		#include <arpa/inet.h>
        #define INVALID_SOCKET -1
		typedef int SOCKET;
		typedef struct sockaddr_in SOCKADDR_IN;
		typedef struct sockaddr_in6 SOCKADDR_IN6;
		typedef struct sockaddr SOCKADDR;
	#endif
}

#include "../Core/Errors.hpp"
#include "NetworkException.hpp"
#include "IpEndPoint.hpp"


/// @brief Namespace définissant la partie réseau de la bibliothèque
namespace Network
{
	//! Classe abstraite permettant de manipuler une socket.
	class Socket
	{
		public:
		
		//! @brief Detructeur de la socket
		virtual ~Socket();
		
		//! @brief Lie la socket avec un point de terminaison local. 
		//! @param host Point de terminaison à lier avec la socket serveur.
		virtual void bind(const IpEndPoint& host) throw(NetworkException) = 0;
		
		//! @brief Ferme la connexion en lecture et écriture et 
		//! libère les ressources utilisées par la socket.
		virtual void finish() = 0;
		
		//! @brief Permet de connaitre le nombre de bytes disponible 
		//! en lecture sur la socket. 
		//!
		//! Dégage une exception du type NetworkException 
		//! si la socket est fermée.
		//! @return Renvoie le nombre de bytes disponibles en lecture 
		//!         sur la socket. 
		virtual long available() throw(NetworkException) = 0;
		
		//! @brief Permet de récupérer le point de terminaison 
		//! local de la socket.
		//! 
		//! La méthode dégage une exception NetworkException si la socket 
		//! n'est pas en état de délivrer le point de terminaison local. 
		//! @return Point de terminaison local de la socket
		virtual const IpEndPoint& localEndPoint() const throw(NetworkException) = 0;
		
		bool operator==(const Socket& other);
		bool operator!=(const Socket& other);
		
		
		protected:
		
		//! @brief Permet de récupérer en lecture la socket native
		//! @return Socket native (non modifiable)
		virtual const SOCKET& socket() const = 0;
		
		
		friend class SelectSocketSet;
		friend class EpollSocketSet;
	};
}


#endif


