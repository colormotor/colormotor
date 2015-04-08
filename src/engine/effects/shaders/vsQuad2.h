/// Quad vertex shader with two uvs
#pragma once


static const char * vsQuad2 = STRINGIFY(

varying vec4 color;


void main()
{	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord0;
	gl_Position  = gl_Vertex;
	color = gl_Color;
}

);