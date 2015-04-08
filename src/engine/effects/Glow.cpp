/*
 *  CMGlow.cpp
 *
 *  Created by Daniel Berio on 1/23/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#include "gfx/CMGLInternal.h"
#include "Glow.h"
#include "shaders/psGlow.h"
#include "shaders/vsQuad.h"

using namespace cm;

GlowEffect::GlowEffect( int w, int h )
{ 
	amt = 1;
	params.addFloat("amt",&amt,0,1);
	
	blurSize = 0.005;
	params.addFloat("blurSize",&blurSize,0.0001,0.01);
	
	thresh = 0.4;
	params.addFloat("thresh",&thresh,0,1);
	
	fadeAmt = 0.78f;
	params.addFloat("fadeAmt",&fadeAmt,0,1);
	
	mulR = 0.6f;
	params.addFloat("mulR",&mulR,0,1);
	mulG = 0.6f;
	params.addFloat("mulG",&mulG,0,1);
	mulB = 0.6f;
	params.addFloat("mulB",&mulB,0,1);
	mulA = 0.6f;
	params.addFloat("mulA",&mulA,0,1);
	
	downsampleAmt = 1;
	
	multiplier = 1;

	if ( w )
		init(w,h);
}
GlowEffect::~GlowEffect() { release(); }
void	GlowEffect::release() 
{
	blur.release();
	threshold.release();
	downsample.release();
	accumulator.release();	
	ShaderEffect::release();
} 

bool	GlowEffect::init( int w, int h )
{
	CM_TEST( blur.init(w/downsampleAmt,h/downsampleAmt) );
	CM_TEST( threshold.init(w/downsampleAmt,h/downsampleAmt) );
	CM_TEST( downsample.init(w,h,1,1) );
	CM_TEST( accumulator.init(w,h) );
	return ShaderEffect::init( w, h, vsQuad, psGlow );
}

bool	GlowEffect::apply()
{
	CM_GLCONTEXT
	
	blur.blurX = blurSize;
	blur.blurY = blurSize;
	
	threshold.thresh = thresh;
	
	accumulator.fadeAmt = cm::min( fadeAmt*multiplier, 1.0f );
	accumulator.mulR = mulR;
	accumulator.mulG = mulG;
	accumulator.mulB = mulB;
	accumulator.mulA = mulA;
	
	Texture * t = inputs[0];
	
	if( thresh > 0 )
	{
		threshold.apply(inputs[0]);
		t = threshold.getOutput();
	}
	
	//downsample.apply(threshold.getOutput());*/
	blur.apply(t);
	accumulator.apply(blur.getOutput());
	
	rt->bind();
	clear(0,0,0,0);
	
	shader->bind();
	
	
	enableBlend(false);
	shader->setFloat("amt",amt);
	
	shader->setTexture("inTex",accumulator.getOutput(),0);
	
	glColor4f(amt,amt,amt,amt);
	Texture::drawQuad(inputs[0]);
	
	shader->unbind();
//	accumulator.getOutput()->draw();
	rt->unbind();
	return true;
}		


bool	GlowEffect::apply( cm::Texture * tex ) 
{ 
	inputs[0] = tex; 
	return apply(); 
}

