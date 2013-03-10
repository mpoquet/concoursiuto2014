#ifndef NETWORK_UDPSOCKET_H
#define NETWORK_UDPSOCKET_H


#include <string>
#include <cstring>

#include "Socket.hpp"
#include "NetworkException.hpp"
#include "IpEndPoint.hpp"
#include "Errors.hpp"


namespace Network
{
	//! Classe permettant de créer et manipuler une socket respectant le protocole UDP. 
	class UdpSocket : public Socket
	{
		private:
		
		struct Data
		{
			int				instance;
			SOCKET			sock;
			IpEndPoint*		localAddr;
		} *_data;
		
		
		public:
		
		//! @brief Constructeur de sockets UDP. 
		//!
		//! Dégage une exception NetworkException si la socket n'a pas pu être 
		//! créée (par exemple si votre machine ne suporte pas le protocole UDP ou 
		//! si vous n'avez pas les droit suffisants pour effectuer cette action).
		UdpSocket() throw(NetworkException);
		
		//! @brief Contructeur de recopie. 
		UdpSocket(const UdpSocket& other);
		
		//! @brief Detructeur de la socket UDP. 
		//!
		//! Cette fonction ne libère les ressource utilisé et ferme la 
		//! connexion établie via la socket que lorsque toute les instance 
		//! de la socket (copies) ont été détruite.
		//! Utilisez la méthode finish pour terminer la communication et 
		//! libérer les ressources avant cela. 
		virtual ~UdpSocket();
		
		void bind(const IpEndPoint& host) throw(NetworkException);
		
		//! @brief Envoie des données sur un point de terminaison. 
		//!
		//! Si une erreur critique est survenue durrant l'envoie 
		//! des données, une exception NetworkException est dégagée.
		//! @param endPoint [in] Point de terminaison sur lequel les données sont écrite
		//! @param data [in] Buffer de données à transmettre
		//! @param len [in] Taille du buffer (en bytes)
		//! @param flags [in] Flags à attribuer
		//! @return Nombre de bytes écrits sur le réseau
		void sendto(const IpEndPoint& endPoint, const void* data, size_t len, int flags = 0) throw(NetworkException);
		
		//! @brief Reçoit des données depuis un point de terminaison. 
		//!
		//! Si une erreur critique est survenue durrant la réçeption 
		//! des données, une exception NetworkException est dégagée.
		//! @param endPoint [out] Point de terminaison depuis lequel les données sont reçus
		//! @param data [out] Buffer de réçeption dans lequel les données sont écrites
		//! @param len [in] Nombre de bytes à lire depuis le réseau
		//! @param flags [in] Flags à attribuer
		//! @return Nombre de bytes lus depuis le réseau
		int recvfrom(IpEndPoint& endPoint, void* data, size_t len, int flags = 0) throw(NetworkException);
		
		void finish();
		
		//! @brief Permet de connaitre le nombre de bytes disponible 
		//! en lecture sur la socket. 
		//!
		//! Déguage une exception du type NetworkException 
		//! si la socket est fermée.
		//! @return Renvoie le nombre de bytes disponibles en lecture 
		//!			sur la socket. Si plusieurs datagrammes ont 
		//!			été transmits, la méthode renvoie la taille 
		//!			du premier datagramme lisible. 
		long available() throw(NetworkException);
		
		//! @brief Permet de récupérer le point de terminaison 
		//! local de la socket.
		//! 
		//! Cette méthode ne fonctionne qu'après l'appel de la 
		//! méthodes bind.
		//! Si jamais la condition précédente n'est pas respectée, 
		//! la méthode dégage une exception NetworkException.
		//! @return Point de terminaison local de la socket
		const IpEndPoint& localEndPoint() const throw(NetworkException);
		
		UdpSocket& operator=(const UdpSocket& other);
		
		
		protected:
		
		const SOCKET& socket() const;
	};
}


#endif


