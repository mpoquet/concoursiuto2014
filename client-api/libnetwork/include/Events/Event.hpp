#ifndef EVENT_EVENT_H
#define EVENT_EVENT_H


#include <stdexcept>
#include <list>

#include "EventArgs.hpp"
#include "EventHandlerBase.hpp"
#include "EventHandler.hpp"


//! @brief Namespace définissant la notion d'évenements
namespace Events
{
	//! @brief Classe definissant un événement
	class Event
	{
		private:
		
		std::list<EventHandlerBase*> handlers;
		
		
		public:
		
		//! @brief Methode connectant l'évenement à un handler.
		//! 
		//! Lorsqu'un évenement survient, il execute tous les handlers
		//! qui sont connecté à cet événement
		//! @param handler Handler à connecter à l'événement
		template<typename T>
		Event& operator+=(const EventHandler<T>& handler)
		{
			handlers.push_back(new EventHandler<T>(handler));
			return *this;
		}
		
		//! @brief Simule l'événement avec des paramètres
		//! @param args Argument passé en paramètre aux handlers
		void operator()(const EventArgs& args) const;
		
		virtual ~Event();
	};
}


#endif


