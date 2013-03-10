#ifndef EVENT_EVENTHANDLER_H
#define EVENT_EVENTHANDLER_H


#include <stdexcept>

#include "EventHandlerBase.hpp"


namespace Events
{
	//! @brief Classe représentant la méthode d'un objet.
	//! 
	//! Cette classe permettra de gérer un événement 
	//! particulier en s'y connectant.
	//! Cette classe est en quelque sorte l'équivalent d'un slot de Qt.
	template<typename T>
	class EventHandler : public EventHandlerBase
	{
		public:
		
		typedef void (T::*Method)(const EventArgs&);
		
		
		T& _object;
		const Method _method;
		
		
		EventHandler(T* object, Method method) : _object(*object), _method(method)
		{
			if(object == NULL)
				throw std::runtime_error("object argument must not be set null in EventHandler constructor");
			
			if(method == NULL)
				throw std::runtime_error("method argument must not be set null in EventHandler constructor");
		}
		
		virtual void call(const EventArgs& args) const
		{
			(_object .* _method)(args);
		}
	};
	
	class StaticEventHandler : public EventHandlerBase
	{
		public:
		
		typedef void (*Function)(const EventArgs&);
		
		
		const Function _function;
		
		
		StaticEventHandler(Function function) : _function(function)
		{
			if(function == NULL)
				throw std::runtime_error("function argument must not be set null in EventHandler constructor");
		}
		
		virtual void call(const EventArgs& args) const
		{
			_function(args);
		}
	};
}


#endif


