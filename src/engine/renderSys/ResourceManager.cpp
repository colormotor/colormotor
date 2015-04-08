/*
 *  ResourceManager.cpp
 *
 *  Created by Daniel Berio on 8/12/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */


#include "ResourceManager.h"

namespace cm
{

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	release();
}
	

void ResourceManager::release()
{
	meshes.clear();
	textures.clear();
	shaders.clear();
}

void ResourceManager::addMesh( const char * name, Mesh * mesh )
{
	if(getMesh(name))
	{
		debugPrint("resource %s allready exists\n",name);
		return;
	}
	
	meshes[name] = mesh;
}

bool ResourceManager::loadShader( const char * name, const char * vs, const char * ps )
{
	if(getShader(name))
	{
		debugPrint("resource %s allready exists\n",name);
		return false;
	}
	
	ShaderProgram * r = new ShaderProgram();
	if(!r->loadString(vs,ps))
	{
		return false;
	}
	
	shaders[name] = r;
	
	return true;
}

bool ResourceManager::loadTexture( const char * name, const char * path, int mipLevels  )
{
	if(getTexture(name))
	{
		debugPrint("resource %s allready exists\n",name);
		return false;
	}
	
	Texture * r = new Texture();
	if(!r->load(path,mipLevels))
		return false;
	textures[name] = r;
	return true;
}


ShaderProgram * ResourceManager::getShader( const char * name )
{
	ShaderMap::iterator it = shaders.find(name);
	if(it!=shaders.end())
		return it->second;
	return 0;
}

Texture * ResourceManager::getTexture( const char * name )
{
	TextureMap::iterator it = textures.find(name);
	if(it!=textures.end())
		return it->second;
	return 0;
}

Mesh * ResourceManager::getMesh( const char * name )
{
	MeshMap::iterator it = meshes.find(name);
	if(it!=meshes.end())
		return it->second;
	return 0;
}

CM_SINGLETON_IMPL(ResourceManager)

}