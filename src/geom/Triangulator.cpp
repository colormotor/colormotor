/********************************************************************
 --------------------------------------------------------------------
 --           _,(_)._
 --      ___,(_______).      ____
 --    ,'__.           \    /\___\-.
 --   /,' /             \  /  /     \
 --  | | |              |,'  /       \
 --   \`.|                  /    ___|________
 --    `. :           :    /     COLORMOTOR
 --      `.            :.,'        Graphics and Multimedia Framework
 --        `-.________,-'          © Daniel Berio
 --								   http://www.enist.org
 --								   drand48@gmail.com
 --
 --------------------------------------------------------------------
 ********************************************************************/


/*
 Copyright (c) 2011, The Cinder Project: http://libcinder.org
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "Triangulator.h"
#include "Shape2d.h"
#include "libtess2/tesselator.h"

using namespace std;

namespace cm {

void* stdAlloc( void* userData, unsigned int size )
{
	int *allocated = (int*)userData;
	*allocated += (int)size;
	return malloc( size );
}

void stdFree( void* userData, void* ptr )
{
	free( ptr );
}

Triangulator::Triangulator( const Path2d &path, float approximationScale )
{	
	allocate();
	addPath( path, approximationScale );

}

Triangulator::Triangulator( const Shape2d &shape, float approximationScale )
{	
	allocate();
	addShape( shape, approximationScale );
}

Triangulator::Triangulator( const Shape &s )
{	
	allocate();
	addShape( s );

}

Triangulator::Triangulator( const Contour &s )
{	
	allocate();
	addContour( s );
}

Triangulator::Triangulator( const Vec2f * pts, size_t size )
{
	allocate();
	addPoly(pts,size);
}


Triangulator::Triangulator()
{
	allocate();
}

Triangulator::~Triangulator()
{
	if(mTess)
		tessDeleteTess(mTess);
	mTess = 0;
}

void Triangulator::allocate()
{
	contourCount = 0;
	
	mAllocated = 0;
	
	TESSalloc ma;
	memset( &ma, 0, sizeof(ma) );
	ma.memalloc = stdAlloc;
	ma.memfree = stdFree;
	ma.userData = (void*)&mAllocated;
	ma.extraVertices = 2560; // realloc not provided, allow 256 extra vertices.

	mTess = tessNewTess( &ma );
}

void Triangulator::addShape( const Shape2d &shape, float approximationScale )
{
	size_t numContours = shape.getContours().size();
	for( size_t p = 0; p < numContours; ++p ) {
		addPath( shape.getContour(p), approximationScale );
	}	
}

void Triangulator::addPath( const Path2d &path, float approximationScale )
{
	if(!path.size())
		return;
	
	vector<Vec2f> subdivided = path.subdivide( approximationScale );
	if(!subdivided.size())
		return;
	contourCount++;
	tessAddContour( mTess, 2, &subdivided[0], sizeof(float) * 2, subdivided.size() );
}

void Triangulator::addShape( const Shape &s )
{
	for( size_t p = 0; p < s.size(); ++p ) {
		addContour( s[p] );
	}	
}

void Triangulator::addContour( const Contour &c )
{
	if(!c.size())
		return;

	contourCount++;
	tessAddContour( mTess, 2, &c.points[0], sizeof(float) * 2, c.size() );
}



void Triangulator::addPoly( const Vec2f * pts, size_t size )
{
	if(!size || !pts)
		return;
	contourCount++;
	tessAddContour( mTess, 2, &pts[0], sizeof(float) * 2, size );
}

TriMesh2d Triangulator::calcMesh( Winding winding )
{
	TriMesh2d result;
	if(!contourCount)
		return result;
	
	tessTesselate( mTess, (int)winding, TESS_POLYGONS, 3, 2, 0 );
	
	result.appendVertices( (Vec2f*)tessGetVertices( mTess ), tessGetVertexCount( mTess ) );
	result.appendIndices( (uint32_t*)( tessGetElements( mTess ) ), tessGetElementCount( mTess ) * 3 );
	
	return result;
}

unsigned int * Triangulator::getIndices()
{
	(unsigned int *)tessGetElements( mTess);
}

Vec2 *Triangulator::getVertices()
{
	return (Vec2*)tessGetVertices( mTess );
}

int Triangulator::getNumVertices()
{
	return tessGetVertexCount( mTess );
}
	
int Triangulator::getNumIndices()
{
	return tessGetElementCount( mTess ) * 3;
}
	


TriMesh Triangulator::calcMesh3d( Winding winding )
{
	TriMesh result;
	if(!contourCount)
		return result;
	
	tessTesselate( mTess, (int)winding, TESS_POLYGONS, 3, 2, 0 );
	
	Vec2f * verts = (Vec2f*)tessGetVertices( mTess );
	int n = tessGetVertexCount( mTess );
	
	for( int i = 0; i < n; i++ )
		result.appendVertex(Vec3f(verts[i].x,verts[i].y,0));
		
	result.appendIndices( (uint32_t*)( tessGetElements( mTess ) ), tessGetElementCount( mTess ) * 3 );
	
	return result;
}

TriMesh2d triangulate(  const Path2d &path, float approx ,Triangulator::Winding winding )
{
	return  Triangulator( path,approx ).calcMesh((Triangulator::Winding)winding);
}

TriMesh2d triangulate(  const Shape2d &path, float approx ,Triangulator::Winding winding )
{
	return  Triangulator( path,approx ).calcMesh((Triangulator::Winding)winding);
}

TriMesh triangulate3d(  const Path2d &path,float approx , Triangulator::Winding winding )
{
	return  Triangulator( path,approx ).calcMesh3d((Triangulator::Winding)winding);
}

TriMesh triangulate3d(  const Shape2d &path,float approx , Triangulator::Winding winding )
{
	return  Triangulator( path,approx ).calcMesh3d((Triangulator::Winding)winding);
}


} // namespace cinder
