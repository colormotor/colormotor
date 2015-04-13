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
 Copyright (c) 2010, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Portions Copyright (c) 2004, Laminar Research.

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


#include "Path2d.h"
#include "cmGeomGfx.h"
#include <algorithm>
#include "Shape.h"

using std::vector;

namespace cm {

const int Path2d::sSegmentTypePointCounts[] = { 0, 1, 2, 3, 0 };


Path2d::Path2d(const Contour & mom)
{
	for( int i = 0; i < mom.size(); i++ )
	{
		addPoint(mom[i]);
	}

	if(mom.isClosed())
		close();
}

Path2d::Path2d( float * buf, int n, int n2, bool closed )
{
	mCurved = false;
	
	if(n2!=2)
	{
		printf("Can't convert numpy array to path2d, DIM2 is %d\n",n2);
		return;
	}

	mPoints.assign(n, Vec2(0,0));
	for( int i = 0; i < n; i++ )
	{
		mPoints[i].x = buf[(i<<1)];
		mPoints[i].y = buf[(i<<1)+1];
	}

	mSegments.assign(n-1, LINETO);

	if(closed)
		mSegments.push_back(CLOSE);
}

Path2d::Path2d(const Path2d & mom)
{
	mCurved = mom.mCurved;
	mPoints = mom.mPoints;
	mSegments = mom.mSegments;
}
void Path2d::moveTo( const Vec2f &p )
{
	if( ! mPoints.empty() )
		assert(0);
		
	mPoints.push_back( p );
}

void Path2d::addPoint( float x, float y )
{	
	addPoint(Vec2(x,y));
}

bool Path2d::hasNans() const
{
	for( int i = 0; i < mPoints.size(); i++ )
	{
		const Vec2 & p = mPoints[i];
        if( std::isnan(p.x) || std::isnan(p.y) )
			return true;
	}
	
	return false;
}

void Path2d::addPoint( const Vec2f &p )
{
	if(mPoints.size())
	{
		mSegments.push_back( LINETO );
	}

	mPoints.push_back( p );
}

void Path2d::insertPoint( int index, const Vec2 & p )
{
	if(index >= mPoints.size() || mPoints.size() == 0 )
		addPoint(p);

	mPoints.insert(mPoints.begin()+index,p);
	mSegments.insert(mSegments.begin()+index,LINETO);
}

void Path2d::addPoints( const std::vector<Vec2> & vec )
{
	for( int i = 0; i < vec.size(); i++ )
		addPoint(vec[i]);
}
	
void Path2d::lineTo( const Vec2f &p )
{
	addPoint(p);
}

void Path2d::quadTo( const Vec2f &p1, const Vec2f &p2 )
{
	// TODO lengths!

	if( mPoints.empty() )
		return;
	mCurved = true;
	mPoints.push_back( p1 );
	mPoints.push_back( p2 );
	mSegments.push_back( QUADTO );
}

void Path2d::curveTo( const Vec2f &p1, const Vec2f &p2, const Vec2f &p3 )
{
	if( mPoints.empty() )
		return;
	mCurved = true;
	mPoints.push_back( p1 );
	mPoints.push_back( p2 );
	mPoints.push_back( p3 );	
	mSegments.push_back( CUBICTO );
}

void Path2d::arc( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward )
{
	if( forward ) 
	{
		while( endRadians < startRadians )
			endRadians += 2 * PI;
	}
	else
	{
		while( endRadians > startRadians )
			endRadians -= 2 * PI;
	}

	if( mPoints.empty() )
		moveTo( center + Vec2f( cos( startRadians ), sin( startRadians ) )*radius );
	else {
		lineTo( center + Vec2f( cos( startRadians ), sin( startRadians ) )*radius );
	}

	if( forward )
		arcHelper( center, radius, startRadians, endRadians, forward );
	else
		arcHelper( center, radius, endRadians, startRadians, forward );
}

void Path2d::arcHelper( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward )
{
	// wrap the angle difference around to be in the range [0, 4*pi]
    while( endRadians - startRadians > 4 * PI )
		endRadians -= 2 * PI;

    // Recurse if angle delta is larger than PI
    if( endRadians - startRadians > PI ) {
		float midRadians = startRadians + (endRadians - startRadians) * 0.5f;
		if( forward ) {
			arcHelper( center, radius, startRadians, midRadians, forward );
			arcHelper( center, radius, midRadians, endRadians, forward );
		}
		else {
			arcHelper( center, radius, midRadians, endRadians, forward );
			arcHelper( center, radius, startRadians, midRadians, forward );
		}
    } 
	else if( fabs( endRadians - startRadians ) > 0.000001f ) {
		int segments = static_cast<int>( ceil( fabs( endRadians - startRadians ) / ( PI / 2.0f ) ) );
		float angle;
		float angleDelta = ( endRadians - startRadians ) / (float)segments;
		if( forward )
			angle = startRadians;
		else {
			angle = endRadians;
			angleDelta = -angleDelta;
		}

		for( int seg = 0; seg < segments; seg++, angle += angleDelta ) {
			arcSegmentAsCubicBezier( center, radius, angle, angle + angleDelta );
		}
    }	
}

void Path2d::arcSegmentAsCubicBezier( const Vec2f &center, float radius, float startRadians, float endRadians )
{
	float r_sin_A, r_cos_A;
	float r_sin_B, r_cos_B;
	float h;

	r_sin_A = radius * sin( startRadians );
	r_cos_A = radius * cos( startRadians );
	r_sin_B = radius * sin( endRadians );
	r_cos_B = radius * cos( endRadians );

	h = 4.0f/3.0f * tan( (endRadians - startRadians) / 4 );

	curveTo( center.x + r_cos_A - h * r_sin_A, center.y + r_sin_A + h * r_cos_A, center.x + r_cos_B + h * r_sin_B,
				center.y + r_sin_B - h * r_cos_B, center.x + r_cos_B, center.y + r_sin_B );
}

// Implementation courtesy of Lennart Kudling
void Path2d::arcTo( const Vec2f &p1, const Vec2f &t, float radius )
{
	if( isClosed() || empty() )
			return;

	const float epsilon = 1e-8;
	
	// Get current point.
	const Vec2f& p0 = getCurrentPoint();
	
	// Calculate the tangent vectors tangent1 and tangent2.
	const Vec2f p0t = p0 - t;
	const Vec2f p1t = p1 - t;
	
	// Calculate tangent distance squares.
	const float p0tSquare = p0t.squareLength();
	const float p1tSquare = p1t.squareLength();
	
	// Calculate tan(a/2) where a is the angle between vectors tangent1 and tangent2.
	//
	// Use the following facts:
	//
	//  p0t * p1t  = |p0t| * |p1t| * cos(a) <=> cos(a) =  p0t * p1t  / (|p0t| * |p1t|)
	// |p0t x p1t| = |p0t| * |p1t| * sin(a) <=> sin(a) = |p0t x p1t| / (|p0t| * |p1t|)
	//
	// and
	//
	// tan(a/2) = sin(a) / ( 1 - cos(a) )
	
	const float numerator = p0t.y * p1t.x - p1t.y * p0t.x;
	const float denominator = sqrt( p0tSquare * p1tSquare ) - ( p0t.x * p1t.x + p0t.y * p1t.y );
	
	// The denominator is zero <=> p0 and p1 are colinear.
	if( fabs( denominator ) < epsilon ) {
		lineTo( t );
	}
	else {
		// |b0 - t| = |b3 - t| = radius * tan(a/2).
		const float distanceFromT = fabs( radius * numerator / denominator );
		
		// b0 = t + |b0 - t| * (p0 - t)/|p0 - t|.
		const Vec2f b0 = t + distanceFromT * p0t.normalized();
		
		// If b0 deviates from p0, add a line to it.
		if( fabs(b0.x - p0.x) > epsilon || fabs(b0.y - p0.y) > epsilon ) {
			lineTo( b0 );
		}
		
		// b3 = t + |b3 - t| * (p1 - t)/|p1 - t|.
		const Vec2f b3 = t +  distanceFromT * p1t.normalized();
		
		// The two bezier-control points are located on the tangents at a fraction
		// of the distance[ tangent points <-> tangent intersection ].
		// See "Approxmiation of a Cubic Bezier Curve by Circular Arcs and Vice Versa" by Aleksas Riskus 
		// http://itc.ktu.lt/itc354/Riskus354.pdf
		
		double b0tSquare = (t.x - b0.x) *  (t.x - b0.x) + (t.y - b0.y) *  (t.y - b0.y);
		double radiusSquare = radius * radius;
		double fraction;
		
		// Assume dist = radius = 0 if the radius is very small.
		if( fabs( radiusSquare / b0tSquare ) < epsilon )
			fraction = 0.0;
		else
			fraction = ( 4.0 / 3.0 ) / ( 1.0 + sqrt( 1.0 + b0tSquare / radiusSquare ) );
		
		const Vec2f b1 = b0 + fraction * (t - b0);
		const Vec2f b2 = b3 + fraction * (t - b3);
		
		curveTo( b1, b2, b3 );
	}
}
    
void Path2d::reverse()
{
    // The path is empty: nothing to do.
    if( empty() )
        return;
    
    // Reverse all points.
    std::reverse( mPoints.begin(), mPoints.end() );

    // Reverse the segments, but skip the "moveto" and "close":
	if( isClosed() ) {
        // There should be at least 4 segments: "moveto", "close" and two other segments.
        if( mSegments.size() > 3 )
            std::reverse( mSegments.begin() + 1, mSegments.end() - 1 );
    }
    else {
        // There should be at least 3 segments: "moveto" and two other segments.
        if( mSegments.size() > 2 )
            std::reverse( mSegments.begin() + 1, mSegments.end() );
    }

}

void Path2d::removeSegment( size_t segment )
{
	int firstPoint = 1; // we always skip the first point, since it's a moveTo
	for( size_t s = 0; s < segment; ++s )
		firstPoint += sSegmentTypePointCounts[mSegments[s]];

	int pointCount = sSegmentTypePointCounts[mSegments[segment]];
	mPoints.erase( mPoints.begin() + firstPoint, mPoints.begin() + firstPoint + pointCount );
	mSegments.erase( mSegments.begin() + segment );
}

Vec2f Path2d::getPosition( float t ) const
{
	if( t <= 0 )
		return mPoints[0];
	else if( t >= 1 )
		return mPoints.back();
	
	size_t totalSegments = mSegments.size();
	float segParamLength = 1.0f / totalSegments; 
	size_t seg = t * totalSegments;
	float subSeg = ( t - seg * segParamLength ) / segParamLength;
	
	return getSegmentPosition( seg, subSeg );
}

Vec2f Path2d::getSegmentPosition( size_t segment, float t ) const
{
	size_t firstPoint = 0;
	for( size_t s = 0; s < segment; ++s )
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	switch( mSegments[segment] ) {
		case CUBICTO: {
			float t1 = 1 - t;
			return mPoints[firstPoint]*(t1*t1*t1) + mPoints[firstPoint+1]*(3*t*t1*t1) + mPoints[firstPoint+2]*(3*t*t*t1) + mPoints[firstPoint+3]*(t*t*t);
		}
		break;
		case QUADTO: {
			float t1 = 1 - t;
			return mPoints[firstPoint]*(t1*t1) + mPoints[firstPoint+1]*(2*t*t1) + mPoints[firstPoint+2]*(t*t);
		}
		break;
		case LINETO: {
			float t1 = 1 - t;
			return mPoints[firstPoint]*t1 + mPoints[firstPoint+1]*t;
		}
		break;
		case CLOSE: {
			float t1 = 1 - t;
			return mPoints[firstPoint]*t1 + mPoints[0]*t;
		}
		break;
			
		default:
			return Vec2f(0,0);
	}
}


vector<Vec2f> Path2d::subdivide( float approximationScale ) const
{
	if( mSegments.empty() )
		return vector<Vec2f>();

	float distanceToleranceSqr = 0.5f / approximationScale;
	distanceToleranceSqr *= distanceToleranceSqr;
	
	size_t firstPoint = 0;
	vector<Vec2f> result;
	result.push_back( mPoints[0] );
	for( size_t s = 0; s < mSegments.size(); ++s ) {
		switch( mSegments[s] ) {
			case CUBICTO:
				result.push_back( mPoints[firstPoint] );
				subdivideCubic( distanceToleranceSqr, mPoints[firstPoint], mPoints[firstPoint+1], mPoints[firstPoint+2], mPoints[firstPoint+3], 0, &result );
				result.push_back( mPoints[firstPoint+3] );
			break;
			case QUADTO:
				result.push_back( mPoints[firstPoint] );
				subdivideQuadratic( distanceToleranceSqr, mPoints[firstPoint], mPoints[firstPoint+1], mPoints[firstPoint+2], 0, &result );
				result.push_back( mPoints[firstPoint+2] );
			break;
			case LINETO:
				result.push_back( mPoints[firstPoint] );
				result.push_back( mPoints[firstPoint+1] );
			break;
			case CLOSE:
				result.push_back( mPoints[firstPoint] );
				result.push_back( mPoints[0] );
			break;
			default:
				break;
		}
		
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}
	
	return result;
}

void Path2d::subdivideCurves(  float approximationScale )
{
	bool closed = isClosed();
	vector<Vec2f> pts = subdivide(approximationScale);
	clear();
	for( int i = 0; i < pts.size(); i++ )
		addPoint(pts[i]);
	if( closed )
		close();
}


// This technique is due to Maxim Shemanarev but removes his tangent error estimates
void Path2d::subdivideQuadratic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, int level, vector<Vec2f> *result ) const
{
	const int recursionLimit = 17;
	const float collinearEpsilon = 0.0000001f;
	
	if( level > recursionLimit ) 
		return;

	Vec2f p12 = ( p1 + p2 ) * 0.5f;
	Vec2f p23 = ( p2 + p3 ) * 0.5f;
	Vec2f p123 = ( p12 + p23 ) * 0.5f;

	float dx = p3.x - p1.x;
	float dy = p3.y - p1.y;
	float d = fabs(((p2.x - p3.x) * dy - (p2.y - p3.y) * dx));

	if( d > collinearEpsilon ) { 
		if( d * d <= distanceToleranceSqr * (dx*dx + dy*dy) ) {
			result->push_back( p123 );
			return;
		}
	}
	else { // Collinear case
		float da = dx * dx + dy * dy;
		if( da == 0 ) {
			d = p1.squareDistance( p2 );
		}
		else {
			d = ((p2.x - p1.x)*dx + (p2.y - p1.y)*dy) / da;
			if( d > 0 && d < 1 ) {
				// Simple collinear case, 1---2---3 - We can leave just two endpoints
				return;
			}
			
			if(d <= 0)
				d = p2.squareDistance( p1 );
			else if(d >= 1)
				d = p2.squareDistance( p3 );
			else
				d = p2.squareDistance( Vec2f( p1.x + d * dx, p1.y + d * dy ) );
		}
		if( d < distanceToleranceSqr ) {
			result->push_back( p2 );
			return;
		}
	}

	// Continue subdivision
	subdivideQuadratic( distanceToleranceSqr, p1, p12, p123, level + 1, result ); 
	subdivideQuadratic( distanceToleranceSqr, p123, p23, p3, level + 1, result );
}

