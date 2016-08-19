namespace cm
{

Shape shapeUnion( const Shape & a, const Shape & b );
Shape shapeDifference( const Shape & a, const Shape & b );
Shape shapeIntersection( const Shape & a, const Shape & b );
Shape shapeXor( const Shape & a, const Shape & b );

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
/*
%typemap(in, numinputs=0) ForceV2* intersection (ForceV2 temp)
{
 $1 = &temp;
}

%typemap(argout) ForceV2* intersection
{
 	  $result= SWIG_Python_AppendOutput( $result, SWIG_NewPointerObj(new arma::vec(*($1)), $1_descriptor, 0) );
}

int lineIntersection( ForceV2* intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int lineSegmentIntersection( ForceV2* intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );
	
//template <typename double> double median( const std::vector<double> & vals );
int segmentLineIntersection( ForceV2* intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int segmentIntersection( ForceV2* intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int lineRayIntersection( ForceV2* intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int rayLineIntersection( ForceV2* intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int rayIntersection( ForceV2* intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 );

int lineIntersectionRange( ForceV2* intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2,
					  float aRangeMin,
					  float aRangeMax,
					  float bRangeMin,
					  float bRangeMax
					   );
*/
}