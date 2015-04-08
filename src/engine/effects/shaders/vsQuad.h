/*
 *  vs_quad.h
 *  resapiFFGL
 *
 *  Created by Daniel Berio on 4/5/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */

#pragma once


static const char * vsQuad = STRINGIFY(

varying vec4 color;


void main()
{	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position  = gl_ModelViewProjectionMatrix * gl_Vertex;
	color = gl_Color;
}

);