// This technique is due to Maxim Shemanarev but removes his tangent error estimates
void Path2d::subdivideCubic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4, int level, vector<Vec2f> *result ) const
{
	const int recursionLimit = 17;
	const float collinearEpsilon = 0.0000001f;
	
	if( level > recursionLimit ) 
		return;
	
	// Calculate all the mid-points of the line segments
	//----------------------

	Vec2f p12 = ( p1 + p2 ) * 0.5f;
	Vec2f p23 = ( p2 + p3 ) * 0.5f;
	Vec2f p34 = ( p3 + p4 ) * 0.5f;
	Vec2f p123 = ( p12 + p23 ) * 0.5f;
	Vec2f p234 = ( p23 + p34 ) * 0.5f;
	Vec2f p1234 = ( p123 + p234 ) * 0.5f;


	// Try to approximate the full cubic curve by a single straight line
	//------------------
	float dx = p4.x - p1.x;
	float dy = p4.y - p1.y;

	float d2 = fabs(((p2.x - p4.x) * dy - (p2.y - p4.y) * dx));
	float d3 = fabs(((p3.x - p4.x) * dy - (p3.y - p4.y) * dx));
	float k, da1, da2;

	switch( (int(d2 > collinearEpsilon) << 1) + int(d3 > collinearEpsilon) ) {
		case 0:
			// All collinear OR p1==p4
			k = dx*dx + dy*dy;
			if( k == 0 ) {
				d2 = p1.squareDistance( p2 );
				d3 = p4.squareDistance( p3 );
			}
			else {
				k   = 1 / k;
				da1 = p2.x - p1.x;
				da2 = p2.y - p1.y;
				d2  = k * ( da1 * dx + da2 * dy );
				da1 = p3.x - p1.x;
				da2 = p3.y - p1.y;
				d3  = k * ( da1 * dx + da2 * dy );
				if( d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1 ) {
					// Simple collinear case, 1---2---3---4
					// We can leave just two endpoints
					return;
				}
					 if(d2 <= 0) d2 = p2.squareDistance( p1 );
				else if(d2 >= 1) d2 = p2.squareDistance( p4 );
				else             d2 = p2.squareDistance( Vec2f( p1.x + d2*dx, p1.y + d2*dy ) );

					 if(d3 <= 0) d3 = p3.squareDistance( p1 );
				else if(d3 >= 1) d3 = p3.squareDistance( p4 );
				else             d3 = p3.squareDistance( Vec2f( p1.x + d3*dx, p1.y + d3*dy ) );
			}
			if(d2 > d3) {
				if( d2 < distanceToleranceSqr ) {
					result->push_back( p2 );
					return;
				}
			}
			else {
				if( d3 < distanceToleranceSqr ) {
					result->push_back( p3 );
					return;
				}
			}
		break;
		case 1:
			// p1,p2,p4 are collinear, p3 is significant
			if( d3 * d3 <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
				result->push_back( p23 );
				return;
			}
		break;
		case 2:
			// p1,p3,p4 are collinear, p2 is significant
			if( d2 * d2 <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
				result->push_back( p23 );
				return;
			}
		break;
		case 3: 
			// Regular case
			if( (d2 + d3)*(d2 + d3) <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
				result->push_back( p23 );
				return;
			}
		break;
	}

	// Continue subdivision
	subdivideCubic( distanceToleranceSqr, p1, p12, p123, p1234, level + 1, result ); 
	subdivideCubic( distanceToleranceSqr, p1234, p234, p34, p4, level + 1, result ); 
}

