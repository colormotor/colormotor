/*
 *  cmGeometryUtils.cpp
 *  colormotor
 *
 *  Created by Daniel Berio on 6/26/09.
 *  Copyright 2009 http://www.enist.org. All rights reserved.
 *
 */


#include "cmGeometryUtils.h"

namespace cm
{

///////////////////////////////////////////////////////////////////////////

void UsingTriangulator::beginTriangulation()
{
	triangulating = true;
	GeometryTools::beginTriangulation(this);
}

void UsingTriangulator::endTriangulation()
{
	GeometryTools::endTriangulation();
	triangulating = false;
}

void UsingTriangulator::newContour()
{
	GeometryTools::newContour();
}

void UsingTriangulator::triangulateVertex( const Vec3 & v )
{
	GeometryTools::triangulateVertex(v);
}



///////////////////////////////////////////////////////////////////////////

GLUtesselator  * GeometryTools::_tess = 0;
UsingTriangulator * GeometryTools::_curTriangulated = 0;
bool GeometryTools::_contourBegun = false;
int GeometryTools::curIndex;
std::vector < int > GeometryTools::indices;
std::vector < Vec3 > GeometryTools::combined;

///////////////////////////////////////////////////////////////////////////
void errorCallback(GLenum errorCode)
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   debugPrint("GLU ERROR: %s\n",estring);
}


void GeometryTools::beginTriangulation(UsingTriangulator * t)
{
	indices.clear();
	combined.clear();
	
	curIndex = 0;
	_curTriangulated = t;
	_contourBegun = false;
	if(_tess == 0)
		initTriangulator();

	gluTessBeginPolygon(_tess, NULL); 
	
}


void GeometryTools::endTriangulation()
{
	if(_contourBegun) // end contour if has been started
		gluTessEndContour(_tess); 

	gluTessEndPolygon(_tess);
	_curTriangulated = 0;
	
	gluDeleteTess( _tess);
	_tess = 0;
}



void GeometryTools::newContour()
{
	if(_contourBegun) // end contour if has been started
		gluTessEndContour(_tess); 
	
	gluTessBeginContour(_tess);
	_contourBegun = true;
}


void GeometryTools::initTriangulator()
{
	_tess = gluNewTess();
	//gluTessProperty(tess,GLU_TESS_BOUNDARY_ONLY ,true);
	gluTessCallback(_tess,GLU_TESS_VERTEX,(void(CALLBACK*)())GeometryTools::tessvertex);
	gluTessCallback(_tess,GLU_TESS_EDGE_FLAG,(void(CALLBACK*)())GeometryTools::tessedge);
	gluTessCallback(_tess,GLU_TESS_COMBINE,(void(CALLBACK*)())GeometryTools::combine);
	gluTessCallback(_tess, GLU_TESS_ERROR,(GLvoid (CALLBACK*) ()) &errorCallback);

}

void GeometryTools::triangulateVertex( const Vec3 & v )
{
	if(!_contourBegun)
	{
		newContour(); // begin contour if it hasnt been started yet
	}
	
	indices.push_back(curIndex);
	curIndex++;
	double vd[3] = {(double)v.x,(double)v.y,(double)v.z};
	gluTessVertex(_tess, vd,&indices.back()); 

}


}