#ifndef NETWORK_DNS_H
#define NETWORK_DNS_H


#include <vector>
#include <set>
#include <cstring>

extern "C"
{
	#ifdef _WIN32
        #ifndef WINVER
            #define WINVER 0x0501 // Compatibilité avec MinGW pour WindowsXP
        #endif
        
		#include <winsock2.h>
        #include <Ws2tcpip.h>
	#else
		#include <unistd.h>
		#include <limits.h>
		#include <netdb.h>
		#include <arpa/inet.h>
		#include <errno.h>
		typedef struct sockaddr_in SOCKADDR_IN;
		typedef struct sockaddr_in6 SOCKADDR_IN6;
		typedef struct sockaddr SOCKADDR;
	#endif
}

#include "Errors.hpp"
#include "NetworkException.hpp"
#include "IpAddress.hpp"


namespace Network
{
	//! Classe fournissant des méthodes statiques permettant de résoudre un nom de dommaine. 
	class Dns
	{
		public:
		
		//! @brief Permet de récupérer le nom de la machine 
		//! qui éxecute cette méthode. 
		//!
		//! Dégage l'exception NetworkException si le système n'a pas pu 
		//! récupérer le nom de l'hôte. 
		//! @return Nom de la machine. 
		static std::string getHostName() throw(NetworkException);
		
		//! @brief Permet de connaitre le nom de dommaine d'une 
		//! machine à partir de son adresse IP.
		//!
		//! Dégage une exception NetworkException si le Dns n'a pas pu 
		//! résoudre l'adresse (par exemple, si il n'y a pas assez de 
		//! mémoire sur le système ou si la machine n'est pas 
		//! connectée à internet).
		//! @return Nom de domaine de la machine distante. 
		static std::string getHostName(const IpAddress& ip) throw(NetworkException);
		
		//! @brief Permet résoudre les addresses IP d'un nom de dommaine.
		//!
		//! Dégage une exception NetworkException si le Dns n'a pas pu 
		//! résoudre l'adresse (par exemple, si il n'y a pas assez de 
		//! mémoire sur le système ou si la machine n'est pas 
		//! connectée à internet).
		//! @param host Nom de dommaine
		//! @return Liste des adresses IP sous forme de vecteur
		static std::vector<IpAddress> getHostAddresses(const std::string& host) throw(NetworkException);
		
		
		private:
		
		static std::string errorString(int errVal);
		
		//! Structure interne utile pour la résolution de nom de dommaine.
		template <typename T>
		struct EqualAddr
		{
			bool operator()(const T* a, const T* b);
		};
	};
}


#endif


