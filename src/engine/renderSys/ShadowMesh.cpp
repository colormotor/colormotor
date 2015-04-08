/*
 *  ShadowMesh.cpp
 *
 *  Created by Daniel Berio on 7/14/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#include "ShadowMesh.h"
#include "Mesh.h"
#include "engine/renderSys/utils/HalfEdgeCm.h"

using namespace cm2;

static const char *vs = STRINGIFY(

uniform vec3 lightPos;
uniform float extrusion;
uniform mat4 viewMatrix;

void main()
{
	vec4 wpos = (gl_ModelViewMatrix * gl_Vertex);
	vec4 lpos = viewMatrix * vec4(lightPos,1.0);
	vec3 lightDir = wpos.xyz-lpos.xyz;
	

	// if the vertex is lit
	if ( gl_Color.x > 0.1 )
	{
		// don't move it
		gl_Position = ftransform();
	} 
	else 
	{
		vec4 fin = vec4(wpos.xyz+lightDir.xyz*extrusion,1.0);
		fin = gl_ProjectionMatrix * fin;
		gl_Position = fin;
	}
}
);

static const char * ps = STRINGIFY(

void main( void )
{	
	gl_FragColor =  vec4(1.0,1.0,1.0,1.0);
}


);

namespace cm
{

static ShaderProgram * shadowShader = 0;

ShadowMesh::ShadowMesh()
:
facingFlags(0),
extrusion(10)
{
}

ShadowMesh::~ShadowMesh()
{
	release();
}

void ShadowMesh::release()
{
	if(shader != 0 && shader->getRefCount() == 1)
		shadowShader = 0;
	shader = 0;
	
	SAFE_DELETE_ARRAY(facingFlags);
	
	MeshAdjacency::release();
}

void ShadowMesh::setMesh( Mesh * m )
{
	// pretty unsafe
	if( shadowShader == 0 )
	{
		shadowShader = new ShaderProgram();
		shadowShader->loadString(vs,ps);
	}
	
	release();
	
	shader = shadowShader;
	
	computeAdjacency(m);
	computeFaceNormalsAndCentroids();
	
	facingFlags = new char[getNumFaces()];
}

void ShadowMesh::extrudeEdge( int a, int b )
{
	cm::Vec3 * v = mesh->getVertices();

	gfx::color(1,0,0,1);
	gfx::vertex( v[a] );
	gfx::vertex( v[b] );
	gfx::color(0,0,0,1);
	gfx::vertex( v[b] );
	gfx::vertex( v[a] );
}

void ShadowMesh::draw( const cm::M44 & m, const cm::Vec3 & lpos )
{
	// find front facing faces
	for( int i = 0; i < getNumFaces(); i++ )
	{
		const MeshAdjacency::Face & f = faces[i];
		cm::Vec3 d = lpos-faceCentroids[i];
		
		if( dot( faceNormals[i], d ) < 0.0f )
				facingFlags[i] = 1;
			else
				facingFlags[i] = 0;
	}
	
	shader->bind();
	shader->setFloat("extrusion",extrusion);
	shader->setVec3("lightPos",lpos);
	shader->setMatrix4x4("viewMatrix",m);
	
	setCullMode(CULL_NONE);
	setFillMode(FILL_WIRE);
	beginVertices(QUADS);
	
/*
	beginVertices( LINELIST );
	
	
	for( int i = 0; i < faces.size(); i++ )
	{
		cm::Vec3 p = faceCentroids[i];
		cm::Vec3 n = faceNormals[i];
		gfx::color(0.2,0,1,1);
		gfx::vertex(p);
		gfx::vertex(p+n);
	}*/
	
	// extrude edges
	for( int i = 0; i < edges.size(); i++ )
	{
		const MeshAdjacency::Edge & e = edges[i];
		if( facingFlags[e.f[0]] != facingFlags[e.f[1]] )
		{
			if( facingFlags[e.f[0]] )
				extrudeEdge( e.e[0], e.e[1] );
			else
				extrudeEdge( e.e[1], e.e[0] );
		}
	}
	endVertices();
	
	gfx::color(1,1,1,1);
	setFillMode(FILL_SOLID);
	shader->unbind();
	
}


