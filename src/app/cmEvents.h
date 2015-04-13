/********************************************************************
 --------------------------------------------------------------------
 --           _,(_)._
 --      ___,(_______).      ____
 --    ,'__.           \    /\___\-.
 --   /,' /             \  /  /     \
 --  | | |              |,'  /       \
 --   \`.|                  /    ___|________
 --    `. :           :    /     COLORMOTOR
 --      `.            :.,'        Graphics and Multimedia Framework
 --        `-.________,-'          © Daniel Berio
 --								   http://www.enist.org
 --								   drand48@gmail.com
 --
 --------------------------------------------------------------------
 ********************************************************************/
#pragma once 

#include "cmCore.h"

namespace cm 
{

/// Simple event class 
/// Derive from this for custom events
class Event : public RttiObject
{
public:
	RTTIOBJECT("Event",Event,RttiObject)
	unsigned int msg;
	unsigned int data;
};

/// Event handler result 
/// \todo tutto.... :P
enum EVENT_RESULT
{
	/// event hasnt been handled
	EVENT_UNHANDLED = 0,	
	/// event has been handled
	EVENT_HANDLED = 1,	
	/// event has been eaten by control, dont propagate to children
	EVENT_EATEN				
};

//////////////////////////////////////////////////////////////////////////
/// Base event broadcaster
/// Sends events to listeners
/// \todo derive from RTTI
template <class T> 
class EventBroadcaster
{


public:
	EventBroadcaster()
	{
		_listeners.clear();
	}

	virtual ~EventBroadcaster()
	{
		_listeners.clear();
	};

	void	clear()
	{
		_listeners.clear();
	}

	int addListener( T * l ) { _listeners.push_back(l); return _listeners.size()-1; };
	int getNumListeners() const { return _listeners.size(); }
	T * getListener( int i ) { return _listeners[i]; }

	
private:
	std::vector <T*> _listeners;
};

}