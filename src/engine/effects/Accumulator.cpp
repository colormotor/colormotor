/*
 *  CMAccumulator.cpp
 *
 *  Created by Daniel Berio on 1/23/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#include "Accumulator.h"
#include "shaders/psAccum.h"
#include "shaders/vsQuad.h"

using namespace cm;

AccumulatorEffect::AccumulatorEffect(  int w, int h)
:
bClear(false)
{ 

	
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
	rotation = 0.0f;
	scale = 1.0f;
	params.addFloat("rotation",&rotation,-3,3);
	params.addFloat("scale",&scale,0.8,1.2);
	
	bFlipOrder = true;
	params.addBool("flip order",&bFlipOrder);
	
	for( int i = 0; i < 2; i++ )
		accum[i] = 0;
		
	cur = 0;

	if( w )
		init(w,h);
}
AccumulatorEffect::~AccumulatorEffect() { release(); }

void	AccumulatorEffect::release() 
{
	for( int i = 0; i < 2; i++ )
	{
		if(accum[i])
			delete accum[i];
		accum[i] = 0;
	}
	rt = 0;
	ShaderEffect::release();
} 


bool	AccumulatorEffect::init( int w, int h )
{
	for( int i = 0; i < 2; i++ )
	 {
		accum[i] = new RenderTarget();
		CM_TEST( accum[i]->create2d( w,h,Texture::A16B16G16R16F,false) );
	 }
	 
	 cur = 0;
	
	
	return ShaderEffect::init( w, h, vsQuad, psAccum );
}

void	AccumulatorEffect::clear()
{
	bClear = true;
}

void	AccumulatorEffect::renderHistory()
{
	float fade = fadeAmt;
	if( bClear ) 
		fade = 0;
	bClear = false;
	
	//gfx::color(fadeAmt,fadeAmt,fadeAmt,fadeAmt);
	shader->setFloat("mulR",fade);
	shader->setFloat("mulG",fade);
	shader->setFloat("mulB",fade);
	shader->setFloat("mulA",fade);
	
	gfx::pushMatrix();
	gfx::rotate(rotation);
	gfx::scale(scale);
	accum[!cur]->getTexture()->draw();
	gfx::popMatrix();
	gfx::color(1,1,1,1);
	
}

void	AccumulatorEffect::renderNew()
{
	//gfx::color(mulR,mulG,mulB,mulA);
	shader->setFloat("mulR",mulR);
	shader->setFloat("mulG",mulG);
	shader->setFloat("mulB",mulB);
	shader->setFloat("mulA",mulA);
	
	inputs[0]->draw();
	
}

bool	AccumulatorEffect::apply()
{

	
	rt = accum[cur];
		
	rt->bind();

	shader->bind();
	gfx::clear(0,0,0,0);
	/*
	shader->setFloat("fadeAmt",fadeAmt);
	shader->setFloat("mulR",mulR);
	shader->setFloat("mulG",mulG);
	shader->setFloat("mulB",mulB);
	shader->setFloat("mulA",mulA);
	
	shader->setTexture("accumTex",accum[!cur]->getTexture(),0);
	shader->setTexture("inTex",inputs[0],1);
*/

	setIdentityTransform();
	setBlendMode(BLENDMODE_ALPHA_PREMULTIPLIED);
	
	if( bFlipOrder )
	{
		renderNew();
		renderHistory();
	}
	else 
	{
		renderHistory();
		renderNew();
	}

	
		//Texture::drawQuad(accum[!cur]->getTexture());
	//Texture::drawQuad2(accum[!cur]->getTexture(),inputs[0]);
	
	cur = !cur;
	
	shader->unbind();
	rt->unbind();
	
	enableBlend(false);
	return true;
}		


bool	AccumulatorEffect::apply( cm::Texture * tex ) 
{ 
	inputs[0] = tex; 
	return apply(); 
}

