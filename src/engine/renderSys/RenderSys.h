/*
 *  RenderSys.h
 *
 *  Created by Daniel Berio on 7/5/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once

#include "engine/renderSys/Mesh.h"
#include "engine/renderSys/Node.h"
#include "engine/renderSys/Material.h"
#include "engine/renderSys/Renderable.h"
#include "engine/renderSys/RenderChunk.h"
#include "engine/renderSys/SceneLoader.h"
#include "engine/renderSys/Camera.h"
#include "engine/renderSys/Scene.h"
#include "engine/renderSys/Renderer.h"
#include "engine/effects/Glow.h"
#include "engine/renderSys/ResourceManager.h"
#include "engine/renderSys/TextureLines.h"
#include "engine/renderSys/Skeleton.h"
#include "engine/renderSys/SkeletonAnimator.h"
#include "engine/renderSys/AnimCache.h"
#include "engine/renderSys/SkeletonAnimSystem.h"

namespace cm
{
	void registerDefaultClasses();
}
using namespace cm;
