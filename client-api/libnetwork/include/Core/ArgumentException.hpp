#ifndef CORE_ARGUMENTEXCEPTION_H
#define CORE_ARGUMENTEXCEPTION_H


#include <string>
#include <stdexcept>


namespace Core
{
	//! @brief Classe utilisée pour générer des erreurs 
	//!			dû à un mauvais passage d'arguments
	class ArgumentException : public std::runtime_error
	{
		public:

		//! @brief Contruit l'exception à partir d'un message d'erreur.
		//! @param what Message d'erreur décrivant le problème
		explicit ArgumentException(const std::string& what) : std::runtime_error(what)
		{
		
		}
	};
}


#endif


