#ifndef NETWORK_ERRORS_H
#define NETWORK_ERRORS_H


#include <string>
#include <cstring>

#ifndef _WIN32
	#include <errno.h>
#endif

#include "../Core/Errors.hpp"


namespace Network
{
	//! @brief Récupère la dernière erreur réseau survenue dans le 
	//! thread courant sous la forme d'une chaine de caractères. 
	//! @return Dernière erreur réseau produite.
	std::string lastError();
}


#endif


