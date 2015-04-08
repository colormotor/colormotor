/*
 *  CMAccumulator.cpp
 *
 *  Created by Daniel Berio on 1/23/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#include "Threshold.h"
#include "shaders/psThresh.h"
#include "shaders/vsQuad.h"
 
using namespace cm;

ThresholdEffect::ThresholdEffect()
{ 
	thresh = 0.5;
	params.addFloat("thresh",&thresh,0,1);
}

ThresholdEffect::~ThresholdEffect() { release(); }
void	ThresholdEffect::release() 
{
	ShaderEffect::release();
} 

bool	ThresholdEffect::init( int w, int h )
{	
	return ShaderEffect::init( w, h, vsQuad, psThresh );
}

bool	ThresholdEffect::apply()
{
	begin();
	clear(0,0,0,0);
	shader->setFloat("thresh",thresh);
	shader->setTexture("inTex",inputs[0],0);
	Texture::drawQuad(inputs[0]);
	
	end();
	
	return true;
}		


bool	ThresholdEffect::apply( cm::Texture * tex ) 
{ 
	inputs[0] = tex; 
	return apply(); 
}

