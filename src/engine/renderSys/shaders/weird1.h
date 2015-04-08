/*
 *  shadowVolume.h
 *  rendererTest
 *
 *  Created by ensta on 7/15/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

/////////////////////////////////////////////////

const char *vsWeird = STRINGIFY(

uniform mat4 viewMatrix;
uniform vec3 lightPos;
uniform float extrusion;
uniform vec4 diffuse;

varying vec3 l;
varying vec3 n;
varying vec4 clr;



void main()
{
	vec4 wpos = (gl_ModelViewMatrix * gl_Vertex);
	vec4 lpos = viewMatrix * vec4(lightPos,1.0);
	vec3 lightDir = wpos.xyz-lpos.xyz;

	n = gl_NormalMatrix * gl_Normal;
	l = normalize(lightDir.xyz);
	
	// if the vertex is lit
	if ( dot(lightDir, n) < 0.1 ) 
	{
		// don't move it
		gl_Position = ftransform();
	} 
	else 
	{
		vec4 fin = vec4(wpos.xyz+l*extrusion,1.0);
		fin = gl_ProjectionMatrix * fin;
		// move it far, is the light direction
				//if ( fin.z > fin.w ) // if fin is behind the far plane
		//	fin.z = fin.w; // move to the far plane (needed for z-fail algo.)
		gl_Position = fin;
	}
	
	clr = gl_Color*diffuse*1.2;
	
}
);


/*
vec4 fin = gl_ProjectionMatrix * (
					 wpos
					 + vec4(normalize(lightDir)*50.0, 0.0)
				   );

*/

/*


gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	clr = gl_Color;
	
	vec3 n = gl_NormalMatrix * gl_Normal;

	vec4 lpos = worldViewMatrix*l;
	
	// Obtain view space position
    vec4 posView = worldViewMatrix * gl_Vertex;

    // Light-to-vertex vector in view space
    vec3 lv = posView.xyz  - lpos.xyz;
	lv = normalize(lv);
	
    // Perform reverse vertex extrusion
    // Extrude the vertex away from light if it's facing away from the light.
    if( dot( n, -lv ) < 0.0 )
    {
        if( posView.z < lpos.z )
            posView.xyz -= lv * (( farPlane - posView.z ) / lpos.z);
        else
            posView = vec4( lv, 0.0 );

        // Transform the position from view space to homogeneous projection space
        gl_Position = projMatrix * posView;
    } else
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
		
*/



/////////////////////////////////////////////////

const char * psWeird = STRINGIFY(
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
uniform float exposure;
uniform float alpha;

void main( void )
{	
	vec4 c = clr; // *  max( dot( normalize(l),normalize(n) ), 0.0 ); 
	c = c*expose(luminosity(clr),exposure);
	c *= alpha;
	gl_FragColor =  c;
	
}
);