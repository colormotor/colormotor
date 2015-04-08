/*
 *  Scene.h
 *
 *  Created by Daniel Berio on 7/10/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once 
#include "engine/renderSys/Node.h"
#include "engine/renderSys/Animation.h"

namespace cm
{	
	class Scene : public Node
	{
		public:
			enum
			{
				FORCE_FACE_NORMALS = 1,
				DEBUG_CLR_NORMALS = 2,
				DISABLE_TEXCOORDS = 4
			};
			
			Scene();
			~Scene() { release(); }
			
			virtual void release();
			
			bool load( const char * path, unsigned int flags = 0 );
			
			int getNumMeshes() const { return meshes.size(); }
			int getNumTextures() const { return textures.size(); }
			int getNumMaterials() const { return materials.size(); }
			
			RenderableNode * createMeshNode( int meshIndex );
			
			void setShadowMesh( int index, Mesh * mesh ) { shadowMeshes[index] = mesh; }
			Mesh * getShadowMesh( int index ) { return shadowMeshes[index]; }
			Mesh * getMesh( int index ) { return meshes[index]; }
			Texture * getTeture( int index ) { return textures[index]; }
			Material * getMaterial( int index ) { return materials[index]; }
			
			void addMesh( Mesh * o ) { meshes.add(o); shadowMeshes.push_back(0); }
			void addTexture( Texture * o ) { textures.add(o); }
			void addMaterial( Material * o ) { materials.add(o); }
			
			int getNumAnimations() const { return anims.size(); }
			SceneAnimation * getAnimation( int index) const { return anims[index]; }
			void addAnimation( SceneAnimation * anim ) { anims.push_back(anim); }
			
			Node * getRoot() const { return root; }
		protected:
			Node * root;
			
			std::vector < RefPtr<Mesh> >  shadowMeshes;
			
			Group <Mesh>  meshes;
			Group <Texture>  textures;
			Group <Material> materials;
			
			std::vector <SceneAnimation *> anims;
	};

	TriMesh loadTriMesh( const std::string & path, float maxSmoothingAngle = 40.0f, bool bNormalDebug = false );
};