/*
 *  DefaultMaterial.cpp
 *
 *  Created by Daniel Berio on 7/10/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */


#include "DefaultMaterial.h"


const char *vs = STRINGIFY(

varying vec3 l;
varying vec3 n;
varying vec4 clr;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	clr = gl_Color;
	n = gl_Normal;
}
);

const char * ps = STRINGIFY(


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

varying vec3 n;
varying vec4 clr;
uniform vec4 diffuse;
uniform vec4 ambient;

void main( void )
{	
	// normal shading hack, gives a bit more realistic look
	gl_FragColor =  clr*ambient+clr*diffuse*max(luminosity( getNormalColor( normalize(n) ) ),0.1); 
}
);

namespace cm
{


DefaultMaterial::DefaultMaterial()
{
}


void DefaultMaterial::release()
{
	shader.release();
	Material::release();
}

bool DefaultMaterial::init()
{
	return shader.loadString( vs, ps );
}

///////////////////////////////////////////////////////////////////////////

void DefaultMaterial::bindTexture( int index )
{
	textures[index].texture->bind( textures[index].sampler );
}


///////////////////////////////////////////////////////////////////////////

void DefaultMaterial::begin()
{
	shader.bind();
	shader.setVec4( "diffuse", diffuseColor );
	shader.setVec4( "ambient", ambientColor );
	Material::begin();
}

void DefaultMaterial::end()
{
	Material::end();
	shader.unbind();
}


}