#pragma once
#include "engine/engineIncludes.h"
#include "engine/renderSys/MeshAdjacency.h"
namespace cm
{

class Mesh;
Mesh * generateShadowMesh( Mesh * srcMesh, bool genCaps = true, bool he = false );
void computeShadowVolume( Mesh * volume, Mesh * srcMesh, bool genCaps = true );
void computeShadowVolumeHE( Mesh * volume, Mesh * srcMesh, bool genCaps = true );
void computeShadowVolume( Mesh * volume, Mesh * srcMesh, const M44 & m, const Vec3 & lpos );
//Mesh * generateShadowMesh( Mesh * srcMesh, bool genCaps = false );

class ShadowMesh : public MeshAdjacency
{
	public:
		
		ShadowMesh();
		~ShadowMesh();
		
		void release();
		
		void setMesh( Mesh * m );
		void draw( const cm::M44 & viewMat, const cm::Vec3 & lpos );
			
		float extrusion;
	private:
		void extrudeEdge( int a, int b );
		char * facingFlags;
		
		RefPtr< ShaderProgram > shader;
};

}