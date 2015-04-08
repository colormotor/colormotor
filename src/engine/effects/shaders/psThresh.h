/*
 *  psThresh.h
 *  colormotor
 *
 *  Created by Daniel Berio on 1/23/11.
 *  Copyright 2011 http://www.enist.org. All rights reserved.
 *
 */

static const char * psThresh = STRINGIFY(

varying vec4 color;

uniform sampler2D inTex;

uniform float thresh;

void main()
{
   vec2 texcoord = vec2(gl_TexCoord[0]);
   vec4 clr = texture2D(inTex,texcoord);
   float lum = luminosity(clr);
   vec4 outClr = vec4(0,0,0,0);
   if(lum>thresh)
		outClr=clr;
	gl_FragColor = outClr;
}

);