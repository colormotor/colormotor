//////////////////////////////////////////////////////////////////////////                                                     
//	 _________  __   ____  ___  __  _______  __________  ___			
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \			
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/			
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.		
//																		
//	� Daniel Berio 2008													
//	  http://www.enist.org/												
//																	
//																		
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



#include "Material.h"

namespace cm
{


Material::Material()
{
	diffuseColor(1,1,1,1);
	specularColor(1,1,1,1);
	ambientColor(0.1,0.1,0.1,1);
	hasAmbient = true;
	hasDiffuse = true;
	hasSpecular = true;
	//_shader = new Shader();
}


void Material::release()
{
	for( int i = 0 ; i < getNumTextures(); i++ )
	{
		if(textures[i].local)
			delete textures[i].texture;
	}
	
	textures.clear();
}


///////////////////////////////////////////////////////////////////////////

bool Material::reloadTexture( int index, const char * filename )
{
	MtlTextureDesc & mt = textures[index];
	return mt.texture->load(filename,0);
}

///////////////////////////////////////////////////////////////////////////

bool Material::addTexture( const char * filename, int sampler, TEXTURE_SEMANTIC semantic, int channel)
{
	MtlTextureDesc mt;
	mt.texture = new Texture();
	mt.texture->load(filename,0);
	mt.texture->bind();
	mt.texture->setWrap(Texture::REPEAT);
	mt.texture->unbind();
	mt.local = true;
	if( mt.texture==0 )
		return false;

	if(sampler>-1)
		mt.sampler = sampler;
	else
		mt.sampler = textures.size(); // consecutive samplers default

	mt.semantic = semantic;
	mt.channel = 0;

	textures.push_back(mt);
	return true;
}

///////////////////////////////////////////////////////////////////////////

bool Material::addTexture( Texture * tex, int sampler, TEXTURE_SEMANTIC semantic, int channel)
{
	MtlTextureDesc mt;
	mt.texture = tex;
	mt.local = false;

	if(sampler>-1)
		mt.sampler = sampler;
	else
		mt.sampler = textures.size(); // consecutive samplers default

	mt.semantic = semantic;
	mt.channel = 0;

	textures.push_back(mt);
	return true;
}

///////////////////////////////////////////////////////////////////////////

void Material::bindTexture( int index )
{
CM_GLCONTEXT
	textures[index].texture->bind( textures[index].sampler );
	
}


///////////////////////////////////////////////////////////////////////////

void Material::begin()
{
	// set textures
	for( int i = 0; i < getNumTextures(); i++ )
	{
		if(textures[i].texture)
			bindTexture(i);
	} 
}

void Material::end()
{
	// set textures
	for( int i = 0; i < getNumTextures(); i++ )
	{
		if(textures[i].texture)
			textures[i].texture->unbind();
	} 
}


}