//
//  cmGeomGfx.h
//  colormotor
//
//  Created by colormotor on 10/16/13.
//
//
// Mostly adapted from Cinder code.

#pragma once
#include "Path2d.h"
#include "Shape2d.h"
#include "Shape.h"
#include "Triangulator.h"
#include "cmGfx.h"

namespace cm
{

    namespace gfx
    {
        
//! Draws a cinder::TriMesh \a mesh at the origin.
void draw( const TriMesh2d &mesh );
//! Draws a range of triangles starting with triangle # \a startTriangle and a count of \a triangleCount from cinder::TriMesh \a mesh at the origin.
void drawRange( const TriMesh2d &mesh, size_t startTriangle, size_t triangleCount );
//! Draws a cinder::TriMesh \a mesh at the origin.
void draw( const TriMesh &mesh );
//! Draws a range of triangles starting with triangle # \a startTriangle and a count of \a triangleCount from cinder::TriMesh \a mesh at the origin.
void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount );

void draw( const std::vector <Vec2> & points, bool close = false);
void draw( const Vec2 * points, int n, bool close = false);

void drawSolid( const std::vector <Vec2> & points,  int winding = Triangulator::WINDING_NONZERO  );
void drawSolid( const Vec2 * points, int n,  int winding = Triangulator::WINDING_NONZERO  );

TriMesh2d makeMesh( const Path2d & path, float approximationScale = 1.0f, int winding = Triangulator::WINDING_NONZERO );
TriMesh2d makeMesh( const Shape2d & shape, float approximationScale = 1.0f, int winding = Triangulator::WINDING_NONZERO );

//! Draws a Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const  Path2d &path2d, float approximationScale = 1.0f );
//! Draws a Shape2d \a shape2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const  Shape2d &shape2d, float approximationScale = 1.0f );
//! Draws a solid (filled) Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
void drawSolid( const  Path2d &path2d, float approximationScale = 1.0f, int winding = Triangulator::WINDING_NONZERO );
//! Draws a solid (filled) Shape2d \a shape2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
void drawSolid( const  Shape2d &shape2d, float approximationScale = 1.0f, int winding = Triangulator::WINDING_NONZERO );

void drawTriangle( const Vec2f & a, const Vec2f & b, const Vec2f & c );

// Draw an archimedean spiral with formula r = a*theta
void drawSpiral( const Vec2 & pos, float a, float thetaInc, int n );

void draw( const Contour &s );
void fill( const Contour &s, int winding = Triangulator::WINDING_NONZERO );
void draw( const Shape &s );
void fill( const Shape &s, int winding = Triangulator::WINDING_NONZERO );
}
}