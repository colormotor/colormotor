/*
 *  diffusePass.h
 *  rendererTest
 *
 *  Created by ensta on 7/16/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

const char *vsAmbientDiffuse = STRINGIFY(

varying vec3 l;
varying vec3 n;
varying vec4 clr;
varying vec3 pos;

uniform vec3 lightPos;
uniform mat4 viewMatrix;

void main()
{
	vec3 wpos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	n = gl_NormalMatrix * gl_Normal;
	clr = gl_Color;
	vec4 lpos = viewMatrix * vec4(lightPos,1.0);
	l = lpos.xyz-wpos;
	pos = wpos;
}
);

const char * psAmbientDiffuse = STRINGIFY(

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

float attenuation( float att, float distance )
{
	return min( 1.0/(att*distance*distance), 1.0 );
}


varying vec3 l;
varying vec3 n;
varying vec4 clr;
varying vec3 pos;

uniform float exposure;
uniform float contrast;
uniform vec4 diffuse;
uniform vec4 ambient;
uniform float ambientAmt;
uniform float diffuseMin;
uniform float fog;

void main( void )
{	
	vec4 c = clr*diffuse*contrast;
	vec3 nn = normalize(n);
	vec3 ll = normalize(l);
	
	float nl = max( dot( nn,ll ), diffuseMin ) + max( dot(-nn,ll), diffuseMin);
	nl = min(nl,1.0);
	
	vec4 a = ambient * diffuse * luminosity( getNormalColor( nn ) ) * ambientAmt;
	vec4 d = c * nl;
	c = a+d; 
	c = c*expose(luminosity(c),exposure);
	c.w = 1.0;
	//c*=attenuation(fog,length(pos));
	gl_FragColor =  c;
	
}

);

//+ c

//// clr *  max( dot( normalize(l),normalize(n) ), 0.1 )
