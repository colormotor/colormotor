/*
 *  psAccum.h
 *  colormotor
 *
 *  Created by Daniel Berio on 1/23/11.
 *  Copyright 2011 http://www.enist.org. All rights reserved.
 *
 */

#pragma once

static const char * psAccum = STRINGIFY(

varying vec4 color;

uniform sampler2D inTex;
uniform sampler2D accumTex;

uniform float mulR;
uniform float mulG;
uniform float mulB;
uniform float mulA;

uniform float fadeAmt;

void main()
{
	vec4 sum = texture2D( inTex, gl_TexCoord[0].st)*vec4(mulR,mulG,mulB,mulA);//+texture2D( accumTex, gl_TexCoord[0].st)*fadeAmt;
	gl_FragColor = sum;//sum;//min( sum, vec4(1,1,1,1) );
}

);