#pragma once


#include "engine/effects/ShaderEffect.h"


namespace cm
{

class ThresholdEffect : public ShaderEffect
{
public:

	ThresholdEffect();
	~ThresholdEffect();
	
	virtual void	release();
	bool	init( int w, int h );
	bool	apply();
	bool	apply( cm::Texture * tex );
	
	float thresh;
	
	
};

}
