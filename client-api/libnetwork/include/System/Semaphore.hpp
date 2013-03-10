#ifndef SYSTEM_SEMAPHORE_H
#define SYSTEM_SEMAPHORE_H


extern "C"
{
	#include <pthread.h>
	#include <semaphore.h>
}


namespace System
{
	//! Classe permettant de créer et de manipuler une sémaphore. 
	class Semaphore
	{
		private:
		
		sem_t				_sem;
		
		
		public:
		
		//! @brief Crée une sémaphore avec 'value' comme valeur. 
		//! @param value Valeur à affecter à la sémaphore.
		Semaphore(int value);
		
		//! @brief Détruit la sémaphore
		virtual ~Semaphore();
		
		//! @brief Tente de décrémenter la value de la sémaphore. 
		//!
		//! Si la valeur de la semaphore est negative, la méthode 
		//! bloque jusqu'à ce que la valeur de la sémaphore soit 
		//! strictement positive via l'appel à la méthode give.
		void take();
		
		//! @brief Incrémente la valeur de la sémaphore.
		void give();
		
		//! @brief Tente de décrémenter la value de la sémaphore. 
		//!
		//! Si la valeur de la semaphore est strictement positive, 
		//! la méthode la décrémente et renvoi true. Sinon, celle-ci 
		//! n'est pas décrémenté et la fonction rend la main en 
		//! renvoyant false. 
		//! @return True si la sémaphore a pu être décrémenté,
		//!         renvoie false dans le cas contraire.
		bool tryTake();
		
		//! @brief Permet de connaitre la valeur de la sémaphore.
		//! @return Valeur de la sémaphore.
		int getValue();
	};
}


#endif


