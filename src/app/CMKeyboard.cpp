/*********************************************************************
 *                                                                   *
 *   |     |     |  |  |     | __  |     |     |_   _|     | __  |   *
 *   |   --|  |  |  |__|  |  |    -| | | |  |  | | | |  |  |    -|   *
 *   |_____|_____|_____|_____|__|__|_|_|_|_____| |_| |_____|__|__|   *
 *                                                                   *
 *********************************************************************
 *   COLORMOTOR 3d & multimedia framework
 *	 #version: alpha
 *	 Copyright (C) Daniel Berio
 *   http://www.enist.org
 *   drand48@gmail.com
 *   
 *   Use at your own risk
 *   Please contact me if you use this library for commercial purposes
 *
 ********************************************************************/

#include "CM.h"
#include "cmKeyboard.h"


namespace cm
{

bool	Keyboard::_initialized = false;
bool	Keyboard::_keyPressed[NUM_KEYS];
bool	Keyboard::_oldKeyPressed[NUM_KEYS];

}