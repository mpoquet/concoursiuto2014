#ifndef SYSTEM_THREADEXCEPTION_H
#define SYSTEM_THREADEXCEPTION_H


#include <string>
#include <stdexcept>


namespace System
{
	//! @brief Classe utilisée pour générer des erreurs
	//!			causées durant la manipulation de threads
	class ThreadException : public std::runtime_error
	{
		public:
		
		//! @brief Contruit l'exception à partir d'un message d'erreur.
		//! @param what Message d'erreur décrivant le problème
		explicit ThreadException(const std::string& what) : std::runtime_error(what)
		{
		
		}
	};
}


#endif


