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

#pragma once
#include "Shape.h"

namespace cm
{

enum 
{
	CLIP_DIFFERENCE = 0,
	CLIP_INTERSECTION,
	CLIP_UNION,
	CLIP_XOR
};

class PolyClipper
{	
public:
	const Shape& apply( int type, const Shape & a, const Shape & b );
	const Shape& apply( int type, const Contour & a, const Contour & b );
	
	// would have liked to call this union... maybe Uppercase func names are better at the end...
	const Shape& merge( Shape & a, const Shape & b, double offset = 2.0 );
	const Shape& merge( Shape & shape, double offset = 2.0 );
	Shape result;
protected:
	void op( int type, const Shape & a, const Shape & b, double offset = 0.0 );
};

Shape shapeUnion( const Shape & a, const Shape & b );
Shape shapeDifference( const Shape & a, const Shape & b );
Shape shapeIntersection( const Shape & a, const Shape & b );
Shape shapeXor( const Shape & a, const Shape & b );

}