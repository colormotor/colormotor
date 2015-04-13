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
 
/// Adapted from Cinder code.
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
#include "cmCore.h"
#include <vector>

namespace cm {

class Contour;

class Path2d {
 public:
	Path2d() 
	:
	mCurved(false)
	{}

	Path2d(const Path2d & mom);
	// For numpy compatibility
	Path2d( float * buf, int n, int n2, bool closed=false );
	Path2d( const Contour & ctr );

	~Path2d() {}
	
	
	//! Sets the start point of the path to \a p. This is the only legal first command, and only legal as the first command.
	void	moveTo( const Vec2f &p );
	//! Sets the start point of the path to (\a x, \a y ). This is the only legal first command, and only legal as the first command.
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
	void	addPoint( float x, float y );
	void	addPoint( const Vec2f & p );
	
	// TODO!
	std::vector<Path2d> split( int index, float t );
	
	//! Closes the path, by drawing a straight line from the first to the last point. This is only legal as the last command.
	void	close() { mSegments.push_back( CLOSE ); }
	bool	isClosed() const { return ( mSegments.size() > 1 ) && mSegments.back() == CLOSE; }
    
	void addPoints( const std::vector<Vec2> & vec );
	
	
	const Vec2f & operator [] ( int index ) const { return mPoints[index]; }
	Vec2f & operator [] ( int index )  { return mPoints[index]; }
	/// Hack to be able to use same functions with vector and path...
	void	push_back( const Vec2f & v ) { addPoint(v); }
	
	//! Reverses the order of the path's points, inverting its winding order
    void	reverse();
	
	bool	empty() const { return mPoints.empty(); }
	void	clear() { mSegments.clear(); mPoints.clear(); mCurved=false; }
	size_t	getNumSegments() const { return mSegments.size(); }
	size_t	getNumPoints() const { return mPoints.size(); }
	size_t	size() const { return mPoints.size(); }
	const Vec2f & last() const { return mPoints.back(); }
	
	// check if path contains NaNs
	bool hasNans() const;
	
	Vec2f getCentroid() const;
	
	//! Returns the point on the curve at parameter \a t, which lies in the range <tt>[0,1]</tt>
	Vec2f	getPosition( float t ) const;
	//! Returns the point in segment # \a segment in the range <tt>[0,getNumSegments())</tt> at parameter \a t in the range <tt>[0,1]</tt>
	Vec2f	getSegmentPosition( size_t segment, float t ) const;
	
	std::vector<Vec2f>	subdivide( float approximationScale = 1.0f ) const;
	void subdivideCurves( float approximationScale = 1.0f );
	
	//! Scales the Path2d by \a amount.x on X and \a amount.y on Y around the center \a scaleCenter
	void		scale( const Vec2f &amount, Vec2f scaleCenter = Vec2f::zero() );

	const std::vector<Vec2f>&	getPoints() const { return mPoints; }
	std::vector<Vec2f>&			getPoints() { return mPoints; }
	const Vec2f&				getPoint( size_t point ) const { return mPoints[point]; }
	Vec2f&						getPoint( size_t point ) { return mPoints[point]; }
	const Vec2f&				getCurrentPoint() const { return mPoints.back(); }
	void						setPoint( size_t index, const Vec2f &p ) { mPoints[index] = p; }
	void insertPoint( int index, const Vec2 & p );
	
	// TODO use M33!
	void			transform( const M44 & m );
	void			transform( const M33 & m );

	void			removeSegment( size_t segment );

	enum SegmentType { MOVETO, LINETO, QUADTO, CUBICTO, CLOSE };
	static const int sSegmentTypePointCounts[];
	SegmentType		getSegmentType( size_t segment ) const { return mSegments[segment]; }
	
	//! Returns the bounding box around all control points. As with Shape2d, note this is not necessarily the bounding box of the Path's shape.
	Rectf	calcBoundingBox() const;
	//! Returns the precise bounding box around the curve itself. Slower to calculate than calcBoundingBox().
	Rectf	calcPreciseBoundingBox() const;	
	//! Returns the approximate length of the whole path
	float  calcLength() const;
	
	static Path2d makeRect( float x, float y, float w, float h );
	
	//! Returns whether the point \a pt is contained within the boundaries of the path
	bool	contains( const Vec2f &pt ) const;

	void	simplify(float tol);
	void append( const Path2d & p );
	void cleanup( float tol );
	
	void	draw();
	void	drawSolid();
	
	// removes initial points to achieve given size
	void	limitSize( int len );
	
	friend class Shape2d;
	friend std::ostream& operator<<( std::ostream &out, const Path2d &p );
  private:
	void	simplifyDP(float tol, Vec2* v, int j, int k, int* mk );
		
	void	arcHelper( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward );
	void	arcSegmentAsCubicBezier( const Vec2f &center, float radius, float startRadians, float endRadians );
	void	subdivideQuadratic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, int level, std::vector<Vec2f> *result ) const;
	void	subdivideCubic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4, int level, std::vector<Vec2f> *result ) const;

	bool mCurved;
	std::vector<Vec2f>			mPoints;
	std::vector<SegmentType>	mSegments;
	
};


inline std::ostream& operator<<( std::ostream &out, const Path2d &p )
{
	size_t pt = 0;
	for( size_t s = 0; s < p.mSegments.size(); ++s ) {
		if( p.mSegments[s] == Path2d::MOVETO ) {
			out << "M " << p.mPoints[pt].x << " " << p.mPoints[pt].y << " ";
			pt++;
		}
		if( p.mSegments[s] == Path2d::LINETO ) {
			out << "L " << p.mPoints[pt].x << " " << p.mPoints[pt].y << " ";
			pt++;
		}
		else if( p.mSegments[s] == Path2d::QUADTO ) {
			out << "Q " << p.mPoints[pt].x << " " << p.mPoints[pt].y << " " << p.mPoints[pt+1].x << " " << p.mPoints[pt+1].y << " ";
			pt += 2;
		}
		else if( p.mSegments[s] == Path2d::CUBICTO ) {
			out << "C " << p.mPoints[pt].x << " " << p.mPoints[pt].y << " " << p.mPoints[pt+1].x << " " << p.mPoints[pt+1].y << " " << p.mPoints[pt+2].x << " " << p.mPoints[pt+2].y << " ";
			pt += 3;
		}
		else {
			out << "Z ";
		}

	}
	
	return out;
}

} // namespace cinder
