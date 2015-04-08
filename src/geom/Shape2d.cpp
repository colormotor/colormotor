// Code mostly borrwed from LibCinder //
/*
 Copyright (c) 2010, The Barbarian Group
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

#include "Shape2d.h"
#include "cmGeomGfx.h"
#include "Shape.h"
 
using std::vector;

namespace cm {

Shape2d::Shape2d( const Path2d & p )
{
	if( p.size() )
		appendContour(p);
}

Shape2d::Shape2d( const Shape2d & s )
{
	mContours = s.mContours;
}

Shape2d::Shape2d( const Shape & s )
{
    for( int i = 0; i < s.size(); i++ )
        mContours.push_back( Path2d(s[i]) );
}
        

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void Shape2d::moveTo( const Vec2f &p )
{
	if(!mContours.size() || mContours.back().size())
		mContours.push_back( Path2d() ); // only ad path if there aren't contours or current is not started..
	mContours.back().moveTo( p );
}

void Shape2d::lineTo( const Vec2f &p )
{
	mContours.back().lineTo( p );
}

void Shape2d::quadTo( const Vec2f &p1, const Vec2f &p2 )
{
	mContours.back().quadTo( p1, p2 );
}

void Shape2d::curveTo( const Vec2f &p1, const Vec2f &p2, const Vec2f &p3 )
{
	mContours.back().curveTo( p1, p2, p3 );
}

void Shape2d::arc( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward )
{
	mContours.back().arc( center, radius, startRadians, endRadians, forward );
}

void Shape2d::arcTo( const Vec2f &p, const Vec2f &t, float radius )
{
	mContours.back().arcTo( p, t, radius );
}

void Shape2d::close()
{
	mContours.back().close();
}

void Shape2d::scale( const Vec2f &amount, Vec2f scaleCenter )
{
	for( vector<Path2d>::iterator contIt = mContours.begin(); contIt != mContours.end(); ++contIt )
		contIt->scale( amount, scaleCenter );
}


Rectf Shape2d::calcBoundingBox() const
{
	Rectf result( Vec2f::zero(), Vec2f::zero() );

	// find the first point and initialize the result with that
	for( vector<Path2d>::const_iterator contIt = mContours.begin(); contIt != mContours.end(); ++contIt ) {
		if( ! contIt->mPoints.empty() ) {
			result = Rectf( contIt->mPoints[0], contIt->mPoints[0] );
			break;
		}
	}

	// now iterate all the contours and expand the result to include their points
	for( vector<Path2d>::const_iterator contIt = mContours.begin(); contIt != mContours.end(); ++contIt ) {
		result.include( contIt->mPoints );
	}
	
	return result;
}

Rectf Shape2d::calcPreciseBoundingBox() const
{
	Rectf result( Vec2f::zero(), Vec2f::zero() );

	// find the first point and initialize the result with that
	for( vector<Path2d>::const_iterator contIt = mContours.begin(); contIt != mContours.end(); ++contIt ) {
		if( ! contIt->mPoints.empty() ) {
			result = Rectf( contIt->mPoints[0], contIt->mPoints[0] );
			break;
		}
	}

	// now iterate all the contours and expand the result to include their points
	for( vector<Path2d>::const_iterator contIt = mContours.begin(); contIt != mContours.end(); ++contIt ) {
		result.include( (Rectf)contIt->calcPreciseBoundingBox() );
	}
	
	return result;
}

void Shape2d::cleanup(float tol)
{
	vector<Path2d>::iterator it = mContours.begin();
	while(it!=mContours.end())
	{
		Path2d & p = *it;
		if( tol > 0.0 )
			p.cleanup(tol);
		if(p.size()<3)
		{
			if(p.isClosed())
				it = mContours.erase(it);
			else if(p.size()<2)
				it = mContours.erase(it);
            else
                ++it;
		}
		else
			++it;


	}
}

class TestClass
{
public:
	TestClass();
	~TestClass();

	void funtion(int v)
	{

	}
};


bool Shape2d::contains( const Vec2f &pt ) const
{
	int numPathsInside = 0;
	for( vector<Path2d>::const_iterator contIt = mContours.begin(); contIt != mContours.end(); ++contIt ) {
		if( contIt->contains( pt ) )
			numPathsInside++;
	}
	
	return ( numPathsInside % 2 ) == 1;
}

void Shape2d::simplify(float tol)
{
	for( vector<Path2d>::iterator contIt = mContours.begin(); contIt != mContours.end(); ++contIt )
		contIt->simplify(tol);
	
}

void Shape2d::appendShape( const Shape2d &shape )
{
	for( int i = 0; i < shape.getNumContours(); i++ )
		appendContour(shape.getContour(i));
}
	
void Shape2d::transform( const M44 & m )
{
	for( int i = 0; i < getNumContours(); i++ )
		getContour(i).transform(m);
}

void Shape2d::transform( const M33 & m )
{
	for( int i = 0; i < getNumContours(); i++ )
		getContour(i).transform(m);
}

void	Shape2d::draw()
{
	gfx::draw(*this);
}

void	Shape2d::drawSolid()
{
	gfx::drawSolid(*this);
}
	
} // namespace cinder
