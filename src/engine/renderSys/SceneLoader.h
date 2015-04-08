/*
 *  SceneLoader.h
 *
 *  Created by Daniel Berio on 7/6/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once 

#include "engine/engineIncludes.h"
#include "engine/renderSys/Node.h"

namespace cm
{
	Node * loadScene( const char * path, bool enableTexcoords = true, bool debugNormals = false, float scale = 1.0 );
}