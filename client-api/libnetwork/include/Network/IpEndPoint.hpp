#ifndef NETWORK_IPENDPOINT_H
#define NETWORK_IPENDPOINT_H


extern "C"
{
	#if defined _WIN32
		#include <winsock2.h>
	#else
		#include <arpa/inet.h>
		typedef struct sockaddr_in SOCKADDR_IN;
		typedef struct sockaddr_in6 SOCKADDR_IN6;
		typedef struct sockaddr SOCKADDR;
	#endif
}

#include "IpAddress.hpp"


namespace Network
{
	//! Classe définissant un point de terminaison sur le réseau. 
	class IpEndPoint : public IpAddress
	{
		public:
		
		//! @brief Construit un point de terminaison à partir d'une 
		//! adresse IP et d'un port. 
		//! @param ip Adresse IP du point de terminaison
		//! @param port Port du point de terminaison
		IpEndPoint(const IpAddress& ip, unsigned short port);
		
		//! @brief Construit un point de terminaison à partir de la 
		//! structure native SOCKADDR. 
		//!
		//! Dégage une exception ArgumentException si le pointeur 
		//! sur la structure native passée en argument est nulle 
		//! ou si le protocole utilisé n'est pas supporté 
		//! (par exemple les socket UNIX). 
		//! @param addr Structure native décrivant un point de terminaison
		IpEndPoint(const SOCKADDR* addr) throw(Core::ArgumentException);
		
		//! @brief Destructeur de la classe IpEndPoint. 
		virtual ~IpEndPoint();
		
		//! @brief Permet de connaître le port du point de terminaison. 
		//! @return Port du point de terminaison.
		unsigned short port() const;
	};
	
	std::ostream& operator<<(std::ostream& out, const IpEndPoint& ep);
}


#endif


