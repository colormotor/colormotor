/*
 *  CMBlur.cpp
 *  resapiTest
 *
 *  Created by Daniel Berio on 4/5/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */

//#define GAUSSBLUR

#include "Blur.h"
#include "shaders/psBlurH.h"
#include "shaders/psBlurV.h"
#include "shaders/vsQuad.h"



//static float weight[5] = {0.2270270270, 0.1945945946, 0.1216216216,0.0540540541, 0.0162162162 };
#define WT_0 1.0
#define WT_1 0.9
#define WT_2 0.55
#define WT_3 0.18
#define WT_4 0.1

#define WT_NORMALIZE (WT_0+2.0*(WT_1+WT_2+WT_3+WT_4))
#define KW_0 (WT_0/WT_NORMALIZE)
#define KW_1 (WT_1/WT_NORMALIZE)
#define KW_2 (WT_2/WT_NORMALIZE)
#define KW_3 (WT_3/WT_NORMALIZE)
#define KW_4 (WT_4/WT_NORMALIZE)

static float weight[5] = {KW_0, KW_1, KW_2, KW_3, KW_4 };

using namespace cm;

bool BlurEffect::init( int w, int h )
{
	release();
	
	width = w;
	height = h;

	hpass = new ShaderEffect();
	vpass = new ShaderEffect();
	
	hpass->useStrings = true;
	vpass->useStrings = true;
	CM_TEST( hpass->init( w,h, vsQuad, psBlurH ) );
	CM_TEST( vpass->init( w,h, vsQuad, psBlurV ) );
		
	return true;
}


bool BlurEffect::apply()
{	
	// mutherfuckin murphy
	// lock x and y to same value 
	if(lockXY)
		blurY = blurX;
	
	float w[7];
	for( int i = 0; i < 7; i++ )
	{
		w[i] = gaussian(i+1,gaussianAmt);
	}
	 
	
	setIdentityTransform();
	
	hpass->inputs[0] = inputs[0];
	
	hpass->begin();
	clear(0,0,0,0);
	hpass->shader->setFloat("blurSize",blurX/width);
#ifdef GAUSSBLUR
	hpass->shader->setFloat("strength",strength);
	hpass->shader->setFloatArray("weight",w,7);
#endif
	
	hpass->shader->setTexture("inTex",hpass->inputs[0],0);
	//hpass->inputs[0]->bind();
	hpass->draw();
	hpass->end();
	
	vpass->inputs[0] = hpass->outputs[0];
	 
	vpass->begin();
	clear(0,0,0,0);
	vpass->shader->setFloat("blurSize",blurY/height);
#ifdef GAUSSBLUR
	vpass->shader->setFloat("strength",strength);
	vpass->shader->setFloatArray("weight",w,7);
#endif
	
	vpass->shader->setTexture("inTex",vpass->inputs[0],0);
	vpass->draw();
	vpass->end();
	
	outputs[0] = vpass->outputs[0];//inputs[0];//vpass->outputs[0];
	 
	return true;
}


