#ifndef NETWORK_NETWORKEXCEPTION_H
#define NETWORK_NETWORKEXCEPTION_H


#include <string>
#include <stdexcept>


namespace Network
{
	//! @brief Classe utilisée pour générer des erreurs causées 
	//!			durant la manipulation de sockets.
	//! 
	//! Cette classe peut utilisée par exemple dans 
	//!	le cas de la fermeture d'une connexion prématurément.
	class NetworkException : public std::runtime_error
	{
		public:
		
		//! @brief Contruit l'exception à partir d'un message d'erreur.
		//! @param what Message d'erreur décrivant le problème
		explicit NetworkException(const std::string& what) : std::runtime_error(what)
		{
		
		}
	};
}


#endif


