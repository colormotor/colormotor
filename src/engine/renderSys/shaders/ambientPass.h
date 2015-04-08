/*
 *  ambientPass.h
 *  rendererTest
 *
 *  Created by ensta on 7/16/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

const char *vsAmbient = STRINGIFY(

uniform vec4 diffuse;

varying vec3 n;
varying vec4 clr;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	clr = gl_Color*diffuse;
	n = gl_Normal;
}
);

const char * psAmbient = STRINGIFY(

const vec4 lumi = vec4( 0.30, 0.59, 0.11, 0.0 );
float luminosity( vec4 clr_ )
{
	return dot(clr_,lumi);
}

vec4			getNormalColor( vec3 inn )
{
        vec3 scaleN = inn * 0.5;
        vec4 outc = vec4(0.5,0.5,0.5,1) + vec4(scaleN,0.0);
        return outc;
}

uniform vec4 ambient;
uniform float ambientAmt;

varying vec3 n;
varying vec4 clr;

void main( void )
{	
	// normal shading hack, gives a bit more realistic look
	gl_FragColor =  (ambient*ambientAmt) + clr * max(luminosity( getNormalColor( normalize(n) ) ),0.0)*ambientAmt;;
}


);


// + clr * max(luminosity( getNormalColor( normalize(n) ) ),0.1)*0.4; 