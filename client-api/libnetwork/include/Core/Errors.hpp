#ifndef CORE_ERRORS_H
#define CORE_ERRORS_H


#include <string>
#include <cstring>

#ifndef _WIN32
	#include <errno.h>
#endif


namespace Core
{
	//! @brief Récupère, sous la forme d'une chaine de caractères, 
	//! une erreur système survenue dans le thread courant 
	//! à partir du numéro de cette l'erreur. 
	//! @return Dernière erreur système produite.
	std::string getErrorString(int errNumber);
}


#endif


