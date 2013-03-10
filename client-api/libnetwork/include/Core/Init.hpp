#ifndef CORE_INIT_H
#define CORE_INIT_H


extern "C"
{
	#if defined (WIN32)
		#include <winsock2.h>
	#endif
}

#include "InitialisationException.hpp"
#include "../Network/Errors.hpp"


/// @brief Namespace définissant toutes les fonctionnalités 
/// et classes nécessaire à la bibliothèque pour fonctionner
namespace Core
{
	void load() throw(InitialisationException);
	void unload() throw(InitialisationException);
}


#endif


