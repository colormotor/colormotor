/*
 *  CMShaderEffect.cpp
 *  resapiTest
 *
 *  Created by Daniel Berio on 4/6/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */

#include "ShaderEffect.h"
#include "shaders/shaderUtils.h"

using namespace cm;

ShaderEffect::ShaderEffect( int numInputs )
:
Effect(numInputs,1),
rt(0),
useStrings(true),
shader(0)
{
}

void	ShaderEffect::release()
{
	SAFE_DELETE(rt);
	SAFE_DELETE(shader);
	Effect::release();
}
		
bool	ShaderEffect::init( int w, int h, const char * vs, const char * ps, cm::Texture::FORMAT format )
{	
	rt = new cm::RenderTarget();
	CM_TEST( rt->create2d(w,h,format,false) );
	
	outputs[0] = rt->getTexture();
	
	
	
	if(vs)
	{
		shader = new cm::ShaderProgram();
		if( useStrings )
		{
			CM_TEST( shader->loadString(vs,ps,shaderUtils) );
		}
		else 
		{
			CM_TEST( shader->load(vs,ps) );
		}
	}
	
	initialized = true;
	return true;
}

bool	ShaderEffect::apply()
{
	begin();
	if(shader)
		shader->setTexture("tex",inputs[0],0);
	draw();
	end();
	
	return true;
}		

void	ShaderEffect::draw()
{
	cm::Texture::drawQuad(outputs[0]);
}

void	ShaderEffect::begin()
{
	//if( inTex )
	//	inputs[0] = inTex;
		
	rt->bind();
	if(shader)
		shader->bind();
}


void	ShaderEffect::end()
{
	
	if(CM_GLERROR)
		debugPrint("glerror\n");

	if(shader)
		shader->unbind();
	
	if(CM_GLERROR)
		debugPrint("glerror\n");

	
	rt->unbind();
	
	if(CM_GLERROR)
		debugPrint("glerror\n");

}


