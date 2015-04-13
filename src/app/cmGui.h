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

#include "cmMouse.h"
#include "cmKeyboard.h"

namespace cm
{

class Gui : public MouseListener, public KeyboardListener
{
public:
	Gui()
	:
	visible(true)
	{
	}
	
	virtual ~Gui() 
	{}
	
	virtual void release() { }
	virtual void resize( float x, float y ) = 0;
	virtual bool hasFocus() const = 0;
	
	bool visible;
};

}
