#pragma once 


#include "cmCore.h"
#include "CMEvents.h"

namespace cm 
{



/// Mouse button flags	
enum MOUSEBUTTON
{
	MOUSE_LEFT = 0,
	MOUSE_MIDDLE = 1,
	MOUSE_RIGHT = 2
};


/// Static mouse state class.
/// Should find a way to handle multiple mouse devices.

#define MAX_MICE 3

class Mouse
{
public:
	static int isButtonDown( int btn, int mouseIndex = 0 ) { return buttonState[mouseIndex] & (1<<btn); }
	static int isLeftDown( int mouseIndex = 0 ) { return buttonState[mouseIndex] & (1<<MOUSE_LEFT); }
	static int isMiddleDown(  int mouseIndex = 0 ) { return buttonState[mouseIndex] & (1<<MOUSE_MIDDLE); }
	static int isRightDown(  int mouseIndex = 0 ) { return buttonState[mouseIndex] & (1<<MOUSE_RIGHT); }

public:
	static int getCurrentButton( int mouseIndex = 0 ) { return currentButton[mouseIndex]; }
	static int getButtonState( int mouseIndex = 0 ) { return buttonState[mouseIndex]; }
	static int getX( int mouseIndex = 0 ) { return mousex[mouseIndex]; }
	static int getY( int mouseIndex = 0 ) { return mousey[mouseIndex]; }
	static int getDX( int mouseIndex = 0 ) { return mousedx[mouseIndex]; }
	static int getDY( int mouseIndex = 0 ) { return mousedy[mouseIndex]; }
	
	static void init();
public:
	static int & x;
	static int & y;
	static int & dx;
	static int & dy;
	
	/// this tells which button is currently being processed
	static int currentButton[MAX_MICE];
	
	/// this holds the current button that has been clicked
	static int buttonState[MAX_MICE];

	/// global mouse x coords
	static int mousex[MAX_MICE]; 
	/// global mouse y coords
	static int mousey[MAX_MICE]; 

	/// mouse x delta
	static int mousedx[MAX_MICE];
	/// mouse y delta
	static int mousedy[MAX_MICE];
};

/// Mouse Listener extension class
/// \todo  derive classes that handle pos etc?
class MouseListener
{
public:
	
	// virtual callbacks for mouse handling
	virtual void		mouseMoved(int x, int y){ }
	virtual void		mouseDragged(int x, int y, int btn) {}
	virtual void		mousePressed( int x, int y, int btn ){}
	virtual void		mouseReleased( int x, int y, int btn ){}
	virtual void		mouseWheel(int delta){}
	virtual void		doubleClick( int x, int y ) {}
};



//////////////////////////////////////////////////////////////////////////
/// Mouse broadcaster 
class MouseBroadcaster
{
public:
	MouseBroadcaster()
	{
	}

	virtual ~MouseBroadcaster()
	{
	};

	

	virtual void		sendMouseMoved(int x, int y) 
	{
		for( int i = 0; i < _mouseListeners.getNumListeners(); i++ )
			_mouseListeners.getListener(i)->mouseMoved(x,y);
	}

	virtual void		sendMousePressed( int x, int y, int btn )
	{
		for( int i = 0; i < _mouseListeners.getNumListeners(); i++ )
			_mouseListeners.getListener(i)->mousePressed(x,y,btn);
	}
	virtual void		sendMouseReleased( int x, int y, int btn )
	{
		for( int i = 0; i < _mouseListeners.getNumListeners(); i++ )
			_mouseListeners.getListener(i)->mouseReleased(x,y,btn);
	}

	virtual void		sendMouseDragged( int x, int y, int btn )
	{
		for( int i = 0; i < _mouseListeners.getNumListeners(); i++ )
			_mouseListeners.getListener(i)->mouseDragged(x,y,btn);
	}

	virtual void		sendMouseWheel(int delta)
	{
		for( int i = 0; i < _mouseListeners.getNumListeners(); i++ )
			_mouseListeners.getListener(i)->mouseWheel(delta);
	}
	
	virtual void		sendDoubleClick( int x, int y )
	{
		for( int i = 0; i < _mouseListeners.getNumListeners(); i++ )
			_mouseListeners.getListener(i)->doubleClick(x,y);
	}

	void				addMouseListener( MouseListener * l ) { _mouseListeners.addListener(l); }
	EventBroadcaster<MouseListener> _mouseListeners;

};


}