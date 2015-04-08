// Code mostly borrwed from LibCinder //
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

#pragma once

#include "cmMath.h"
#include "Shape2d.h"
#include "Path2d.h"
#include "TriMesh.h"
#include "Shape.h"

struct TESStesselator;

namespace cm {



//! Converts an arbitrary Shape2d into a TriMesh2d
class Triangulator {
  public:
	typedef enum Winding { WINDING_ODD, WINDING_NONZERO, WINDING_POSITIVE, WINDING_NEGATIVE, WINDING_ABS_GEQ_TWO } Winding;

	//! Default constructor
	Triangulator();
	~Triangulator();
	
	//! Constructs using a Path2d. \a approximationScale represents how smooth the tesselation is, with 1.0 corresponding to 1:1 with screen space
	Triangulator( const Path2d &path, float approximationScale = 1.0f );
	Triangulator( const Shape2d &path, float approximationScale = 1.0f );
	Triangulator( const Contour &s );
	Triangulator( const Shape &s );

	//! Constructs using a PolyLine2f.
	Triangulator(  const Vec2f * pts, size_t size );

	//! Adds a Shape2d to the tesselation. \a approximationScale represents how smooth the tesselation is, with 1.0 corresponding to 1:1 with screen space	
	void		addShape( const Shape2d &path, float approximationScale = 1.0f );
	//! Adds a Path2d to the tesselation. \a approximationScale represents how smooth the tesselation is, with 1.0 corresponding to 1:1 with screen space	
	void		addPath( const Path2d &path, float approximationScale = 1.0f );
	
	void		addContour( const Contour &s );
	void		addShape( const Shape &s );

	void addPoly(  const Vec2f * pts, size_t size );
	
	unsigned int * getIndices();
	Vec2 *getVertices();
	int getNumVertices();
	int getNumIndices();
	
	
	
	//! Performs the tesselation, returning a TriMesh2d
	TriMesh2d calcMesh( Winding winding = WINDING_NONZERO );
	TriMesh calcMesh3d( Winding winding = WINDING_NONZERO );
protected:	
	void			allocate();
	int				contourCount;
	int				mAllocated;
	TESStesselator*	mTess;
};

TriMesh2d triangulate(  const Path2d &path, float approx = 1.0f, Triangulator::Winding winding = Triangulator::WINDING_NONZERO );
TriMesh2d triangulate(  const Shape2d &path, float approx = 1.0f, Triangulator::Winding winding = Triangulator::WINDING_NONZERO );
TriMesh triangulate3d(  const Path2d &path,  float approx = 1.0f,  Triangulator::Winding winding = Triangulator::WINDING_NONZERO );
TriMesh triangulate3d(  const Shape2d &path,  float approx = 1.0f, Triangulator::Winding winding = Triangulator::WINDING_NONZERO );

} 