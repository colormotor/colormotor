/*
 *  CMEffect.h
 *  resapiFFGL
 *
 *  Created by Daniel Berio on 4/5/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */

#pragma once

#include "engine/engineIncludes.h"

namespace cm
{
#define MAX_INOUT	3


	class Effect : public Base
	{
	public:
		Effect( int numInputs, int numOutputs  ) : 
		numInputs(numInputs), numOutputs(numOutputs),
		active(true),
		initialized(false)
		{
			memset( inputs, 0, sizeof(cm::Texture*)*MAX_INOUT );
			memset( outputs, 0, sizeof(cm::Texture*)*MAX_INOUT );
			params.addBool("active",&active);
		}
		
		~Effect() { release(); }
		
		virtual void	release() 
		{
			memset( inputs, 0, sizeof(cm::Texture*)*MAX_INOUT );
			memset( outputs, 0, sizeof(cm::Texture*)*MAX_INOUT );
		} 
		
		virtual bool	init( int w, int h ) = 0;
		
		virtual bool	apply() = 0;
		
		virtual cm::Texture * getOutput() =  0;
		
		void setInput( int index, Texture * tex ) { inputs[index] = tex; }
		
		bool active;
		bool initialized;
		ParamList params;
	public:

		int		numInputs;
		int		numOutputs;
		cm::Texture * outputs[MAX_INOUT];
		cm::Texture * inputs[MAX_INOUT];
			
		
	};
	
	
}
