/*
 *  CMAccumulator.cpp
 *
 *  Created by Daniel Berio on 1/23/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#include "Downsample.h"

using namespace cm;

DownsampleEffect::DownsampleEffect()
{ 
}

DownsampleEffect::~DownsampleEffect() { release(); }
void	DownsampleEffect::release() 
{
	ShaderEffect::release();
} 

bool	DownsampleEffect::init( int w, int h, int divw, int divh  )
{	
	return ShaderEffect::init( w/divw, h/divh, 0, 0 );
}

bool	DownsampleEffect::apply()
{
	begin();
	clear(0,0,0,0);
	inputs[0]->draw();
	end();	
	return true;
}		


bool	DownsampleEffect::apply( cm::Texture * tex ) 
{ 
	inputs[0] = tex; 
	return apply(); 
}

