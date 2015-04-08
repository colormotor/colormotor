/*
 *  psRamp.h
 *  colormotor
 *
 *  Created by Daniel Berio on 1/23/11.
 *  Copyright 2011 http://www.enist.org. All rights reserved.
 *
 */

static const char * psRamp = STRINGIFY(

varying vec4 color;

uniform sampler2D inTex;
uniform sampler2D rampTex;

void main()
{
	gl_FragColor = texture2D( inTex, gl_TexCoord[0].st);//vec4(intens*outclr.r,intens*outclr.g,intens*outclr.b,1.0);
}


);