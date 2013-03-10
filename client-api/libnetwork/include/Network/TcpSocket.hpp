#ifndef NETWORK_TCPSOCKET_H
#define NETWORK_TCPSOCKET_H


#include <string>
#include <cstring>

extern "C"
{
	#ifdef _WIN32
		#include <Ws2tcpip.h>
	#endif
}

#include "../Core/ByteArray.hpp"
#include "Socket.hpp"
#include "NetworkException.hpp"
#include "IpEndPoint.hpp"
#include "Errors.hpp"


namespace Network
{
	//! Classe permettant de créer et manipuler une socket respectant le protocole TCP. 
	class TcpSocket : public Socket
	{
		private:
		
		struct Data
		{
			int				instance;
			SOCKET			sock;
			IpEndPoint*		localAddr;
			IpEndPoint*		remoteAddr;
		} *_data;
		
		
		public:
		
		//! @brief Constructeur de sockets TCP. 
		//!
		//! Dégage une exception NetworkException si la 
		//! socket n'a pas pu être créée (par exemple si votre machine 
		//! ne suporte pas le protocole TCP ou si vous n'avez pas les 
		//! droits suffisants pour effectuer cette action). 
		TcpSocket() throw(NetworkException);
		
		//! @brief Contructeur de recopie. 
		TcpSocket(const TcpSocket& other);
		
		//! @brief Detructeur de la socket TCP. 
		//!
		//! Cette fonction libère les ressource utilisé et ferme la 
		//! connexion établie via la socket que lorsque toute les instance 
		//! de la socket (copies) ont été détruite. 
		//! Utilisez la méthode finish pour terminer la communication et 
		//! libérer les ressources avant cela. 
		virtual ~TcpSocket();
		
		//! @brief Permet de se connecter sur un point de terminaison. 
		//! @param host Point de terminaison sur lequel la socket 
		//!             doit être connectée. 
		void connect(const IpEndPoint& host) throw(NetworkException);
		
		void bind(const IpEndPoint& host) throw(NetworkException);
		
		//! @brief Met en écoute la socket serveur. 
		//! @param backlog Nombre de connexions simultanées possibles 
		//! sur la file d'attente des connexions.
		void listen(unsigned int backlog = 5) throw(NetworkException);
		
		//! @brief Accepte une connexion sur la socket serveur. 
		//! @return Socket accepté. 
		TcpSocket accept() throw(NetworkException);
		
		//! @brief Envoie des données sur la socket. 
		//!
		//! Dégage une exception NetworkException si aucune donnée n'a pu 
		//! être envoyé sur le réseau (dans la cas où la socket n'est plus 
		//! connectée à l'hôte distant).
		//! @param data Buffer qui contient les données à envoyer
		//! @param len Nombre de bytes à envoyer
		//! @param flags Flags d'envoie. Celui-ci peut prendre les 
		//!              même valeurs que le flags de la fonction send
		//! @return Nombre de bytes envoyés.
		int send(const void* data, int len, int flags = 0) throw(NetworkException);
		
		//! @brief Reçoit des données depuis la socket. 
		//!
		//! Si il n'y a rien à lire depuis le réseau. La méthode attend 
		//! jusqu'à ce que se soit le cas, lit les données et rend la main. 
		//! Dégage une exception NetworkException si aucune donnée n'a pu être 
		//! lu depuis le réseau. 
		//! @param data Buffer dans lequel les données reçues sont écrites
		//! @param len Nombre de bytes à reçevoir
		//! @param flags Flags de reçeption. Celui-ci peut prendre les 
		//!              même valeurs que le flags de la fonction recv
		//! @return Nombre de bytes reçus.
		int recv(void* data, int len, int flags = 0) throw(NetworkException);
		
		//! @brief Envoie un entier de 4 octets sur la socket. 
		//!
		//! Dégage une exception NetworkException si les donnée n'ont pas pu 
		//! toutes être envoyés sur le réseau ou si cet envoi est partiel.
		//! (dans la cas où la socket n'est plus connectée à l'hôte distant).
		//! L'endianness de l'entier envoyé est celle du réseau (Big endian).
		//! @param data Entier de 4 octets envoyé sur la socket
		void sendInt4(int data) throw(NetworkException);
		
