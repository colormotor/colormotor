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
#include "Shape2d.h"

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
	const Shape2d& apply( int type, const Shape2d & a, const Shape2d & b );
	const Shape2d& apply( int type, const Path2d & a, const Path2d & b );
	
	// would have liked to call this union... maybe Uppercase func names are better at the end...
	const Shape2d& merge( Shape2d & a, const Shape2d & b, double offset = 2.0 );
	const Shape2d& merge( Shape2d & shape, double offset = 2.0 );
	Shape2d result;
protected:
	void op( int type, const Shape2d & a, const Shape2d & b, double offset = 0.0 );
};

Shape2d shapeUnion( const Shape2d & a, const Shape2d & b );
Shape2d shapeDifference( const Shape2d & a, const Shape2d & b );
Shape2d shapeIntersection( const Shape2d & a, const Shape2d & b );
Shape2d shapeXor( const Shape2d & a, const Shape2d & b );

}