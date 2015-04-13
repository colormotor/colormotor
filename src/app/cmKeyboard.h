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
#include "cmEvents.h"


namespace cm {

//////////////////////////////////////////////////////////////////////////

/// Keyboard enumeration
/// \todo missing keys !? etc
enum KEY
{
	KEY_UNKNOWN = -1,
	KEY_BACKSPACE = 128,
	KEY_TAB,
	KEY_ENTER,
	KEY_PAUSE,
	KEY_SCROLL_LOCK,
	KEY_ESCAPE,
	KEY_HOME,
	
	KEY_LEFT,
	KEY_UP,
	KEY_RIGHT,
	KEY_DOWN,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,
	KEY_END,
	KEY_PRINT,
	KEY_INSERT,
	KEY_MENU,
	KEY_HELP,
	KEY_NUM_LOCK,

	KEY_SHIFT_L,
	KEY_SHIFT_R,
	KEY_CTRL_L,
	KEY_CTRL_R,

	KEY_CAPS_LOCK,
	KEY_ALT_L,
	KEY_ALT_R,
	KEY_DELETE,
	NUM_KEYS
};

//////////////////////////////////////////////////////////////////////////

/// Keybard state class ( static )
class  Keyboard
{
public:
	static bool	isKeyPressed( int key ) { if( !_initialized) init(); if( key == KEY_UNKNOWN ) return false; return _keyPressed[key]; }
	static bool	wasKeyPressed( int key ) { if( !_initialized) init(); if( key == KEY_UNKNOWN ) return false; return _oldKeyPressed[key]; }

	static bool isCtrlDown() { return isKeyPressed( KEY_CTRL_L ) || isKeyPressed( KEY_CTRL_R); }
	static bool isShiftDown() { return isKeyPressed( KEY_SHIFT_L ) || isKeyPressed( KEY_SHIFT_R); }
	static bool isAltDown() { return isKeyPressed( KEY_ALT_L ) || isKeyPressed( KEY_ALT_R); }
	
	/// should not call these functions find a way to block user?
	/// cant set any class as friend because these will be probably set by some 
	/// platform specific event handling func ( eg. windows proc )
	static bool	setKeyPressed( int key, bool val = true ) 
	{
		if( !_initialized) init();

		if( key == KEY_UNKNOWN ) return false; 

		_oldKeyPressed[key] = _keyPressed[key];
		_keyPressed[key] = val;

		return true;
	}

	static void init()
	{
		for( int i = 0; i < NUM_KEYS; i++ ) 
		{
			_keyPressed[i] = _oldKeyPressed[i] = false;
		}
		_initialized = true;
	}
	
	static bool isBackspace( int c )
	{
		return c == 127 || c == 8;
	}


protected:
	static bool	_keyPressed[NUM_KEYS];
	static bool	_oldKeyPressed[NUM_KEYS];
	static bool	_initialized;
};


//////////////////////////////////////////////////////////////////////////

/// Keyboard listener extension class
class KeyboardListener
{
public:
	
	///   Handle a key press
	/// \param c ascii code of key pressed
	virtual void		ascii(int c) { }

	///   Handle key down event
	/// \param k Key identifier 
	virtual void		keyPressed( int k ) {  }

	///   Handle key up event
	/// \param k Key identifier 
	virtual void		keyReleased( int k ) { }

};

//////////////////////////////////////////////////////////////////////////
/// Keyboard event broadcaster
class  KeyboardBroadcaster
{
public:
	KeyboardBroadcaster()
	{

	}

	virtual ~KeyboardBroadcaster()
	{
	};

	

	virtual void		sendAscii(char c) 
	{
		for( int i = 0; i < _keyboardListeners.getNumListeners(); i++ )
			_keyboardListeners.getListener(i)->ascii(c);
	}

	virtual void		sendKeyPressed(int k) 
	{
		for( int i = 0; i < _keyboardListeners.getNumListeners(); i++ )
			_keyboardListeners.getListener(i)->keyPressed(k);
	}

	virtual void		sendKeyReleased(int k) 
	{
		for( int i = 0; i < _keyboardListeners.getNumListeners(); i++ )
			_keyboardListeners.getListener(i)->keyReleased(k);
	}


	void				addKeyboardListener( KeyboardListener * l ) { _keyboardListeners.addListener(l); }

	EventBroadcaster<KeyboardListener> _keyboardListeners;
};

/// @}
}