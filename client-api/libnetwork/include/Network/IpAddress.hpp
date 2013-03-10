#ifndef NETWORK_IPADDRESS_H
#define NETWORK_IPADDRESS_H


#include <string>
#include <cstring>
#include <cstdio>

extern "C"
{
	#ifdef _WIN32
        #ifndef WINVER
            #define WINVER 0x0501 // Compatibilité avec MinGW pour WindowsXP
        #endif
        
		#include <winsock2.h>
		#include <Ws2tcpip.h>
	#else
		#include <arpa/inet.h>
		typedef struct sockaddr_in SOCKADDR_IN;
		typedef struct sockaddr_in6 SOCKADDR_IN6;
		typedef struct sockaddr SOCKADDR;
	#endif
}

#include "../Core/ArgumentException.hpp"


namespace Network
{
	//! Classe définissant une adresse IP en version 4 ou 6 sur le réseau. 
	class IpAddress
	{
		public:
		
		//! @brief Type d'adresse IP : 
		//! <ul>
		//! 	<li>IPV4 : Adresse IP en version 4.</li>
		//! 	<li>IPV6 : Adresse IP en version 6.</li>
		//! </ul>
		enum Type
		{
			IPV4,
			IPV6
		};
		
		
		public:
		
		//! Adresse IP en version 4 de la machine sur toutes les interfaces. 
		static const IpAddress ipv4_any;
		
		//! Adresse locale en version 4 de la machine (loopback). 
		static const IpAddress ipv4_localhost;
		
		//! Adresse IP en version 6 de la machine sur toutes les interfaces. 
		static const IpAddress ipv6_any;
		
		//! Adresse locale en version 6 de la machine (loopback). 
		static const IpAddress ipv6_localhost;
		
		
		private:
		
		union Data
		{
			short family;
			SOCKADDR base;
			SOCKADDR_IN v4;
			SOCKADDR_IN6 v6;
		} _addr;
		
		
		public:
		
		//! @brief Construit une adresse IP (version 4 ou 6) à partir 
		//! d'une chaine de caractères. 
		//!
		//! Dégage une exception ArgumentException si la chaine de 
		//! caractères passée en argument n'est pas une adresse IP valide.
		//! @param addr Adresse IP sous forme de chaine de caractères
		IpAddress(const std::string& addr) throw(Core::ArgumentException);
		
		//! @brief Permet de créer une adresse IP (version 4 ou 6) à 
		//! partir de la structure native SOCKADDR. 
		//!
		//! Dégage une exception ArgumentException si le pointeur sur la 
		//! structure native passée en argument est nulle ou si le protocole 
		//! utilisé n'est pas supporté (par exemple les socket UNIX).
		//! @param addr Structure native décrivant une adresse IP
		IpAddress(const SOCKADDR* addr) throw(Core::ArgumentException);
		
		//! @brief Destructeur de la classe IpAddress. 
		virtual ~IpAddress();
		
		//! @brief Permet de connaître le type de l'adresse IP. 
		//! @return IPV4 si l'adresse IP est en version 4 ou 
		//!         IPV6 si elle est en version 6. 
		Type type() const;
		
		//! @brief Permet de récupérer l'adresse IP sous forme de 
		//! chaine de caractères. 
		//! @return Adresse IP sous forme de chaine de caractères. 
		std::string ip() const;
		
		//! @brief Permet de récupérer la structure native 
		//! de l'adresse IP. 
		//! @return Structure native correspondante à l'adresse IP. 
		const SOCKADDR* sockaddr() const;
		
		//! @brief Permet de récupérer la taille qu'occupe la structure 
		//! native de l'adresse IP. 
		//! @return Taille (en bytes) de la structure native. 
		int sockaddrSize() const;
		
		
		protected:
		
		//! @brief Structure native de l'adresse IP. 
		SOCKADDR* sockaddr();
		
		
		private:
		
		//! @brief Méthode utile pour savoir si une chaine de caractère 
		//! correspond à une adresse IP en version 4. 
		bool isIpv4Addr(const std::string& addr) const;
	};
	
	std::ostream& operator<<(std::ostream& out, const IpAddress& ipa);
}


#endif