void computeShadowVolume( Mesh * volume, Mesh * srcMesh, const M44 & m, const Vec3 & lpos )
{
	typedef MeshAdjacency::Edge E;
	typedef MeshAdjacency::Face F;
	
	MeshAdjacency adj;
	adj.computeAdjacency(srcMesh);
	adj.computeFaceNormalsAndCentroids();

	cm::Vec3 * v = srcMesh->getVertices();
	
	F * faces = adj.getFaces();
	E * edges = adj.getEdges();
	Vec3 * normals = adj.getFaceNormals();
	Vec3 * cenps = adj.getFaceCentroids();

	int numEdges = adj.getNumEdges();
	int numFaces = adj.getNumFaces();
		

	std::vector<char> facingFlags;
	facingFlags.assign(numFaces,0);

	// find front facing faces
	for( int i = 0; i < numFaces; i++ )
	{
		F & f = faces[i];
		Vec3 d = lpos-cenps[i];
		
		if( dot( normals[i], d ) < 0.0f )
				facingFlags[i] = 1;
			else
				facingFlags[i] = 0;
	}

	volume->primitive = QUADS;

	#define EXTR(a,b)   volume->setColor(MAKERGBA(1,0,0,1)); \
						volume->vertex( v[a] ); \
						volume->vertex( v[b] ); \
						volume->setColor(MAKERGBA(1,0,0,1)); \
						volume->vertex( v[b] ); \
						volume->vertex( v[a] );

	for( int i = 0; i < numEdges; i++ )
	{
		E & e = edges[i];
		int a = e.e[0];
		int b = e.e[1];

		if( facingFlags[e.f[0]] != facingFlags[e.f[1]] )
		{
			if( facingFlags[e.f[0]] )
			{
				EXTR( e.e[0], e.e[1] )
			}
			else
			{
				EXTR( e.e[1], e.e[0] )
			}
		}
	}
	
	volume->end();
	
}

//#define HEEE

/*
void computeShadowVolume( Mesh * volume, Mesh * srcMesh, bool genCaps )
{
	cm::Vec3 * srcVerts = srcMesh->getVertices();

	TriMesh triMesh;
	for( int i = 0; i < srcMesh->getNumVertices(); i++ )
		triMesh.addVertex(srcVerts[i]);
	std::vector<unsigned int>&	inds = triMesh.getIndices();
	for( int i = 0; i < srcMesh->getNumIndices(); i++ )
		inds.push_back(triMesh.getIndices()->getIndex(i));

	HalfEdge heMesh;
	heMesh.compute(triMesh);
	
	int numEdges = heMesh.getNumEdges();
	int numFaces = heMesh.getNumFaces();
	
	volume->begin();
	
	// first fill normal triangles
	HalfEdge::Face * faces = heMesh.getFaces();
	
	int ind = 0;
	for( int i = 0; i < numFaces; i++ )
	{
		const HalfEdge::Face & f = faces[i];
		HalfEdge::Edge * e = f.edge;
		int a = e->index;
		int b = e->next->index;
		int c = e->next->next->index;
		
		if( genCaps )
		{
			volume->triangle(ind,ind+1,ind+2);
		}
		
		// change indices in half edges to index volume mesh
		e->index = ind;
		e->next->index = ind+1;
		e->next->next->index = ind+2;
		ind += 3;
		
		const cm::Vec3 & pa = srcVerts[a];
		const cm::Vec3 & pb = srcVerts[b];
		const cm::Vec3 & pc = srcVerts[c];
		
		// compute face normal
		cm::Vec3 n = computeNormal(pa,pb,pc);
		
		// add vertices and normals
		volume->vertex(pa);
		volume->normal(n);
		volume->vertex(pb);
		volume->normal(n);
		volume->vertex(pc);
		volume->normal(n);
	}
	
	// build extrusion quads
	
	typedef std::map<HalfEdge::Edge*,char> EdgeMap;

	// edge table to indicate which half-edges we allready used 
	EdgeMap edgeMap;
	
	HalfEdge::Edge * edges = heMesh.getEdges();
		
	for( int i = 0; i < numEdges; i++ )
	{
		HalfEdge::Edge * e = &edges[i];
		HalfEdge::Edge * twin = e->twin;
		
		if( edgeMap.find(e) != edgeMap.end() )
			continue;
			
		if( twin == 0 ) // should handle this, 
			continue;
			
		if( edgeMap.find(twin) != edgeMap.end() )
			continue;
			
		edgeMap[e] = 1;
		edgeMap[twin] = 1;
				
		int a,b,c,d;
		
		// quad indices in ccw order starting from edge vertex ( end of half-edge )
		a = e->index;
		b = e->getPrev()->index;
		c = twin->index;
		d = twin->getPrev()->index;
		
		volume->quad(a,b,c,d);
//		volume->triangle(a,b,c);
//		volume->triangle(c,d,a);
	}
	
	
	volume->end();
	
}
*/
std::vector<Vec3> computeFaceNormals( Mesh * mesh, const HalfEdge & he )
{
    std::vector <Vec3> N;
    int n = he.getNumFaces(); //adj.faces.size();
    N.resize(n);
    Vec3 * V = mesh->getVertices();
    const HalfEdge::Face * faces = he.getFaces();
    for( int i = 0; i < n; i++ )
    {
        const HalfEdge::Face & f = faces[i];//faces[i];
        int a,b,c;
        f.getIndices(a,b,c);
        N[i] = computeNormal(V[a],V[b],V[c]);//f.v[0]],V[f.v[1]],V[f.v[2]]);
    }
    
    return N;
}

