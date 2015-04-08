/*
 *  cmGui.h
 *  colormotor
 *
 *  Created by ensta on 8/28/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "CMMouse.h"
#include "CMKeyboard.h"

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