		//! @brief Envoie un entier de 2 octets sur la socket. 
		//!
		//! Dégage une exception NetworkException si les donnée n'ont pas pu 
		//! toutes être envoyés sur le réseau ou si cet envoi est partiel
		//! (dans la cas où la socket n'est plus connectée à l'hôte distant).
		//! L'endianness de l'entier envoyé est celle du réseau (Big endian).
		//! @param data Entier de 2 octets envoyé sur la socket
		void sendInt2(short data) throw(NetworkException);
		
		//! @brief Envoie un entier d'un seul octets sur la socket. 
		//!
		//! Dégage une exception NetworkException si l'octet n'a pu être envoyé 
		//! sur le réseau (si la socket n'est plus connectée à l'hôte distant).
		//! L'endianness de l'entier envoyé est celle du réseau (Big endian).
		//! @param data Entier de 4 octets envoyé sur la socket
		void sendChar(char data) throw(NetworkException);
		
		//! @brief Reçoit un entier de 4 octets depuis la socket. 
		//!
		//! Dégage une exception NetworkException si les donnée n'ont pas pu 
		//! toutes être reçu sur le réseau ou si cet réception est partiel.
		//! (dans la cas où la socket n'est plus connectée à l'hôte distant).
		//! L'endianness de l'entier reçu reçu doit être est celle du réseau
		//! (Big endian).
		//! @return Entier de 4 octets reçu depuis la socket
		int recvInt4() throw(NetworkException);
		
		//! @brief Reçoit un entier de 2 octets depuis la socket. 
		//!
		//! Dégage une exception NetworkException si les donnée n'ont pas pu 
		//! toutes être reçu sur le réseau ou si cet réception est partiel.
		//! (dans la cas où la socket n'est plus connectée à l'hôte distant). 
		//! L'endianness de l'entier reçu reçu doit être est celle du réseau
		//! (Big endian).
		//! @return Entier de 2 octets reçu depuis la socket
		short recvInt2() throw(NetworkException);
		
		//! @brief Reçoit un entier d'un seul octet depuis la socket. 
		//!
		//! Dégage une exception NetworkException si rien n'a été reçu 
		//! (dans la cas où la socket n'est plus connectée à l'hôte distant). 
		//! @return Entier d'un octets reçu depuis la socket
		char recvChar() throw(NetworkException);
		
		//! @brief Envoie des données sur la socket. 
		//!
		//! Dégage une exception NetworkException si aucune donnée n'a pu 
		//! être envoyé sur le réseau (dans la cas où la socket n'est plus 
		//! connectée à l'hôte distant).
		//! @param data Buffer qui contient les données à envoyer
		//! <br/>
		//! Note : Il est conseillé d'utiliser cette fonction en 
		//! bufferisant les données à envoyer pour obtenir de 
		//! meilleurs performances par rapport aux méthodes tel que 
		//! sendInt4, sendInt2 et sendChar.
		void send(const Core::ByteArray& data) throw(NetworkException);
		
		//! @brief Reçoit des données depuis la socket. 
		//!
		//! Si il n'y a rien à lire depuis le réseau. La méthode attend 
		//! jusqu'à ce que se soit le cas, lit les données et rend la main. 
		//! Dégage une exception NetworkException si aucune donnée n'a pu être 
		//! lu depuis le réseau. Le buffer reçu est vidé puis redimentionné 
		//! selon la taille des données reçus.
		//! @param data Buffer dans lequel les données reçues sont écrites
		//! <br/>
		//! Note : Il est conseillé d'utiliser cette fonction en 
		//! bufferisant les données à envoyer pour obtenir de 
		//! meilleurs performances par rapport aux méthodes tel que 
		//! readInt4, readInt2, readChar.
		void recv(Core::ByteArray& data) throw(NetworkException);
		
