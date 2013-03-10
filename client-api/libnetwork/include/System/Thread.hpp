#ifndef SYSTEM_THREAD_H
#define SYSTEM_THREAD_H


#include <cstring>
#include <ctime>

extern "C"
{
	#include <pthread.h>
	
	#ifdef _WIN32
		#include <winsock2.h>
	#else
		#include <sys/time.h>
	#endif
}

#include "../Core/Errors.hpp"
#include "ThreadException.hpp"
#include "Mutex.hpp"


//! @brief Namespace définissant les classes et les fonctionnalités 
//! lié au système qui sont offert par la bibliothèque (threads, mutex, etc.)
namespace System
{
	//! @brief Classe permettant de créer et manipuler un threads. 
	//! 
	//! Il est vivement conseillé d'allouer dynamiquement cette classe.
	//! En effet, la classe ne doit absolument pas être détruite 
	//! durant l'execution du thread.
	class Thread
	{
		private:
		
		Mutex				_started;
		pthread_t			_thread;
		
		
		public:
		
		//! @brief Constructeur de threads. 
		//!
		//! Initialise les ressources utiles pour créer un thread. 
		//! Le thread ne commence que lorsque les méthodes exec 
		//! ou start sont appelées. 
		Thread();
		
		//! @brief Destructeur de threads. 
		//!
		//! Le destructeur de la classe thread libère les ressources 
		//! associés au thread (si possible), mais celle-ci ne termine pas 
		//! l'éxecution du thread. 
		virtual ~Thread();
		
		//! @brief Créé un nouveau thread s'éxécutant concurremment 
		//! avec le thread appelant (mis en attente).
		//!
		//! Le thread appelant est mis en attente pendant se temps. 
		//! La méthode rend la main uniquement lorsque le thread 
		//! se termine. Le nouveau thread exécute la méthode run 
		//! (qui doit être redéfinie). Le nouveau thread s'achève 
		//! lorsque la fonction run se termine. 
		void exec() throw(ThreadException);
		
		//! @brief Créé un nouveau thread s'éxécutant concurremment 
		//! avec le thread appelant. 
		//!
		//! Le nouveau thread exécute la méthode run 
		//! (qui doit être redéfinie). Le nouveau thread s'achève 
		//! lorsque la fonction run se termine. 
		void start() throw(ThreadException);
		
		//! @brief Suspend l'exécution du thread appelant jusqu'à ce 
		//! que le thread identifié achève son exécution. 
		//!
		//! Contrairement aux threads natifs qui n'autorise qu'un seul 
		//! thread à attendre la mort d'un thread donné, cette méthode 
		//! permet à plusieurs thread d'attendre simultanément la mort 
		//! d'un thread donné.
		void wait();
		
		//! @brief Permet de savoir si le thread est lancé.
		//! @return True si le thread est lancé. 
		//!         False dans le cas contraire. 
		bool isRunning();
		
		
		protected:
		
		//! @brief Méthode définissant le travail que doit réaliser 
		//! le thread durant son exécution.
		//!
		//! Cette méthode est virtuelle pure et doit être implementée dans 
		//! les classes dérivées. Lorsque cette méthode se termine, 
		//! l'éxecution du thread prend fin. 
		virtual void run() = 0;
		
		
		public:
		
		//! @brief Met en attente le thread qui éxecute cette 
		//! méthode pendant un nombre de secondes spécifié. 
		//! @param secondes Nombre de secondes à attendre. 
		static void sleep(double secondes);
		
		//! @brief Compare le thread courant avec un autre.
		//! @return true si les deux thread sont en fait qu'un seul 
		//!			et unique thread (avec le même context d'exécution)
		//!			et false dans le cas contraire.
		//! @param other Thread avec lequel le thread courant est comparé
		bool operator==(const Thread& other) const;
		
		
		private:
		
		static void* startRoutine(void* thread);
	};
}


#endif


