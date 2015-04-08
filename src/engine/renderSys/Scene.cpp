/*
 *  Scene.cpp
 *
 *  Created by Daniel Berio on 7/10/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */


#include "Scene.h"
#include "SceneLoader.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postProcess.h"
#include "DefaultMaterial.h"
#include "ShadowMesh.h"

namespace cm
{

Scene::Scene()
:
root(0)
{
}

void Scene::release()
{
	materials.release();
	meshes.release();
	textures.release();
	root = 0;
	DELETE_VECTOR(anims);
	shadowMeshes.clear();
	//DELETE_VECTOR(shadowMeshes);
}

typedef std::map < std::string , Node* > NodeMap;
static NodeMap nodeMap;

static Node * findNode( const char * n )
{
	NodeMap::iterator it = nodeMap.find( n );
	if( it==nodeMap.end())
		return 0;
	return it->second;
}

static void loadMeshes( Scene * scene, const aiScene * sc, unsigned int flags )
{
	for( int i = 0; i < sc->mNumMeshes; i++ )
	{
		aiMesh * mesh = sc->mMeshes[i];
		
		bool normals = mesh->HasNormals();
		bool texCoords = mesh->HasTextureCoords(0);
		bool hasColors = mesh->HasVertexColors(0);
		
		bool bNormalDebug = flags & Scene::DEBUG_CLR_NORMALS;
		bool bTexCoords = !( flags & Scene::DISABLE_TEXCOORDS );
		Mesh * triMesh = new Mesh;
		
		triMesh->begin();
		if( bNormalDebug )
			triMesh->enableNormalDebug(true);
		
		if(!hasColors)
			triMesh->setColor(cm::Color(1,1,1,1).getValue());
			
		int nn = triMesh->normals.size();
		
		aiVector3D v;
		
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
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
		
		if( mesh->mNumFaces )
		{
			if( mesh->mNumVertices >= 0xFFFF )
			{
				triMesh->createIndices( Mesh::INDEX_32 );
			}
			else
			{
				triMesh->createIndices( Mesh::INDEX_16 );
			}
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
		
		if( !normals || ( flags & Scene::FORCE_FACE_NORMALS ) )
			triMesh->computeFaceNormals();
			
		triMesh->end();
		triMesh->setId(i);
		scene->addMesh( triMesh );
	}
}

static void loadMaterials( Scene * scene, const aiScene * sc, unsigned int flags )
{
	for( int i = 0; i < sc->mNumMaterials; i++ )
	{
		aiMaterial * m = sc->mMaterials[i];
		Material * mtl = new Material();
		scene->addMaterial(mtl);
		mtl->setId(i);
		
		aiColor4D clr;
		if(AI_SUCCESS == aiGetMaterialColor(m, AI_MATKEY_COLOR_DIFFUSE, &clr))
		{
			mtl->diffuseColor( clr.r, clr.g, clr.b, clr.a );
			mtl->ambientColor( clr.r*0.3, clr.g*0.3, clr.b*0.3, clr.a );

		}
		
		if(AI_SUCCESS == aiGetMaterialColor(m, AI_MATKEY_COLOR_AMBIENT, &clr))
		{
		//	mtl->ambientColor( clr.r, clr.g, clr.b, clr.a );
		}
		
		if(!mtl->init())
			debugPrint("Failed to load material \n");
	}
}

static void recursiveLoad(Scene * scene, Node *parent, const aiScene *sc, const aiNode* nd, unsigned int flags )
{
	RenderableNode * node = new RenderableNode();
	nodeMap[nd->mName.data] = node;
	
	aiMatrix4x4  aim = nd->mTransformation;
	aim.Transpose();
	
	node->setNodeMatrix( cm::M44( aim.a1, aim.a2, aim.a3, aim.a4,
							   aim.b1, aim.b2, aim.b3, aim.b4,
							   aim.c1, aim.c2, aim.c3, aim.c4,
							   aim.d1, aim.d2, aim.d3, aim.d4 ) );
	
	for (int n = 0; n < nd->mNumMeshes; n++)
	{
		const struct aiMesh *mesh = sc->mMeshes[nd->mMeshes[n]];
		
		Mesh * triMesh = scene->getMesh( nd->mMeshes[n] );
		
		if( !triMesh )
			continue;
			
		RenderableModel * model = new RenderableModel();
		model->addMesh( triMesh );
		
		Material * mtl = scene->getMaterial( mesh->mMaterialIndex );
		if( mtl )
		{
			model->addMaterial(mtl);
		}
		
		node->addRenderable(model);
	}
	
	parent->addChild(node);
	
	// process all children
	for (int i = 0; i < nd->mNumChildren; i++)
	{
		recursiveLoad(scene, node, sc, nd->mChildren[i], flags);
	}
	
}

static AnimationChannelVec3 *getAnimationChannelVec3( aiVectorKey * keys, int n )
{
	if( !n )
		return 0;
		
	AnimationChannelVec3 * chan = new AnimationChannelVec3();
	for( int i = 0; i < n; i++ )
	{
		const aiVectorKey & k = keys[i];
		// this does useless work since assimp allready sorts keyframes
		chan->addKey( cm::Vec3(k.mValue.x,k.mValue.y,k.mValue.z), k.mTime );
	}
	
	return chan;
}

static AnimationChannelQuat *getAnimationChannelQuat( aiQuatKey * keys, int n )
{
	if( !n )
		return 0;
		
	AnimationChannelQuat * chan = new AnimationChannelQuat();
	for( int i = 0; i < n; i++ )
	{
		const aiQuatKey & k = keys[i];
		// this does useless work since assimp allready sorts keyframes
		chan->addKey( cm::Quat(k.mValue.x,k.mValue.y,k.mValue.z,k.mValue.w), k.mTime );
	}
	
	return chan;
}

static void loadAnimation( Scene * scene, const aiAnimation * ia )
{
	SceneAnimation * sceneAnim = new SceneAnimation();
	for( int i = 0; i < ia->mNumChannels; i++ )
	{
		aiNodeAnim * na = ia->mChannels[i];
		Node * node = findNode( na->mNodeName.data );
		if( !node )
			continue;
		AnimationChannelVec3 * pos = 0;
		AnimationChannelQuat * rot = 0;
		AnimationChannelVec3 * scale = 0;
		
		if( na->mPositionKeys )
		{
			pos = getAnimationChannelVec3( na->mPositionKeys, na->mNumPositionKeys );
		}
		
		if( na->mRotationKeys )
		{
			rot = getAnimationChannelQuat( na->mRotationKeys, na->mNumRotationKeys );
		}
		
		if( na->mScalingKeys )
		{
			scale = getAnimationChannelVec3( na->mScalingKeys, na->mNumScalingKeys );
		}
		
		sceneAnim->addAnimation( node, pos,rot,scale );
	}
	
	if(sceneAnim->getNumAnimations() == 0)
	{
		SAFE_DELETE(sceneAnim)
	}
	else
	{
		scene->addAnimation(sceneAnim);
	}
}

static void loadAnimations( Scene * scene, const aiScene * sc )
{
	if( !sc->HasAnimations() )
		return;
		
	for( int i = 0; i < sc->mNumAnimations; i++ )
	{
		loadAnimation(scene,sc->mAnimations[i]);
	}
}

RenderableNode * Scene::createMeshNode( int meshIndex )
{
	Mesh * m = meshes[meshIndex];
	RenderableNode * rn = new RenderableNode();
	RenderableModel * r = new RenderableModel();
	r->addMesh( m );
	Mesh * shad = shadowMeshes[meshIndex];
	if(!shad)
		shadowMeshes[meshIndex] = generateShadowMesh(m);
	shad = shadowMeshes[meshIndex];
	r->setShadowMesh(0,shad);
	rn->addRenderable(r);
	return rn;
}

bool Scene::load( const char * path, unsigned int flags  )
{
	release();
	nodeMap.clear();
	
	Assimp::Importer importer;
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE,40.0f);
	
	const aiScene *scene = importer.ReadFile(path,aiProcess_Triangulate | aiProcess_GenSmoothNormals );
	
	if(!scene)
	{
		debugPrint("Could not load scene %s\n",importer.GetErrorString());
		return false;
	}
	
	loadMeshes( this, scene, flags );
	//loadTextures( this, scene );
	loadMaterials( this, scene, flags );
	
	root = new Node();
	recursiveLoad( this, root, scene, scene->mRootNode, flags );
	
	addChild(root);

	loadAnimations(this, scene);
	
	return true;
}
			
TriMesh loadTriMesh( const std::string & path, float maxSmoothingAngle, bool bNormalDebug )
{

	Assimp::Importer importer;
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE,maxSmoothingAngle);
	