		//! @brief Reçoit un bloc de données d'une taille limitée depuis la socket. 
		//!
		//! Tant que 'len' bytes n'ont pas été reçus, la méthode bloque 
		//! jusqu'à se que ce soit le cas. Si la socket se déconnecte 
		//! pour une raison quelconque durant l'exécution de cette méthode, 
		//! deux cas sont possibles : 
		//! <ul>
		//!     <li>
		//!         soit aucune données n'a été reçu, dans se cas l'exception 
		//!         NetworkException est levée.
		//!     </li>
		//!     <li>
		//!         soit la taille des données reçus est insuffisante, 
		//!         dans se cas aucune exception n'est levée et le buffer 
		//!         est redimentionné à la taille des données reçus.
		//!     </li>
		//! </ul>
		//! @param data Buffer dans lequel les données reçues sont écrites
		//! Note : Il est conseillé d'utiliser cette fonction en 
		//! bufferisant les données à envoyer pour obtenir de 
		//! meilleurs performances par rapport aux méthodes tel que 
		//! readInt4, readInt2, readChar.
		//! @param data Buffer dans lequel les données reçues sont écrites
		//! @param len Taille des données à recevoir
		void recv(Core::ByteArray& data, int len) throw(NetworkException, Core::ArgumentException);
		
		//! @brief Reçoit un bloc de données terminant par un separateur 
		//! ou par une déconnexion depuis la socket. 
		//!
		//! Dégage une exception NetworkException si aucune donnée n'a pu être 
		//! lu depuis le réseau. Le buffer reçu est vidé puis redimentionné 
		//! selon la taille des données reçus.
		//! @param data Buffer dans lequel les données reçues sont écrites
		//! @param separator Séparateur
		void recv(Core::ByteArray& data, const Core::ByteArray& separator) throw(NetworkException, Core::ArgumentException);
		
		//! @brief Reçoit des données depuis la socket sans les retirer 
		//! de la pile réseau.
		//!
		//! Identique à la méthode recv qui prend les même paramètres 
		//! à la différence que les données qui sont reçu ne sont pas retirées 
		//! de la pile réseau.
		void peek(Core::ByteArray& data) throw(NetworkException);
		
		//! @brief Reçoit un bloc de données d'une taille limitée depuis la 
		//! socket de sans le retirer a pile réseau.
		//!
		//! Identique à la méthode recv qui prend les même paramètres 
		//! à la différence que les données qui sont reçu ne sont pas retirées 
		//! de la pile réseau.
		void peek(Core::ByteArray& data, int len) throw(NetworkException, Core::ArgumentException);
		
		void finish();
		
		//! @brief Permet de connaitre le nombre de bytes disponibles 
		//! en lecture sur la socket.
		//!
		//! Déguage une exception du type NetworkException 
		//! si la socket est fermée. 
		//! @return Renvoie le nombre de bytes disponibles en lecture 
		//!         sur la socket. Ce nombre peut être éventuellement 
		//!         tronqué par le système si il est trop élevé.
		long available() throw(NetworkException);
		
		//! @brief Permet de savoir si la socket à été fermé par l'hôte distant. 
		//!
		//! La socket peut être fermé par l'hôte distant 
		//! ou par nous même via les méthodes close ou shutdown. 
		//! @return Renvoie true si la connexion à été fermé par l'hôte 
		//!         distant et si il n'y à plus rien à lire dessus. 
		//!         Renvoie false dans le cas contraire.
		bool finished() throw(NetworkException);
		
		//! @brief Permet de récupérer le point de terminaison 
		//! local de la socket.
		//!
		//! Cette méthode ne fonctionne qu'après l'appel des 
		//! méthodes connect et bind ou si la socket est récupérée
		//! récupérée depuis la méthode accept d'une autre socket. 
		//! Si jamais la condition précédente n'est pas respectée, 
		//! la méthode dégage une exception NetworkException.
		//! @return Point de terminaison local de la socket
		const IpEndPoint& localEndPoint() const throw(NetworkException);
		
		//! @brief Permet de récupérer le point de terminaison 
		//! distant de la socket.
		//!
		//! Cette méthode ne fonctionne qu'après l'appel de la méthode 
		//! connect ou si la socket est récupérée depuis la méthode 
		//! accept d'une autre socket. 
		//! Si jamais la condition précédente n'est pas respectée, 
		//! la méthode dégage une exception NetworkException.
		//! @return Point de terminaison distant de la socket
		const IpEndPoint& remoteEndPoint() const throw(NetworkException);
		
		TcpSocket& operator=(const TcpSocket& other);
		
		
		protected:
		
		//! @brief Contruit une socket TCP à partir d'élements natifs.
		//! @param sock Socket native
		//! @param sockaddr Point de terminaison natif
		TcpSocket(const SOCKET& sock, const SOCKADDR* sockaddr);
		
		const SOCKET& socket() const;
	};
}


#endif


