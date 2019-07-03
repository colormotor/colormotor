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
	CLIP_EVENODD, CLIP_NONZERO, CLIP_POSITIVE, CLIP_NEGATIVE 
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
	const Shape& apply( int type, const Shape & a, const Shape & b, int fillType=CLIP_NONZERO );
	const Shape& apply( int type, const Contour & a, const Contour & b, int fillType=CLIP_NONZERO );
	
	// would have liked to call this union... maybe Uppercase func names are better at the end...
	const Shape& merge( const Shape & a, const Shape & b, double offset=2.0, int fillType=CLIP_NONZERO );
	const Shape& merge( const Shape & shape, double offset = 2.0, int fillType=CLIP_NONZERO );

	const Shape& offset( const Shape & a, double offset, int joinType=JOIN_ROUND, double miterLimit=1., int fillType=CLIP_NONZERO );

	Shape result;
protected:
	void op( int type, const Shape & a, const Shape & b, double offset = 0.0, int fillType=CLIP_NONZERO );
};

Shape shapeUnion( const Shape & a, const Shape & b, int fillType=CLIP_NONZERO );
Shape shapeDifference( const Shape & a, const Shape & b, int fillType=CLIP_NONZERO );
Shape shapeIntersection( const Shape & a, const Shape & b, int fillType=CLIP_NONZERO );
Shape shapeXor( const Shape & a, const Shape & b, int fillType=CLIP_NONZERO );
Shape shapeOffset( const Shape& shape, double offset, int joinType=JOIN_ROUND, double miterLimit=1., int fillType=CLIP_NONZERO );

std::vector<arma::mat> shapeUnion( const std::vector<arma::mat> & a, const std::vector<arma::mat> & b, int fillType=CLIP_NONZERO );
std::vector<arma::mat> shapeDifference( const std::vector<arma::mat> & a, const std::vector<arma::mat> & b, int fillType=CLIP_NONZERO );
std::vector<arma::mat> shapeIntersection( const std::vector<arma::mat> & a, const std::vector<arma::mat> & b, int fillType=CLIP_NONZERO );
std::vector<arma::mat> shapeXor( const std::vector<arma::mat> & a, const std::vector<arma::mat> & b, int fillType=CLIP_NONZERO );
std::vector<arma::mat> shapeOffset( const std::vector<arma::mat>& shape, double offset, int joinType=JOIN_ROUND, double miterLimit=1., int fillType=CLIP_NONZERO );

////////////////////////////////////////////////////////////////
// Other contour operations
Shape dpSimplify( const Shape& shape, float eps );
Contour dpSimplify( const Contour& c, float eps );
std::vector<arma::mat> dpSimplify( const std::vector<arma::mat> & S, bool closed, float eps );
arma::mat dpSimplify( const arma::mat& X, bool closed, float eps );


arma::vec chordLengths( const arma::mat& P, bool closed=false );
arma::vec cumChordLengths( const arma::mat& P, bool closed=false);
double chordLength( const arma::mat& P, bool closed=false);
    
typedef arma::mat Poly;
typedef std::vector<arma::mat> PolyList;

CM_INLINE arma::mat polyFromStd( const std::vector<arma::vec>& pts )
{
	arma::mat P = arma::zeros(pts[0].size(), pts.size());
	for( int i = 0; i < pts.size(); i++ )
		P.col(i) = pts[i];
	return P;
}

CM_INLINE arma::mat polyFromStd( const std::vector<V2>& pts )
{
	arma::mat P = arma::zeros(2, pts.size());
	for( int i = 0; i < pts.size(); i++ )
		P.col(i) = P[i];
	return P;
}

/// Interpolate contour points
/// X are the distances along the contour for each point
/// Xi are the interpolation values
arma::mat interpolate( const arma::mat& P, const arma::vec &X, const arma::vec &Xi, bool closed=false, const char* method="linear" );
    
/// Interpolate contour points
/// Xi are the interpolation values in the range 0 to 1
arma::mat interpolate( const arma::mat & P, const arma::vec& Xi, bool closed=false, const char* method="linear");

/// Interpolate vector
/// Xi are the interpolation values in the range 0 to 1
arma::vec interpolate( const arma::vec& Y, const arma::vec& Xi, const char* method="linear");

/// Uniformly sample a contour at distance s
arma::mat uniformSample( const arma::mat & P, float ds, bool closed=false );

/// Save points to file of type @type, if @savePointsAsRows is true (default) the 
/// contour will be saved transposed with respect to the armadillo matrix layout
void save( const arma::mat& P, const std::string & path, arma::file_type type = arma::csv_ascii, bool savePointsAsRows = true );
/// Load points from file of type @type, if @savePointsAsRows is true (default) the 
/// contour is assumed to be saved transposed with respect to the armadillo matrix layout
arma::mat load( const std::string & path, arma::file_type type = arma::csv_ascii, bool savePointsAsRows = true );

/// Load PolyList from SVG file
PolyList loadSvg( const std::string & f, int subd );

/// transform polygon
arma::mat transform( const arma::mat& m, const arma::mat& P );
/// transform Polylist
PolyList transform( const arma::mat& m, const PolyList& S );

/// Angle between two vectors
double angleBetween( const V2& a, const V2& b);
    
/// Signed triangle area
double triangleArea( const V2& a, const V2& b, const V2 & c );
    
/// Closest point on segment (a,b) to point 'p' 
V2 closestPointOnSegment( const V2& p, const V2& a, const V2& b );

/// Distance of point 'p' to segment (a,b)
double distanceToSegment( const V2& p, const V2& a, const V2& b );
    
/// Square distance of point 'p' to segment (a,b)
double squareDistanceToSegment( const V2& p, const V2& a, const V2& b );
    
/// Distance of point 'p' to line passing through (a,b)
double distanceToLine( const V2& p, const V2& a, const V2& b );
    
/// Transformation from rect 'src' to 'dst' with padding
M33 rectTransform( const Box &src, const Box&dst, float padding=0.0 );
    
/// Line intersection utils
//@{
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
//@}

}
