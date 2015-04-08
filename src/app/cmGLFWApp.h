/*
 *  CMGLFWApp.h
 *
 *  Created by Daniel Berio on 8/28/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

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