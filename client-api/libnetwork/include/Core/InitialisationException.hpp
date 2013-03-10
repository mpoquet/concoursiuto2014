#ifndef CORE_INITIALISATIONEXCEPTION_H
#define CORE_INITIALISATIONEXCEPTION_H


#include <string>
#include <stdexcept>


namespace Core
{
	//! @brief Classe utilisée pour générer des erreurs 
	//!			dû à un problème lors de l'initialisation de la
	//! 		bibliothèque ou de ça destruction.
	class InitialisationException : public std::runtime_error
	{
		public:

		//! @brief Contruit l'exception à partir d'un message d'erreur.
		//! @param what Message d'erreur décrivant le problème
		explicit InitialisationException(const std::string& what) : std::runtime_error(what)
		{
			
		}
	};
}


#endif


