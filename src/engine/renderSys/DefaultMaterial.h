/*
 *  DefaultMaterial.h
 *
 *  Created by Daniel Berio on 7/10/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once

#include "engine/renderSys/Material.h"

namespace cm
{

class DefaultMaterial : public Material
{
public:
	RTTIOBJECT( "DefaultMaterial", DefaultMaterial, Material )
	
	DefaultMaterial();
	~DefaultMaterial() { release(); }
	
	virtual void release();
	
	bool init();
	void begin();
	void end();
	void bindTexture( int index );
	
	ShaderProgram shader;
};

}