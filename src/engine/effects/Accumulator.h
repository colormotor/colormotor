#pragma once


#include "engine/effects/ShaderEffect.h"


namespace cm
{

class AccumulatorEffect : public ShaderEffect
{
public:
	float 		fadeAmt;
	float 		mulR;
	float 		mulG;
	float 		mulB;
	float 		mulA;
	float		rotation;
	float		scale;
	
	RenderTarget	* accum[2];
	int				cur;
	
	
	AccumulatorEffect( int w = 0, int h = 0);
	~AccumulatorEffect();
	
	virtual void	release();
	bool	init( int w, int h );
	bool	apply();
	bool	apply( cm::Texture * tex );
	
	void renderHistory();
	void renderNew();
	
	void clear();
	bool bClear;
	bool bFlipOrder;
	float thresh;
	
	
};

}
