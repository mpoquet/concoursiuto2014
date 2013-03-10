#ifndef SYSTEM_MUTEX_H
#define SYSTEM_MUTEX_H


extern "C"
{
	#include <pthread.h>
}


namespace System
{
	//! Classe permettant de créer et de manipuler une mutex. 
	class Mutex
	{
		private:
		
		pthread_mutex_t		_mutex;
		
		
		public:
		
		//! @brief Constructeur de mutex. 
		Mutex();
		
		//! @brief Destructeur de mutex. 
		virtual ~Mutex();
		
		//! @brief Bloque la mutex. 
		//!
		//! Si la mutex est déjà bloquée, le thread appelant 
		//! est bloqué jusqu'à ce que la mutex soit débloquée.
		void lock();
		
		//! @brief Débloque la mutex précedement bloquée. 
		//!
		//! La mutex ne peut être débloquée que par le thread qui l'a bloqué. 
		void unlock();
		
		//! Tente de bloquer la mutex. 
		//!
		//! Si celle-ci est déjà bloquée, la 
		//! méthode rend la main sans rien faire, sinon la méthode bloque 
		//! la mutex et rend la main juste après.
		//! @return true si la mutex a pu être bloquée, false sinon.
		bool tryLock();
	};
}


#endif