void computeShadowVolumeHE( Mesh * volume, Mesh * srcMesh, bool genCaps )
{
	cm::Vec3 * srcVerts = srcMesh->getVertices();
	/*
	TriMesh triMesh;
	for( int i = 0; i < srcMesh->getNumVertices(); i++ )
		triMesh.appendVertex(srcVerts[i]);
	std::vector<unsigned int>&	inds = triMesh.getIndices();
	for( int i = 0; i < srcMesh->getNumIndices(); i++ )
		inds.push_back(srcMesh->getIndices()->getIndex(i));
	*/

	HalfEdge heMesh;
	heMesh.compute(srcMesh);
	
	int numEdges = heMesh.getNumEdges();
	int numFaces = heMesh.getNumFaces();

	std::vector<Vec3> N = computeFaceNormals(srcMesh,heMesh);
	const HalfEdge::Face * faces = heMesh.getFaces();
		
	//volume->primitive = QUADS;
	volume->begin();
	int v1,v2,v3;
	cm::Vec3 * V = srcMesh->getVertices();

	if( genCaps )
		for( int i = 0; i < numFaces; i++ )
		{
			const HalfEdge::Face & f = faces[i];
			f.getIndices(v1,v2,v3);

			const Vec3 & a = V[v1];
			const Vec3 & b = V[v2];
			const Vec3 & c = V[v3];
			const Vec3 & n = N[i];
			
			volume->vertex(c);
			volume->normal(n);
			volume->vertex(b);
			volume->normal(n);
			volume->vertex(a);
			volume->normal(n);
		}

	const HalfEdge::Edge * edges = heMesh.getEdges();
	

	for( int i = 0; i < numEdges; i++ )
	{
		const HalfEdge::Edge * e = &edges[i];
		const HalfEdge::Edge * twin = e->twin;
		
		//if( edgeMap.find(e) != edgeMap.end() )
		//	continue;
			
		//if( twin == 0 ) // should handle this, 
		//	continue;
			
		//if( edgeMap.find(twin) != edgeMap.end() )
		//	continue;
		

		// coords of edge
		int ia,ib;
		e->getIndices(ia,ib);

		const Vec3 & a = V[ia];
		const Vec3 & b = V[ib];
		// normals of incident faces
		Vec3 n1,n2;
		n1 = N[e->face->index]; //srcMesh->getNormals(e.f[0]);

		if(twin)
		{
			n2 = N[twin->face->index];
		}
		else
		{
			n2 = -n1;
		}
		
		Vec3  verts[4][2] = 
		{
			a,n1,b,n1,b,n2,a,n2
		};

		int inds[6] = { 0,1,2,0,2,3 };

		for ( int j = 0; j < 6; j++ )
		{
			int ind = inds[j];
			volume->vertex(verts[ind][0]);
			volume->normal(verts[ind][1]);	
		}
	}

	volume->end();
	
}


void computeShadowVolume( Mesh * volume, Mesh * srcMesh, bool genCaps )
{
	typedef MeshAdjacency::Edge E;
	typedef MeshAdjacency::Face F;
	
	MeshAdjacency adj;
	adj.computeAdjacency(srcMesh);
	adj.computeFaceNormalsAndCentroids();

	cm::Vec3 * v = srcMesh->getVertices();
	
	F * faces = adj.getFaces();
	E * edges = adj.getEdges();
	Vec3 * normals = adj.getFaceNormals();
	
	int numEdges = adj.getNumEdges();
	int numFaces = adj.getNumFaces();
		
	//volume->primitive = QUADS;
	volume->begin();
	
	if( genCaps )
		for( int i = 0; i < numFaces; i++ )
		{
			F & f = faces[i];
			const Vec3 & a = v[f.v[0]];
			const Vec3 & b = v[f.v[1]];
			const Vec3 & c = v[f.v[2]];
			const Vec3 & n = normals[i];
			
			volume->vertex(c);
			volume->normal(n);
			volume->vertex(b);
			volume->normal(n);
			volume->vertex(a);
			volume->normal(n);
		}
	
	for( int i = 0; i < numEdges; i++ )
	{
		E & e = edges[i];
		
		// coords of edge
		const Vec3 & a = v[e.e[0]];
		const Vec3 & b = v[e.e[1]];
		// normals of incident faces
		const Vec3 & n1 = normals[e.f[0]];
		const Vec3 & n2 = normals[e.f[1]];
	
		Vec3  verts[4][2] = 
		{
			a,n1,b,n1,b,n2,a,n2
		};

		int inds[6] = { 0,1,2,0,2,3 };

		for ( int j = 0; j < 6; j++ )
		{
			int ind = inds[j];
			volume->vertex(verts[ind][0]);
			volume->normal(verts[ind][1]);	
		}
		/*volume->vertex(a);
		volume->normal(n1);
		volume->vertex(b);
		volume->normal(n1);

		volume->vertex(b);
		volume->normal(n2);
		volume->vertex(a);
		volume->normal(n2);*/

		/*	
		volume->vertex(a);
		volume->normal(n1);
		volume->vertex(b);
		volume->normal(n1);

		volume->vertex(b);
		volume->normal(n2);
		volume->vertex(a);
		volume->normal(n2);*/
	}

	volume->end();
	
}


Mesh * generateShadowMesh( Mesh * srcMesh, bool genCaps, bool he )
{
	Mesh * volume = new Mesh();
	if(he)
		computeShadowVolumeHE(volume,srcMesh,genCaps);
	else
		computeShadowVolume(volume,srcMesh,genCaps);
	return volume;
}


}

