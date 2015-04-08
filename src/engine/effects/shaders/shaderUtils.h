/*
 *  shaderUtils.h
 *  colormotor
 *
 *  Created by Daniel Berio on 1/23/11.
 *  Copyright 2011 http://www.enist.org. All rights reserved.
 *
 */

#pragma once

static const char * shaderUtils = STRINGIFY
(

uniform mat4 matWorld;

const vec4	lumi = vec4( 0.30, 0.59, 0.11, 0.0 );


float	luminosity( vec4 _clr )
{
	return dot(_clr,lumi);
}


vec4			getNormalColor( vec3 inn )
{
        vec3 scaleN = inn * 0.5;
        vec4 outc = vec4(0.5,0.5,0.5,1) + vec4(scaleN,0.0);
        return outc;
}


float expose( float l, float e )
{
	return (1.5 - exp(-l*e));
}


mat3 mat4to3( mat4 m )
{
	mat3 result;
	
	result[0][0] = m[0][0]; 
	result[0][1] = m[0][1]; 
	result[0][2] = m[0][2]; 

	result[1][0] = m[1][0]; 
	result[1][1] = m[1][1]; 
	result[1][2] = m[1][2]; 
	
	result[2][0] = m[2][0]; 
	result[2][1] = m[2][1]; 
	result[2][2] = m[2][2]; 
	
	return result;
}	


float attenuation( float att, float distance )
{
	return min( 1.0/(att*distance*distance), 1.0 );
}



vec3 contrastSaturationBrightness(vec3 color, float brt, float sat, float con)
{
	// Increase or decrease theese values to adjust r, g and b color channels seperately
	const float AvgLumR = 0.5;
	const float AvgLumG = 0.5;
	const float AvgLumB = 0.5;
	
	const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
	
	vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);
	vec3 brtColor = color * brt;
	vec3 intensity = vec3(dot(brtColor, LumCoeff));
	vec3 satColor = mix(intensity, brtColor, sat);
	vec3 conColor = mix(AvgLumin, satColor, con);
	return conColor;
}

);