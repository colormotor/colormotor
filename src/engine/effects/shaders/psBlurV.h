

#pragma once

#ifdef GAUSSBLUR

static const char * psBlurV = STRINGIFY(
										
varying vec4 color;

uniform sampler2D inTex;

uniform float blurSize;
uniform float strength;

uniform float weight[7];

void main(void)
{
	vec4 sum = vec4(0.0);
	vec2 texcoord = vec2(gl_TexCoord[0]);
	
	sum += texture2D(inTex, vec2(texcoord.x, texcoord.y)) * weight[0] * strength;
	
	float mult = 1.0;
	for( int i = 1; i < 7; i++ )
	{
		sum += texture2D(inTex, vec2(texcoord.x, texcoord.y - mult*blurSize)) * weight[i] * strength;
		sum += texture2D(inTex, vec2(texcoord.x, texcoord.y + mult*blurSize)) * weight[i] * strength;
		mult+=1.0;
	}
	
	gl_FragColor = sum;
}
										
										);

#else

static const char * psBlurV = STRINGIFY(
										
										varying vec4 color;
										
										uniform sampler2D inTex;
										
										uniform float blurSize;
										
										void main(void)
{
	
	vec4 sum = vec4(0.0);
	vec2 texcoord = vec2(gl_TexCoord[0]);
	
	sum += texture2D(inTex, vec2(texcoord.x,texcoord.y - 4.0*blurSize)) * 0.05;
	sum += texture2D(inTex, vec2(texcoord.x,texcoord.y - 3.0*blurSize)) * 0.09;
	sum += texture2D(inTex, vec2(texcoord.x,texcoord.y - 2.0*blurSize)) * 0.12;
	sum += texture2D(inTex, vec2(texcoord.x,texcoord.y - blurSize)) * 0.15;
	sum += texture2D(inTex, vec2(texcoord.x,texcoord.y)) * 0.16;
	sum += texture2D(inTex, vec2(texcoord.x,texcoord.y + blurSize)) * 0.15;
	sum += texture2D(inTex, vec2(texcoord.x,texcoord.y + 2.0*blurSize)) * 0.12;
	sum += texture2D(inTex, vec2(texcoord.x,texcoord.y + 3.0*blurSize)) * 0.09;
	sum += texture2D(inTex, vec2(texcoord.x,texcoord.y + 4.0*blurSize)) * 0.05;
	
	gl_FragColor = sum;
}
										
										);


#endif

