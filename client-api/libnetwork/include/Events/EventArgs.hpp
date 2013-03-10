#ifndef EVENT_EVENTARGS_H
#define EVENT_EVENTARGS_H


namespace Events
{
	/// @brief Classe assurant la transmission des données entre 
	/// les événements et les handlers
	class EventArgs
	{
		public:
		
		virtual ~EventArgs();
	};
}


#endif


