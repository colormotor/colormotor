/*
 *  CMGeometryUtils.h
 *
 *  Created by Daniel Berio on 9/28/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

/*
 *  cmGeometryUtils.h
 *  colormotor
 *
 *  Created by Daniel Berio on 6/26/09.
 *  Copyright 2009 http://www.enist.org. All rights reserved.
 *
 */

#pragma once 

#include "gfx/CMGfxIncludes.h"
#include "../cmMath.h"

#define GEOMETRYTOOLS

#ifdef WIN32
#include <GL/glu.h>
#else
#include <OpenGL/glu.h>
#endif

namespace cm
{

class GeometryTools;

//////////////////////////////////////////////////////////////////////////

/// derived classes gain triangulation utilites and callbacks.
class UsingTriangulator
{
public:
	UsingTriangulator()
	{
		triangulating = false;
	}

protected:

friend class GeometryTools;

	/// triangulated vertex callback
	///
	/// gets call upon triangulation for every new vertex index added by triangulator
	/// \param i Index of generated triangulated vertex
	virtual void	onTriangulatedVertexIndex(int i){};
	
	/// triangulated vertex data callback
	virtual void	onTriangulatedVertex(void * data){};
	
	/// Combine ( pos is position of new vertex, must allocate data and send to outData
	virtual void	onCombine( 	const Vec3 & pos, 
								void *vertex_data[4],
								GLfloat weight[4], 
								void **outData ) {}
	
public:
	/// Begin triangulating polygon
	void	beginTriangulation();
	/// End triangulating polygon
	void	endTriangulation();
	/// Begin polygon contour
	/// Used for complex polygons with holes.
	/// This function is optional and needs to be called inside a begin-endPolygon pair
	void	newContour();

public:
	/// Triangulate a vertex, just the index gets stored
	/// \param v vertex to triangulate
	void		triangulateVertex( const Vec3 & v );

	bool			triangulating;

};


/// Static class for geometry manipulation utilities.
/// use GeometryTools::func(..)
class GeometryTools
{
protected:
	
public:
	/// basis for tangent space
	struct TangentBasis
	{
		TangentBasis(){}
		TangentBasis( const Vec3 & u, const Vec3 & v, const Vec3 & n ) : u(u),v(v),n(n) {}
		Vec3 u; //tangent
		Vec3 v; //binormal
		Vec3 n; //normal
	};

	/// Compute tangent space for geometry
	/// adjusts vertex format if necessary ( in whole vertex buffer ? \todo check it )
	//static bool	computeTangentSpace( Geometry * geom, int uvChannel = 0);

	// Triangulation functions

	/// Begin triangulating polygon
	static void	beginTriangulation( UsingTriangulator * t );

	static void	triangulateVertex( const Vec3 & v );

	/// End triangulating polygon
	static void	endTriangulation();

	/// Begin polygon contour
	/// Used for complex polygons with holes.
	/// This function is optional and needs to be called inside a begin-endPolygon pair
	/// gets called automatically if user doesnt call it
	static void	newContour();

protected:
	static void	initTriangulator();


	 static void CALLBACK tessvertex (void * data)
	{
		if(_curTriangulated)
		{
			int i = *((int*)data);
			_curTriangulated->onTriangulatedVertexIndex(i);
		}
	}

	 static void  CALLBACK combine(	GLdouble coords[3], void
		   							 *vertex_data[4],
				  					GLfloat weight[4], void **outData )

	{
		/*Vec3 c(coords[0],coords[1],coords[2]);
		combined.push_back(c);
		*outData = &combined.back();
		*/
		if(_curTriangulated)
		{
			_curTriangulated->onCombine(Vec3(coords[0],coords[1],coords[2]),vertex_data,weight,outData);
		}

		//printf("Edge\n");
	}


	 static void  CALLBACK tessedge(GLboolean flag)
	{
		//printf("Edge\n");
	}
	
	static int curIndex;
	static std::vector < int > indices;
	static std::vector < Vec3 > combined;
	
	static GLUtesselator  * _tess;
	static UsingTriangulator * _curTriangulated;
	static bool	_contourBegun;
	

};

}