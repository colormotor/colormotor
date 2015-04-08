#pragma once


#include "engine/effects/ShaderEffect.h"


namespace cm
{

class DownsampleEffect : public ShaderEffect
{
public:

	DownsampleEffect();
	~DownsampleEffect();
	
	virtual void	release();
	bool	init( int w, int h, int divw, int divh );
	bool	apply();
	bool	apply( cm::Texture * tex );
	
	float thresh;
	
	
};

}
