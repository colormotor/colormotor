/*
 *  CMEffect.h
 *  resapiFFGL
 *
 *  Created by Daniel Berio on 4/5/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */

#pragma once

#include "engine/effects/Effect.h"

namespace cm
{
	class ShaderEffect : public Effect
	{
	public:
		ShaderEffect( int numInputs = 1 );
		~ShaderEffect() { release(); }
		virtual void	release();
		
		virtual bool	init( int w, int h ) { return false; }
		virtual bool	init( int w, int h, const char * vs, const char * ps, cm::Texture::FORMAT format = cm::Texture::A8R8G8B8 );
		cm::Texture * getOutput() { return rt->getTexture(); }
		
		
		
		virtual bool	apply();
		/*virtual bool	apply( CMTexture * tex ) 
		{ 
			inputs[0] = tex; 
			return apply(); 
		}*/
		
		virtual void	begin();
		virtual void	end();
		
		cm::ShaderProgram * shader;
		cm::RenderTarget * rt;
		
		bool			useStrings;
		
		// CALL ME DRAW QUAD!!!!
		void draw();
	protected:
		
	};
}