	const aiScene *sc = importer.ReadFile(path,aiProcess_Triangulate | aiProcess_GenSmoothNormals );

	if(!sc)
		return TriMesh();

	TriMesh res;

	for( int i = 0; i < sc->mNumMeshes; i++ )
	{
		aiMesh * mesh = sc->mMeshes[i];
		
		bool normals = mesh->HasNormals();
		bool texCoords = mesh->HasTextureCoords(0);
		bool hasColors = mesh->HasVertexColors(0);
		
		TriMesh m;
		
		aiVector3D v;
		
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			v = mesh->mVertices[i];
			m.appendVertex( Vec3(v.x, v.y, v.z) );

			Vec3 n;
			if (normals)
			{
				v = mesh->mNormals[i];
				n = Vec3(v.x, v.y, v.z);
				m.appendNormal(n);
			}
			
			if (texCoords)
			{
				m.appendTexCoord( cm::Vec2( mesh->mTextureCoords[0][i].x,
											1.0 - mesh->mTextureCoords[0][i].y ) );
			}
							
			if( hasColors )
			{
				Color clr;
				if(bNormalDebug)
					normalColor( &clr, n );
				else
					clr = Color( const_cast<const float *>(&(mesh->mColors[0][i].r)) );
				m.appendColorRGBA( clr );
			}
			else
			{
				if(bNormalDebug)
				{
					Color clr;
					normalColor( &clr, n );
					m.appendColorRGBA( clr );
				}
			}
		}
		
		
		for (int t = 0; t < mesh->mNumFaces; t++)
		{
			const struct aiFace *face = &(mesh->mFaces[t]);

			if (face->mNumIndices == 3)
			{
				m.appendTriangle(face->mIndices[0], face->mIndices[1], face->mIndices[2]);
			}
			else if( face->mNumIndices > 3 )
			{
				m.appendTriangle( face->mIndices[0], face->mIndices[1], face->mIndices[2] );
				m.appendTriangle( face->mIndices[0], face->mIndices[2], face->mIndices[3] );
			}
		}

		res.append(m);
	}

	return res;
}			

}