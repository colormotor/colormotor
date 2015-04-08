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

#pragma once

#include "cmMath.h"
#include "Path2d.h"

#include <vector>

namespace cm {

class Shape;

class Shape2d {
  public:
	Shape2d() {}
	Shape2d( const Path2d & p );
	Shape2d( const Shape2d & s );
	Shape2d( const Shape & s );
	
	~Shape2d() {}
	
	void	moveTo( const Vec2f &p );
	void	moveTo( float x, float y ) { moveTo( Vec2f( x, y ) ); }
	void	lineTo( const Vec2f &p );
	void	lineTo( float x, float y ) { lineTo( Vec2f( x, y ) ); }
	void	quadTo( const Vec2f &p1, const Vec2f &p2 );
	void	quadTo( float x1, float y1, float x2, float y2 ) { quadTo( Vec2f( x1, y1 ), Vec2f( x2, y2 ) ); }
	void	curveTo( const Vec2f &p1, const Vec2f &p2, const Vec2f &p3 );
	void	curveTo( float x1, float y1, float x2, float y2, float x3, float y3 ) { curveTo( Vec2f( x1, y1 ), Vec2f( x2, y2 ), Vec2f( x3, y3 ) ); }
	void	arc( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward = true );
	void	arc( float centerX, float centerY, float radius, float startRadians, float endRadians, bool forward = true ) { arc( Vec2f( centerX, centerY ), radius, startRadians, endRadians, forward ); }
	void	arcTo( const Vec2f &p, const Vec2f &t, float radius );
	void	arcTo( float x, float y, float tanX, float tanY, float radius) { arcTo( Vec2f( x, y ), Vec2f( tanX, tanY ), radius ); }
	void	close();
	
	bool	empty() const { return mContours.empty(); }
	void	clear() { mContours.clear(); }
	size_t	getNumContours() const { return mContours.size(); }
	
	// tolerance of 0 cleans up only 0 sized paths
	void cleanup( float tol = 0.0 );
	
	// TODO use M33!
	void			transform( const M44 & m );
	void			transform( const M33 & m );

	const Path2d & operator [] ( int index ) const { return mContours[index]; }
	Path2d & operator [] ( int index )  { return mContours[index]; }
	/// Hack to be able to use same functions with vector and path...
	void	push_back( const Path2d & v ) { appendContour(v); }
	
	size_t	size() const { return mContours.size(); }
	const Path2d & last() const { return mContours.back(); }
	
	
	
	
	const Path2d&				getContour( size_t i ) const { return mContours[i]; }
	Path2d&						getContour( size_t i ) { return mContours[i]; }
	const std::vector<Path2d>&	getContours() const { return mContours; }
	void setContour( int i, const Path2d & path ) { mContours[i] = path; }
	Path2d&						getCurContour() { return mContours.back(); }
	const Path2d&				getCurContour() const { return mContours.back(); }
	
	const Vec2f&	getCurrentPoint() const { return mContours.back().getCurrentPoint(); }
	
	Path2d & appendContour() { mContours.push_back( Path2d() ); return mContours.back(); }
	
    void			appendContour( const Path2d &contour ) { if(contour.size()) mContours.push_back( contour ); }
	void			appendShape( const Shape2d &shape );
	void 			append( const Path2d & contour ) { appendContour(contour); }
	void 			append( const Shape2d & shape ) { appendShape(shape); }
	
	void			removeContour( size_t i ) { mContours.erase( mContours.begin() + i ); }

	//! Scales the Shape2d by \a amount.x on X and \a amount.y on Y around the center \a scaleCenter
	void		scale( const Vec2f &amount, Vec2f scaleCenter = Vec2f::zero() );

	//! Returns the bounding box of the Shape's control points. Note that this is not necessarily the bounding box of the path's shape.
	Rectf	calcBoundingBox() const;
	//! Returns the precise bounding box of the Shape's curves. Slower to calculate than calcBoundingBox().
	Rectf	calcPreciseBoundingBox() const;

	//! Returns whether the point \a pt is contained within the boundaries of the shape
	bool	contains( const Vec2f &pt ) const;

	void simplify(float tol);
	
	void	draw();
	void	drawSolid();
	
	
	//! Iterates all of the contours and points of a Shape2d.
	/** Expects a template parameter that implements
		\code bool operator()( Path2d::SegmentType type, Vec2f *points, Vec2f *previousPoint ) \endcode.
		Functor should return false to cease iteration.
		A \a SegmentType of type \c CLOSE receives &mPoints[0] for its \a points parameters. **/
	template<typename IT>
	void iterate( IT &it )
	{
		bool stop = false;
		for( std::vector<Path2d>::const_iterator contourIt = mContours.begin(); contourIt != mContours.end(); ++contourIt ) {
			size_t pt = 0;
			it( Path2d::MOVETO, &contourIt->mPoints[0], 0 );
			pt++;
			for( std::vector<Path2d::SegmentType>::const_iterator segIt = contourIt->mSegments.begin(); segIt != contourIt->mSegments.end(); ++segIt ) {
				if( *segIt == Path2d::CLOSE )
					it( *segIt, &contourIt->mPoints[0], &contourIt->mPoints[pt-1] );
				else if( ! it( *segIt, &contourIt->mPoints[pt], ( pt > 0 ) ? &contourIt->mPoints[pt-1] : 0 ) ) {
					stop = true;
					break;
				}
				pt += Path2d::sSegmentTypePointCounts[*segIt];
			}
			if( stop ) break;
			/*else if( contourIt->isClosed() ) {
				if( ! it( Path2d::CLOSE, contourIt->empty() ? NULL : &contourIt->mPoints[0], ( pt > 0 ) ? &contourIt->mPoints[pt-1] : 0 ) )
					break;
			}*/
		}
	}
	
  private:
	std::vector<Path2d>	mContours;
};


} // namespace cinder
