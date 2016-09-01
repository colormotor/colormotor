#pragma once
#include "cm_gfx.h"

namespace cm
{

enum 
{
	CLIP_DIFFERENCE = 0,
	CLIP_INTERSECTION,
	CLIP_UNION,
	CLIP_XOR
};

enum
{
	JOIN_SQUARE=0,
	JOIN_MITER,
	JOIN_ROUND
};

class PolyClipper
{	
public:
	const Shape& apply( int type, const Shape & a, const Shape & b );
	const Shape& apply( int type, const Contour & a, const Contour & b );
	
	// would have liked to call this union... maybe Uppercase func names are better at the end...
	const Shape& merge( const Shape & a, const Shape & b, double offset=2.0 );
	const Shape& merge( const Shape & shape, double offset = 2.0 );

	const Shape& offset( const Shape & a, double offset, int joinType=JOIN_ROUND, double miterLimit=1. );

	Shape result;
protected:
	void op( int type, const Shape & a, const Shape & b, double offset = 0.0 );
};

Shape shapeUnion( const Shape & a, const Shape & b );
Shape shapeDifference( const Shape & a, const Shape & b );
Shape shapeIntersection( const Shape & a, const Shape & b );
Shape shapeXor( const Shape & a, const Shape & b );
Shape shapeOffset( const Shape& shape, double offset, int joinType=JOIN_ROUND, double miterLimit=1. );


arma::vec chordLengths( const Contour & P );
arma::vec cumChordLengths( const Contour & P );
double chordLength( const Contour & P );
    
/// Interpolate contour points
/// X are the distances along the contour for each point
/// Xi are the interpolation values
Contour interpolate( const Contour & P, const arma::vec &X, const arma::vec &Xi, const char* method="linear" );
    
/// Interpolate contour points
/// Xi are the interpolation values in the range 0 to 1
Contour interpolate( const Contour & P, const arma::vec& Xi, const char* method="linear");

/// Interpolate vector
/// Xi are the interpolation values in the range 0 to 1
arma::vec interpolate( const arma::vec& Y, const arma::vec& Xi, const char* method="linear");

    
/// Uniformly sample a contour at distance s
Contour uniformSample( const Contour & P, float s );
    
double angleBetween( const V2& a, const V2& b);
    
double triangleArea( const V2& a, const V2& b, const V2 & c );
    
V2 closestPointOnSegment( const V2& p, const V2& a, const V2& b );
    
double distanceToSegment( const V2& p, const V2& a, const V2& b );
    
double squareDistanceToSegment( const V2& p, const V2& a, const V2& b );
    
double distanceToLine( const V2& p, const V2& a, const V2& b );
    
M33 rectTransform( const Box &src, const Box&dst, float padding=0.0 );
    
/// Line intersection utils
int lineIntersection( V2 * intersection,  V2 * uv, 
					  const V2& a1, 
					  const V2& a2, 
					  const V2& b1, 
					  const V2& b2, 
					  bool aIsSegment = true, bool bIsSegment = true );

int lineIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int lineSegmentIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );
	
//template <typename double> double median( const std::vector<double> & vals );
int segmentLineIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int segmentIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int lineRayIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int rayLineIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int rayIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int lineIntersectionRange( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2,
					  float aRangeMin,
					  float aRangeMax,
					  float bRangeMin,
					  float bRangeMax
					   );

}