/*
 *  SceneLoader.cpp
 *
 *  Created by Daniel Berio on 7/6/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */


#include "SceneLoader.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postProcess.h"

namespace cm
{

static bool bNormalDebug = false;
static bool bTexCoords = true;

static int count = 0;

// an idea with animations could be loading each node animation channel separate
// and enable user to set interpolation value for each channel, this would enable setting stuff with sliders.....
// To automatically generate plugin
// we could expect plugins to reside within 
void recursiveLoad(Node *parent, const aiScene *sc, const aiNode* nd)
{
	RenderableNode * node = new RenderableNode();
	aiMatrix4x4  aim = nd->mTransformation;
	aim.Transpose();
	
	node->setNodeMatrix( cm::M44( aim.a1, aim.a2, aim.a3, aim.a4,
							   aim.b1, aim.b2, aim.b3, aim.b4,
							   aim.c1, aim.c2, aim.c3, aim.c4,
							   aim.d1, aim.d2, aim.d3, aim.d4 ) );
	
	int localcount = count;
	for (int n = 0; n < nd->mNumMeshes; n++)
	{
		const struct aiMesh *mesh = sc->mMeshes[nd->mMeshes[n]];
		
		bool normals = mesh->HasNormals();
		bool texCoords = mesh->HasTextureCoords(0);
		bool hasColors = mesh->HasVertexColors(0);
		
		RenderableModel * model = new RenderableModel();
		Mesh * triMesh = model->addMesh();
		
		triMesh->begin();
		if( bNormalDebug )
			triMesh->enableNormalDebug(true);
		
		if(!hasColors)
			triMesh->setColor(cm::Color(1,1,1,1).getValue());
			
		int nn = triMesh->normals.size();
		
		aiVector3D v;
		
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			localcount++;
			
			v = mesh->mVertices[i];
			triMesh->vertex(cm::Vec3(v.x,v.y,v.z));//const_cast<const float *>(&(mesh->mVertices[i].x))));
			int nn2 = triMesh->normals.size();
		
			if (normals)
			{
				v = mesh->mNormals[i];
				triMesh->normal(cm::Vec3(v.x,v.y,v.z));//(const_cast<const float *>(&(mesh->mNormals[i].x))));
			}
			
			if (texCoords && bTexCoords)
				triMesh->uv(cm::Vec2(mesh->mTextureCoords[0][i].x,
							1.0 - mesh->mTextureCoords[0][i].y));
							
			if( hasColors && !bNormalDebug )
				triMesh->setColor(cm::Color(const_cast<const float *>(&(mesh->mColors[0][i].r))).getValue()); 
		
		}
		
		
		for (int t = 0; t < mesh->mNumFaces; t++)
		{
			const struct aiFace *face = &(mesh->mFaces[t]);

			if (face->mNumIndices == 3)
				triMesh->triangle( face->mIndices[0], face->mIndices[1], face->mIndices[2] );
			else if( face->mNumIndices > 3 )
			{
				triMesh->triangle( face->mIndices[0], face->mIndices[1], face->mIndices[2] );
				triMesh->triangle( face->mIndices[0], face->mIndices[2], face->mIndices[3] );
			}
		}
		
		if( !normals )
			triMesh->computeFaceNormals();
			
		triMesh->end();
		
		node->addRenderable(model);
		
	}
	
	count = localcount;
	
	parent->addChild(node);
	
	// process all children
	for (int i = 0; i < nd->mNumChildren; i++)
	{
		recursiveLoad(node, sc, nd->mChildren[i]);
	}
}


Node * loadScene( const char * path, bool enableTexcoords, bool debugNormals, float scale  )
{
	count = 0;
	bTexCoords = enableTexcoords;
	bNormalDebug = debugNormals;
	
	Assimp::Importer importer;
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE,40.0f);
	const aiScene *scene = importer.ReadFile(path,aiProcess_Triangulate | aiProcess_GenSmoothNormals );
	
	if(!scene)
	{
		debugPrint("Could not load scene %s\n",importer.GetErrorString());
		return 0;
	}
	
	Node * node = new Node();
	recursiveLoad( node, scene, scene->mRootNode );
	node->scale(scale);
	return node;
}

}