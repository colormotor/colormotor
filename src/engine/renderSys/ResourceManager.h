/*
 *  ResourceManager.h
 *
 *  Created by Daniel Berio on 8/12/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once

#include "engine/renderSys/Mesh.h"

namespace cm
{

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	
	void release();
	
	void addMesh( const char * name, Mesh * mesh );
	bool loadShader( const char * name, const char * vs, const char * ps );
	bool loadTexture( const char * name, const char * path, int mipLevels = 1 );
	
	ShaderProgram * getShader( const char * name );
	Texture * getTexture( const char * name );
	Mesh * getMesh( const char * name );
	
	CM_SINGLETON(ResourceManager)
	
private:
	typedef std::map <std::string,RefPtr<Texture> > TextureMap;
	TextureMap textures;
	typedef std::map <std::string,RefPtr<ShaderProgram> > ShaderMap;
	ShaderMap shaders;
	typedef std::map <std::string,RefPtr<Mesh> > MeshMap;
	MeshMap meshes;
	
};

}