void Path2d::scale( const Vec2f &amount, Vec2f scaleCenter )
{
	for( vector<Vec2f>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt = scaleCenter + Vec2f( ( ptIt->x - scaleCenter.x ) * amount.x, ( ptIt->y - scaleCenter.y ) * amount.y );
}

Rectf Path2d::calcBoundingBox() const
{
	Rectf result(0,0,0,0);
	if( ! mPoints.empty() )
	{
		result = Rectf( mPoints[0], mPoints[0] );
		result.include( mPoints );
	}
	
	return result;	
}
	
float Path2d::calcLength() const
{
	float l;
	std::vector<Vec2f> pts = subdivide();
	for( int i = 0; i < pts.size()-1; i++ )
	{
		l += cm::distance( pts[i],pts[i+1] );
	}
	return l;
}



// calcPreciseBoundingBox helper routines
namespace {
int	calcQuadraticBezierMonotoneRegions( const Vec2f p[3], float resultT[2] )
{
	int resultIdx = 0;
	float dx = p[0].x - 2 * p[1].x + p[2].x;
	if( dx != 0 ) {
		float t = ( p[0].x - p[1].x ) / dx;
		if( t > 0 && t < 1 )
			resultT[resultIdx++] = t;
	}
	float dy = p[0].y - 2 * p[1].y + p[2].y;
	if( dy != 0 ) {
		float t = ( p[0].y - p[1].y ) / dy;
		if( t > 0 && t < 1 )
			resultT[resultIdx++] = t;
	}
	
	return resultIdx;
}

Vec2f calcQuadraticBezierPos( const Vec2f p[3], float t )
{
	float nt = 1 - t;
	return Vec2f( nt * nt * p[0].x + 2 * nt * t * p[1].x +  t * t * p[2].x, nt * nt * p[0].y + 2 * nt * t * p[1].y +  t * t * p[2].y );
}

int	calcCubicBezierMonotoneRegions( const Vec2f p[4], float resultT[4] )
{
	float Ax = -p[0].x + 3 * p[1].x - 3 * p[2].x + p[3].x;
	float Bx =  3 * p[0].x - 6 * p[1].x + 3 * p[2].x;
	float Cx = -3 * p[0].x + 3 * p[1].x;
	float ax = 3 * Ax;
	float bx = 2 * Bx;
	float cx = Cx;

	float Ay = -p[0].y + 3 * p[1].y - 3 * p[2].y + p[3].y;
	float By =  3 * p[0].y - 6 * p[1].y + 3 * p[2].y;
	float Cy = -3 * p[0].y + 3 * p[1].y;
	float ay = 3 * Ay;
	float by = 2 * By;
	float cy = Cy;

	int resultIdx = 0;
	float r1[2], r2[2];
	int o1 = solveQuadratic( r1, ax, bx, cx );
	int o2 = solveQuadratic( r2, ay, by, cy );

	if( o1 > 0 && r1[0] > 0 && r1[0] < 1 ) resultT[resultIdx++] = r1[0];
	if( o1 > 1 && r1[1] > 0 && r1[1] < 1 ) resultT[resultIdx++] = r1[1];

	if( o2 > 0 && r2[0] > 0 && r2[0] < 1 ) resultT[resultIdx++] = r2[0];
	if( o2 > 1 && r2[1] > 0 && r2[1] < 1 ) resultT[resultIdx++] = r2[1];

	return resultIdx;
}

Vec2f calcCubicBezierPos( const Vec2f p[4], float t )
{
	float nt = 1 - t;
	float w0 = nt * nt * nt;
	float w1 = 3 * nt * nt * t;
	float w2 = 3 * nt * t * t;
	float w3 = t * t * t;
	return Vec2f( w0 * p[0].x + w1 * p[1].x + w2 * p[2].x + w3 * p[3].x, w0 * p[0].y + w1 * p[1].y + w2 * p[2].y + w3 * p[3].y );
}

} // anonymous namespace

Rectf Path2d::calcPreciseBoundingBox() const
{
	if( mPoints.empty() )
		return Rectf();
	else if( mPoints.size() == 1 )
		return Rectf( mPoints[0], mPoints[0] );
	else if( mPoints.size() == 2 )
		return Rectf( mPoints[0], mPoints[1] );
	
	Rectf result( mPoints[0], mPoints[0] );
	size_t firstPoint = 0;
	for( size_t s = 0; s < mSegments.size(); ++s ) 
	{
		switch( mSegments[s] ) 
		{
			case CUBICTO: {
				float monotoneT[4];
				int monotoneCnt = calcCubicBezierMonotoneRegions( &(mPoints[firstPoint]), monotoneT );
				for( int monotoneIdx = 0; monotoneIdx < monotoneCnt; ++monotoneIdx )
				result.include( calcCubicBezierPos( &(mPoints[firstPoint]), monotoneT[monotoneIdx] ) );
				result.include( mPoints[firstPoint+0] );
				result.include( mPoints[firstPoint+3] );
			}
			break;
			case QUADTO: {
				float monotoneT[2];
				int monotoneCnt = calcQuadraticBezierMonotoneRegions( &(mPoints[firstPoint]), monotoneT );
				for( int monotoneIdx = 0; monotoneIdx < monotoneCnt; ++monotoneIdx )
					result.include( calcQuadraticBezierPos( &(mPoints[firstPoint]), monotoneT[monotoneIdx] ) );
				result.include( mPoints[firstPoint+0] );
				result.include( mPoints[firstPoint+2] );
			}
			break;
			case LINETO:
				result.include( mPoints[firstPoint] );
				result.include( mPoints[firstPoint+1] );
			break;
			case CLOSE:
			break;
			default:
				break;
				//throw Path2dExc();
		}
		
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}
	
	return result;
}

float linearYatX( const Vec2f p[2], float x )
{
	if( p[0].x == p[1].x ) 	return p[0].y;
	return p[0].y + (p[1].y - p[0].y) * (x - p[0].x) / (p[1].x - p[0].x);
}

size_t linearCrossings( const Vec2f p[2], const Vec2f &pt )
{
	if( (p[0].x < pt.x && pt.x <= p[1].x ) ||
		(p[1].x < pt.x && pt.x <= p[0].x )) {
		if( pt.y > linearYatX( p, pt.x ) )
			return 1;
	}
	return 0;
}

size_t cubicBezierCrossings( const Vec2f p[4], const Vec2f &pt )
{
	float Ax =     -p[0].x + 3 * p[1].x - 3 * p[2].x + p[3].x;
	float Bx =  3 * p[0].x - 6 * p[1].x + 3 * p[2].x;
	float Cx = -3 * p[0].x + 3 * p[1].x;
	float Dx =		p[0].x - pt.x;

	float Ay =     -p[0].y + 3 * p[1].y - 3 * p[2].y + p[3].y;
	float By =  3 * p[0].y - 6 * p[1].y + 3 * p[2].y;
	float Cy = -3 * p[0].y + 3 * p[1].y;
	float Dy =		p[0].y;

	float roots[3];
	int numRoots = solveCubic( roots, Ax, Bx, Cx, Dx );

	if( numRoots < 1)
		return 0;

	int result = 0;
	for( int n = 0; n < numRoots; ++n )
		if( roots[n] > 0 && roots[n] < 1 )
			if( Ay * roots[n] * roots[n] * roots[n] + By * roots[n] * roots[n] + Cy * roots[n] + Dy < pt.y )
				++result;
	
	return result;
}

size_t quadraticBezierCrossings( const Vec2f p[3], const Vec2f &pt )
{
	float Ax = 1.0f * p[0].x - 2.0f * p[1].x + 1.0f * p[2].x;
	float Bx = -2.0f * p[0].x + 2.0f * p[1].x;
	float Cx = 1.0f * p[0].x - pt.x;

	float Ay = 1.0f * p[0].y - 2.0f * p[1].y + 1.0f * p[2].y;
	float By = -2.0f * p[0].y + 2.0f * p[1].y;
	float Cy = 1.0f * p[0].y;

	float roots[2];
	int numRoots = solveQuadratic( roots, Ax, Bx, Cx );

	if( numRoots < 1)
		return 0;

	int result = 0;
	for( int n = 0; n < numRoots; ++n )
		if (roots[n] > 0 && roots[n] < 1 )
			if( Ay * roots[n] * roots[n] + By * roots[n] + Cy < pt.y )
				++result;
	
	return result;
}


bool Path2d::contains( const Vec2f &pt ) const
{
	if( mPoints.size() <= 2 )
		return false;

	size_t firstPoint = 0;
	size_t crossings = 0;
	for( size_t s = 0; s < mSegments.size(); ++s ) {
		switch( mSegments[s] ) {
			case CUBICTO:
				crossings += cubicBezierCrossings( &(mPoints[firstPoint]), pt );
			break;
			case QUADTO:
				crossings += quadraticBezierCrossings( &(mPoints[firstPoint]), pt );
			break;
			case LINETO:
				crossings += linearCrossings( &(mPoints[firstPoint]), pt );
			break;
			case CLOSE: // ignore - we always assume closed
			break;
			default:
				;//throw Path2dExc();
		}
		
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}

	Vec2f temp[2];
	temp[0] = mPoints[mPoints.size()-1];
	temp[1] = mPoints[0];
	crossings += linearCrossings( &(temp[0]), pt );
	
	return (crossings & 1) == 1;
}


// local defs for simplify

typedef struct{
		Vec2f P0;
		Vec2f P1;
}Segment;
	
#define norm2(v)   dot(v,v)        // norm2 = squared length of vector
#define norm(v)    sqrt(norm2(v))  // norm = length of vector
#define d2(u,v)    norm2(u-v)      // distance squared = norm2 of difference
#define d(u,v)     norm(u-v)       // distance = norm of difference


void  Path2d::simplifyDP(float tol, Vec2f* v, int j, int k, int* mk )
{
	if (k <= j+1) // there is nothing to simplify
		return;

	// check for adequate approximation by segment S from v[j] to v[k]
	int     maxi	= j;          // index of vertex farthest from S
	float   maxd2	= 0;         // distance squared of farthest vertex
	float   tol2	= tol * tol;  // tolerance squared
	Segment S		= {v[j], v[k]};  // segment from v[j] to v[k]
	Vec2 u;
	u				= S.P1 - S.P0;   // segment direction vector
	double  cu		= dot(u,u);     // segment length squared

	// test each vertex v[i] for max distance from S
	// compute using the Feb 2001 Algorithm's dist_ofPoint_to_Segment()
	// Note: this works in any dimension (2D, 3D, ...)
	Vec2  w;
	Vec2   Pb;                // base of perpendicular from v[i] to S
	float  b, cw, dv2;        // dv2 = distance v[i] to S squared

	for (int i=j+1; i<k; i++){
		// compute distance squared
		w = v[i] - S.P0;
		cw = dot(w,u);
		if ( cw <= 0 ) dv2 = d2(v[i], S.P0);
		else if ( cu <= cw ) dv2 = d2(v[i], S.P1);
		else {
			b = (float)(cw / cu);
			Pb = S.P0 + u*b;
			dv2 = d2(v[i], Pb);
		}
		// test with current max distance squared
		if (dv2 <= maxd2) continue;

		// v[i] is a new max vertex
		maxi = i;
		maxd2 = dv2;
	}
	if (maxd2 > tol2)        // error is worse than the tolerance
	{
		// split the polyline at the farthest vertex from S
		mk[maxi] = 1;      // mark v[maxi] for the simplified polyline
		// recursively simplify the two subpolylines at v[maxi]
		simplifyDP( tol, v, j, maxi, mk );  // polyline v[j] to v[maxi]
		simplifyDP( tol, v, maxi, k, mk );  // polyline v[maxi] to v[k]
	}
	// else the approximation is OK, so ignore intermediate vertices
	return; 
}


//-------------------------------------------------------------------
// needs simplifyDP which is above
void Path2d::simplify(float tol)
{
	int n = mPoints.size();
	std::vector <Vec2> V = mPoints;
	bool curved = mCurved;
	bool closed = (mSegments.back() == CLOSE);
	
	clear();
	
	// TODO handle CURVE segments...
	assert(mCurved==false);
	
	int    i, k, m, pv;            // misc counters
	float  tol2 = tol * tol;       // tolerance squared
	
	Vec2 * vt = new Vec2[n];
	int * mk = new int[n];

	memset(mk, 0, sizeof(int) * n );

	// STAGE 1.  Vertex Reduction within tolerance of prior vertex cluster
	vt[0] = V[0];              // start at the beginning
	for (i=k=1, pv=0; i<n; i++) {
		if (d2(V[i], V[pv]) < tol2) continue;

		vt[k++] = V[i];
		pv = i;
	}
	
	// add flag?
	if (pv < n-1) vt[k++] = V[n-1];      // finish at the end

	// STAGE 2.  Douglas-Peucker polyline simplification
	mk[0] = mk[k-1] = 1;       // mark the first and last vertices
	simplifyDP( tol, vt, 0, k-1, mk );

	mPoints.assign(k,Vec2(0,0));
	
	// copy marked vertices to the output simplified polyline
	Vec2 prev;
	bool found = false;
	for (i=m=0; i<k; i++) 
	{
		const Vec2 & p = vt[i];
		if(found)
		{
			if( mk[i]  && cm::squareDistance(p,prev) > 4 )
			{
				mPoints[m++] = p;
				prev = p;
			}
		}
		else
		{
			if (mk[i]) 
			{
				mPoints[m++] = p;//vt[i];
				found = true;
				prev = p;
			}
		}
	}

	//get rid of the unused points
	if( m < (int)mPoints.size() ) mPoints.erase( mPoints.begin()+m, mPoints.end() );

	delete [] vt;
	delete [] mk;
	
	for( int i = 1; i < mPoints.size(); i++ )
	{
		mSegments.push_back(LINETO);
	}
	
	if(closed)
	{
		mSegments.push_back(CLOSE);
	}
}


// TODO use M33!
void Path2d::transform( const M44 & m )
{
	for( int i = 0; i < size(); i++ )
	{
		const Vec2 & pp = mPoints[i];
		Vec4 p(pp.x,pp.y,0,1);
	
		p = mul(p,m);
		p /= p.w;

		
		mPoints[i] = Vec2(p.x,p.y);//p.xy();//mul(mPoints[i],m); 
	}
}

void Path2d::transform( const M33 & m )
{
	transform(M44(m));
}


Vec2 Path2d::getCentroid() const
{
	Vec2 c(0,0);
	for( int i = 0; i < size(); i++ )
		c+=getPoint(i);
	c/=size();
	return c;
}

	void Path2d::cleanup( float eps )
	{
		if( !size() )
			return;
		
		bool closed = isClosed();

		bool hadNans = false;
		std::vector<Vec2>::iterator it = mPoints.begin();
		while(it!=mPoints.end())
		{
			const Vec2 & p = *it;
			if( std::isnan(p.x) || std::isnan(p.y) )
			{
				it = mPoints.erase(it);
				hadNans = true;
			}
			else
				it++;
		}

		if( size() < 2 || (closed && size() < 3) )
		{
			clear();
			return;
		}

		std::vector <Vec2> tmp;
		std::vector<SegmentType>	tmpSeg;
		
		Vec2 prev = mPoints[0];
		tmp.push_back(prev);
		
		for( int i = 1; i < size(); i++ )
		{
			const Vec2 &p = mPoints[i];
			
			bool ok = true;
			
			if( cm::distance(prev,p) < eps )
				continue;
			else
			{
				tmp.push_back(p);
				prev = p;
			}
		}
		
		// make sure last point is in
		if( tmp.size() < 2 )
		{
			clear();
			return;
		}

		tmp.back() = mPoints.back();

		if( closed )
		{
			if( tmp.size() < 3 )
			{
				// what shal we do then?
				//assert(0);
				printf("Path is corrupted!\n");
				clear();
				return;
			}
		}
		
		for (int i = 1; i < tmp.size(); i++ )
		{
			tmpSeg.push_back( Path2d::LINETO );
		}
		
		if( closed )
			tmpSeg.push_back( Path2d::CLOSE );
		
		mSegments = tmpSeg;
		mPoints = tmp;
	}

	
	void	Path2d::draw()
	{
		gfx::draw(*this);
	}
	
	void	Path2d::drawSolid()
	{
		gfx::drawSolid(*this);
	}
	
	void	Path2d::limitSize( int len )
	{
		int n = getNumPoints()-len;
		
		if( n <= 0 )
			return;
		
		mPoints.erase(mPoints.begin(),mPoints.begin()+n);
		mSegments.erase(mSegments.begin(),mSegments.begin()+n);
	}
/*
void Path2d::cleanup( float eps )
{
	if( !size() )
		return;
	
	if( isClosed() && size() < 3 )
		return;
	
	std::vector <Vec2> tmp;
	std::vector<SegmentType>	tmpSeg;
	
	Vec2 prev = mPoints[0];
	tmp.push_back(prev);
	
	for( int i = 1; i < size(); i++ )
	{
		Vec2 p = mPoints[i];
		float d = cm::distance( prev, p );
		if( d > eps )
		{
			prev = p;
			tmp.push_back(p);
		}
	}
	
	if( isClosed() )
	{
		if( tmp.size() < 3 )
		{
			// what shal we do then?
			//assert(0);
			printf("Path is corrupted!\n");
			clear();
			return;
		}
		
		Vec2 ref = tmp[0];
		Vec2 p = tmp.back();
		float d =  cm::distance(ref,p);
		while( d < eps )
		{
			tmp.pop_back();
			p = tmp.back();
			d = cm::distance(ref,p);
		}
	}
	
	for (int i = 1; i < tmp.size(); i++ )
	{
		tmpSeg.push_back( Path2d::LINETO );
	}
	
	if( isClosed() )
		tmpSeg.push_back( Path2d::CLOSE );
	
	mSegments = tmpSeg;
	mPoints = tmp;
}
	
*/

Path2d Path2d::makeRect( float x, float y, float w, float h )
{
	Path2d p;
	p.addPoint(x,y);
	p.addPoint(x+w,y);
	p.addPoint(x+w,y+h);
	p.addPoint(x,y+h);
	p.close();
	return p;
}
	

void Path2d::append( const Path2d & p )
{
	for( int i = 0; i < p.size(); i++ )
		addPoint(p[i]);
}
	
	
/*
 isClockwise: function() {
 if (this._clockwise !== undefined)
 return this._clockwise;
 var sum = 0,
 xPre, yPre;
 function edge(x, y) {
 if (xPre !== undefined)
 sum += (xPre - x) * (y + yPre);
 xPre = x;
 yPre = y;
 }
 for (var i = 0, l = this._segments.length; i < l; i++) {
 var seg1 = this._segments[i],
 seg2 = this._segments[i + 1 < l ? i + 1 : 0],
 point1 = seg1._point,
 handle1 = seg1._handleOut,
 handle2 = seg2._handleIn,
 point2 = seg2._point;
 edge(point1._x, point1._y);
 edge(point1._x + handle1._x, point1._y + handle1._y);
 edge(point2._x + handle2._x, point2._y + handle2._y);
 edge(point2._x, point2._y);
 }
 return sum > 0;
 },
 
 setClockwise: function(clockwise) {
 if (this.isClockwise() != (clockwise = !!clockwise)) {
 this.reverse();
 this._clockwise = clockwise;
 }
 },
*/
	
}
