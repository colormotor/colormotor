///////////////////////////////////////////////////////////////////////////                                                     
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

#include "Renderable.h"
#include "ShadowMesh.h"

namespace cm
{

void Renderable::setDiffuseColor( const Color & clr )
{
    if(!materials->size())
        addMaterial();
    
	if( materials->size() )
	{
		Material * mtl = materials->get(0);
		mtl->diffuseColor = clr;
	}
}

void Renderable::setAmbientColor( const Color & clr )
{
    if(!materials->size())
        addMaterial();
    
	if( materials->size() )
	{
		Material * mtl = materials->get(0);
		mtl->ambientColor = clr;
	}
}

void Renderable::setSpecularColor( const Color & clr )
{
    if(!materials->size())
        addMaterial();
    
	if( materials->size() )
	{
		Material * mtl = materials->get(0);
		mtl->specularColor = clr;
	}
}


//////////////////////////////////////////////////////////////////////////

void RenderableModel::release()
{
	/*for( int i = 0; i < getNumMeshes(); i++ )
	{
		const MeshInfo & m = meshes[i];
		if(m.local && m.mesh )
			delete m.mesh;
		if(m.local && m.shadowMesh)
			delete m.shadowMesh;
	}
	*/
	meshes.clear();
}

//////////////////////////////////////////////////////////////////////////

void RenderableModel::render( int materialIndex, bool useMaterial )
{
	if(!getNumMeshes())
		return;
		
	int meshIndex = materialMeshMap[materialIndex];
	if( meshIndex != -1 )
	{
		if( useMaterial )
		{
			Material * mtl = getMaterial(materialIndex);
			mtl->begin();
			getMesh(meshIndex)->draw();
			mtl->end();
		}
		else
			getMesh(meshIndex)->draw();
	}
	
	
}

void RenderableModel::renderShadows()
{
	
	for( int i = 0; i < getNumMeshes(); i++ )
	{
		Mesh * m = meshes[i].mesh;
		
		/*
		if(!m)
			continue;
	
		m->draw(); //m->drawNormals(3.0);
		*/
		m = meshes[i].shadowMesh;
		if(!m)
			continue;
		
		m->draw();
	}
}

//////////////////////////////////////////////////////////////////////////

Mesh * RenderableModel::addMesh( int materialIndex  )
{
	MeshInfo m;
	m.mesh = new Mesh();
	m.shadowMesh = 0;
	m.materialIndex = materialIndex;
	meshes.push_back(m);
	updateMaterialMeshMap();
	return m.mesh;
}

//////////////////////////////////////////////////////////////////////////

// TODO refPointers!!!!
void RenderableModel::addMesh( Mesh * mesh, int materialIndex )
{
	MeshInfo m;
	m.mesh = mesh;
	m.shadowMesh = 0;
	m.materialIndex = materialIndex;
	meshes.push_back(m);
	updateMaterialMeshMap();
} 

//////////////////////////////////////////////////////////////////////////


void RenderableModel::renderWithoutMaterial()
{ 
	for( int i = 0; i < getNumMeshes(); i++ )
	{
		const MeshInfo & mesh = meshes[i];
		mesh.mesh->draw();
	}
}

//////////////////////////////////////////////////////////////////////////

void RenderableModel::updateShadows()
{
	buildShadowVolumes(true);
}

void RenderableModel::updateShadows( const M44 & m, const Vec3 & lpos )
{
	buildShadowVolumes(m,lpos);
}


//////////////////////////////////////////////////////////////////////////

void RenderableModel::buildShadowVolumes( bool forceUpdate, bool useHalfEdge )
{
	for( int i = 0; i < getNumMeshes(); i++ )
		if(!meshes[i].shadowMesh)
			meshes[i].shadowMesh = generateShadowMesh( meshes[i].mesh, true, useHalfEdge );
		else if ( forceUpdate )
		{
			if(useHalfEdge)
				computeShadowVolumeHE( meshes[i].shadowMesh, meshes[i].mesh );
			else
				computeShadowVolume( meshes[i].shadowMesh, meshes[i].mesh );
		}
}

	
void RenderableModel::buildShadowVolumes( const M44 & m, const Vec3 & lpos )
{
	for( int i = 0; i < getNumMeshes(); i++ )
	{
		if(!meshes[i].shadowMesh)
			meshes[i].shadowMesh = new Mesh();
		computeShadowVolume( meshes[i].shadowMesh, meshes[i].mesh, m, lpos );
	}
}

	

#ifdef CAZZULO
void	RenderableSkinnedGeometry::setMatrixPalette()
{
	Skeleton * s = _skeleton;
	// a bit hacky..
	Material * mtl = getMaterial(0);
	if(mtl)
	{
		/// \todo use var-> must have an array type.. or buffer type... 
		const char * hand = mtl->getParameterByName("skinnedMatrices");
		mtl->getShader()->setMatrixArray(hand,s->getMatrixPalette(),s->getNumBones());//setValue(hand,skinMatrixArray,s->getNumBones()*sizeof(cm::M44));
	}
}

void RenderableSkinnedGeometry::frame( float msecs )
{
	Skeleton * s = _skeleton;
	s->transform(msecs);
	/*
	Material * mtl = getMaterial(0);
	if(mtl)
	{
		const char * hand = mtl->getParameterByName("skinnedMatrices");
		mtl->getShader()->setMatrixArray(hand,s->getMatrixPalette(),s->getNumBones());//setValue(hand,skinMatrixArray,s->getNumBones()*sizeof(cm::M44));
	}*/
}
#endif
}