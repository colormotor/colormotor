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

/// \defgroup RenderSystem Render System
/// Materials and other 3d rendering utilities



#pragma once

#include "engine/engineIncludes.h"
#include "engine/renderSys/Material.h"
#include "engine/renderSys/Mesh.h"

namespace cm {

/// \addtogroup Gfx
/// @{

/// \addtogroup RenderSystem
/// @{

///////////////////////////////////////////////////////////////////////////
/// base renderable object
/// an object the render system can actually render, usually linked to a scene graph node 
/// from which it gets transformation data.
/// links geometry or other kinds of primitives to a group of materials
class Renderable : public Base
{
public:
	RTTIOBJECT( "Renderable", Renderable, Base )
	Renderable()
	{
		materials = new Group<Material>;
		invisible = false;
	}

	~Renderable()
	{
		release();
	};

	virtual void release()
	{
		materials = 0;
	}

	virtual	void render( bool useMaterial = true )
	{
		// add a dummy material
		if(!getNumMaterials())
		{
			addMaterial( new Material() );
		}
		
		for( int i = 0; i < getNumMaterials(); i++ )
		{
			render( i, useMaterial  );
		}
	}

	virtual void renderShadows() { }
	virtual void updateShadows() {}
	virtual void updateShadows( const M44 & m, const Vec3 & lpos ) {}
	
	virtual void render( int index, bool useMaterial ) {}	
	virtual void update(double msecs) {}

	Material *	addMaterial() { Material * mtl = new Material(); materials->add( mtl ); return mtl; }
	Material *	getMaterial( int ind ) { return materials->get(ind); }
	void		addMaterial( Material * mtl ) { materials->add(mtl); }
	void		setMaterial( int ind, Material * mtl ) { materials->set(ind,mtl); }
	void		setMaterials( Group<Material> * mtlGroup ) { materials = mtlGroup; }
	int		getNumMaterials() { return materials->size(); }

	void setDiffuseColor( const Color & clr );
	void setAmbientColor( const Color & clr );
	void setSpecularColor( const Color & clr );
	
	bool invisible;
	cm::RefPtr< Group<Material> > materials;
protected:
};


/// Generally with meshes, user should be able to 
///////////////////////////////////////////////////////////////////////////
/// renderable geometry

class RenderableModel : public Renderable
{
public:
	RTTIOBJECT( "RenderableModel", RenderableModel, Renderable )
	
	struct MeshInfo
	{
		RefPtr<Mesh> mesh;
		RefPtr<Mesh> shadowMesh;
		//bool local;
		int materialIndex;
	};
	
	RenderableModel()
	:
	materialMeshMap(0)
	{
	}

	~RenderableModel()
	{
		release();
	};

	virtual void release();
	
	void render( bool useMaterial = true )
	{
		Renderable::render(useMaterial);
		renderShadows();
	}
	
	void updateShadows();
	void updateShadows( const M44 & m, const Vec3 & lpos );
	void renderShadows();
	void render( int materialIndex, bool useMaterial );
	Mesh * addMesh( int materialIndex = 0 );
	void addMeshAndShadow( Mesh * mesh, int materialIndex = 0 );
	void addMesh( Mesh * mesh, int materialIndex = 0 );
	virtual void renderWithoutMaterial();
	int getNumMeshes() const { return meshes.size(); }

	void setShadowMesh( int index, Mesh * mesh ) { meshes[index].shadowMesh = mesh; }
	
	Mesh * getMesh( int index ) const { return meshes[index].mesh; }
	const MeshInfo & getMeshInfo( int index ) const { return meshes[index]; }

	void buildShadowVolumes( bool forceUpdate = false, bool useHalfEdge = false );
	void buildShadowVolumes( const M44 & m, const Vec3 & lpos );
	
protected:
	// This is unoptimal. but this func should be called only in non time critical situations
	void updateMaterialMeshMap()
	{
		SAFE_DELETE_ARRAY(materialMeshMap);
		materialMeshMap = new int[getNumMaterials()];
		memset(materialMeshMap,-1,getNumMaterials()*sizeof(int));
		for( int i = 0; i < getNumMeshes(); i++ )
			materialMeshMap[ meshes[i].materialIndex ] = i;
	}
	
	std::vector <MeshInfo> meshes;
	int * materialMeshMap;
};

///////////////////////////////////////////////////////////////////////////
/// renderable skinned geometry
/*
class COLORMOTOR_API RenderableSkinnedGeometry : public RenderableGeometry
{
public:
	CM_RTTI(RenderableSkinnedGeometry,RenderableGeometry)

		RenderableSkinnedGeometry()
	{
	}

	~RenderableSkinnedGeometry()
	{
		release();
	};

	virtual void release( RELEASE_FLAGS flags = RELEASE_ALL )
	{
		if(flags&RELEASE_ALL)
			RenderableGeometry::release(flags);
	}

	virtual bool	render( float msecs )
	{
		setMatrixPalette();
		//frame(msecs);
		return RenderableGeometry::render(msecs);
	}

	virtual bool	render( uint32 index, float msecs )
	{
		setMatrixPalette();
		return RenderableGeometry::render(index,msecs);
	}

	virtual void	frame(float msecs);

	void	setSkeleton( Skeleton * s ) { _skeleton = s; }
	Skeleton *getSkeleton() { return _skeleton; }

	void			setMatrixPalette();

protected:
	RefPtr<Skeleton>	_skeleton;
	//cm::M44

};

*/

/// @}

/// @}


}