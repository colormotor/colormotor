/*
 *  CMBlur.h
 *  resapiFFGL
 *
 *  Created by Daniel Berio on 4/5/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */

#pragma once


#include "engine/effects/ShaderEffect.h"
#include "engine/effects/Effect.h"

namespace cm
{

class BlurEffect : public Effect
{
public:
	BlurEffect( int w = 0, int h = 0 ) : Effect(1,1) 
	{ 
		blurX = blurY = 0.01f;
		lockXY = false; 
		hpass=0;vpass=0; 
	 
		// todo relative to size
		params.addFloat("blurX",&blurX,0,100);
		params.addFloat("blurY",&blurY,0,100);
		
		gaussianAmt = 1.0;
		strength = 1.0;
		params.addFloat("gauss",&gaussianAmt,0.1,500);
		
		params.addFloat("strength",&strength,0.01,20.0);
		
		if(w)
			init(w,h);
		
	}
	~BlurEffect() { release(); }
	virtual void	release() 
	{
		SAFE_DELETE(hpass);
		SAFE_DELETE(vpass);
		
		Effect::release();
	} 
	
	bool	init( int w, int h );
	bool	apply();
	bool	apply( cm::Texture * tex ) 
	{ 
		inputs[0] = tex; 
		return apply(); 
	}
	
	cm::Texture * getOutput() { return outputs[0]; }
	
	float		gaussianAmt;
	float		blurX;
	float		blurY;
	float		strength;
	bool		lockXY;
	float 		width;
	float 		height;
	
	ShaderEffect * hpass;
	ShaderEffect * vpass;
	
	
};

}
