/*
 *  diffusePass.h
 *  rendererTest
 *
 *  Created by ensta on 7/16/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

const char *vsDiffuse = STRINGIFY(

varying vec3 l;
varying vec3 n;
varying vec4 clr;
varying vec2 uv;

uniform vec3 lightPos;
uniform vec4 diffuse;
uniform mat4 viewMatrix;

void main()
{
	vec3 wpos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	n = gl_NormalMatrix * gl_Normal;
	clr = gl_Color*diffuse*1.2;
	vec4 lpos = viewMatrix * vec4(lightPos,1.0);
	l = lpos.xyz-wpos;
}
);

const char * psDiffuse = STRINGIFY(

float expose( float l, float e )
{
	return (1.5 - exp(-l*e));
}

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

varying vec3 l;
varying vec3 n;
varying vec4 clr;
varying vec2 uv;

uniform sampler2D tex;
uniform float exposure;

void main( void )
{	
	vec4 c = clr *  max( dot( normalize(l),normalize(n) ), 0.0 ); 
	c*=texture2D(tex,uv);
	c = c*expose(luminosity(clr),exposure);
	gl_FragColor =  c;
	
}

);

//// clr *  max( dot( normalize(l),normalize(n) ), 0.1 )
