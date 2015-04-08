#pragma once


#include "engine/effects/ShaderEffect.h"
#include "engine/effects/Threshold.h"
#include "engine/effects/Downsample.h"
#include "engine/effects/Accumulator.h"
#include "engine/effects/Blur.h"


namespace cm
{

class GlowEffect : public ShaderEffect
{
public:
	BlurEffect blur;
	ThresholdEffect threshold;
	DownsampleEffect downsample;
	AccumulatorEffect accumulator;
	
	int			downsampleAmt;
	
	float		multiplier;
	float		amt;
	
	float blurSize;
	float thresh;
	
	float 		fadeAmt;
	float 		mulR;
	float 		mulG;
	float 		mulB;
	float 		mulA;
	
	GlowEffect( int w = 0, int h = 0 );
	~GlowEffect();
	
	virtual void	release();
	bool	init( int w, int h );
	bool	apply();
	bool	apply( cm::Texture * tex );

};

}
