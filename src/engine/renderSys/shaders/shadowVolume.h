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

const char *vsShadow = STRINGIFY(


uniform mat4 viewMatrix;
uniform vec3 lightPos;
uniform float extrusion;

varying vec4 clr;

void main()
{
	vec4 wpos = (gl_ModelViewMatrix * gl_Vertex);
	vec4 lpos = viewMatrix * vec4(lightPos,1.0);
	vec3 lightDir = wpos.xyz-lpos.xyz;

	// if the vertex is lit
	if ( dot(lightDir, gl_NormalMatrix * gl_Normal) < 0.00 ) 
	{
		// don't move it
		gl_Position = ftransform();
	} 
	else 
	{
		vec4 fin = vec4(wpos.xyz+normalize(lightDir.xyz)*extrusion,1.0);
		fin = gl_ProjectionMatrix * fin;
		// move it far, is the light direction
				//if ( fin.z > fin.w ) // if fin is behind the far plane
		//	fin.z = fin.w; // move to the far plane (needed for z-fail algo.)
		gl_Position = fin;
	}
}
/*

uniform vec3 lightPos;
uniform float extrusion;
uniform mat4 viewMatrix;

void main()
{
	vec4 wpos = (gl_ModelViewMatrix * gl_Vertex);
	vec4 lpos = viewMatrix * vec4(lightPos,1.0);
	vec3 lightDir = wpos.xyz-lpos.xyz;
	

	// if the vertex is lit
	if ( gl_Color.x > 0.1 )
	{
		// don't move it
		gl_Position = ftransform();
	} 
	else 
	{
		vec4 fin = vec4(wpos.xyz+lightDir.xyz*extrusion,1.0);
		fin = gl_ProjectionMatrix * fin;
		gl_Position = fin;
	}
}
*/
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

const char * psShadow = STRINGIFY(

void main( void )
{	
	// normal shading hack, gives a bit more realistic look
	gl_FragColor =  vec4(1.0,0.0,0.0,0.9);
}
);