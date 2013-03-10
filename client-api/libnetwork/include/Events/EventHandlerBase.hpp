#ifndef EVENT_EVENTHANDLERBASE_H
#define EVENT_EVENTHANDLERBASE_H


#include "EventArgs.hpp"


namespace Events
{
	class EventHandlerBase
	{
		public:
		
		virtual void operator()(const EventArgs& args);
		virtual void call(const EventArgs&) const = 0;
		virtual ~EventHandlerBase();
	};
}


#endif


