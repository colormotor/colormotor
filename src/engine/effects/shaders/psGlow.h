#pragma once

static const char * psGlow = STRINGIFY(

varying vec4 color;

uniform sampler2D inTex;


uniform float amt;

void main()
{
	vec4 sum = texture2D( inTex, gl_TexCoord[0].st)*amt;
	gl_FragColor = sum;
}


);