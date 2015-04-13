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

#include "cmGfxAppModule.h"

namespace cm
{

// Can call these seperately to setup app and begin, so if custom GL initialization is necessary it can be done in between
bool setupGlfwApp( GfxAppModule * app, int w, int h, bool fullscreen, const char * title );
bool beginGlfwLoop( GfxAppModule * app );

// or call this one in one go
bool runGlfwApp( GfxAppModule * app, int w, int h, bool fullscreen = false, const char * title = "colormotor" );

}