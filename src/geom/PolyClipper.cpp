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


#include "PolyClipper.h"
#include "clipper/clipper.hpp"

namespace cm
{

#define RANGE	0xFFFFFFFFFF

ClipperLib::IntPoint convertToIntPoint( const Vec2 & p, const Rectf & rect )
{
	Vec2 vf = p-Vec2(rect.l,rect.t);
	Vec2d v(vf.x,vf.y);
	v /= Vec2d(rect.getWidth(),rect.getHeight());
	v *= RANGE;
	return ClipperLib::IntPoint(v.x,v.y);
}

Vec2 convertToVec2( const ClipperLib::IntPoint & p, const Rectf & rect )
{
	Vec2d v(p.X,p.Y);
	v /= RANGE;
	v *= Vec2d(rect.getWidth(),rect.getHeight());
	v += Vec2d(rect.l,rect.t);
	return Vec2f(v.x,v.y);
}

void PolyClipper::op( int type, const Shape & a, const Shape & b, double offset )
{	
	Vec2 scale;
	
	Rectf r = a.boundingBox();
	r.include(b.boundingBox());	
	
	ClipperLib::Polygons pa;
	ClipperLib::Polygons pb;
	ClipperLib::Polygons sol;
	
	pa.resize(a.size());
	pb.resize(b.size());
	
	for( int i = 0; i < a.size(); i++ )
	{
		const Contour & p = a.getContour(i);
		for( int j = 0; j < p.size(); j++ )
			pa[i].push_back(convertToIntPoint(p.getPoint(j),r));
	}
	for( int i = 0; i < b.size(); i++ )
	{
		const Contour & p = b.getContour(i);
		for( int j = 0; j < p.size(); j++ )
			pb[i].push_back(convertToIntPoint(p.getPoint(j),r));
	}
	
	ClipperLib::Clipper c;
	
	// expand ( or shrink? ) polys, this can be useful when making an union of hardly intersecting shapes
	if(false)// fabs( offset ) > 0.0 )
	{
		ClipperLib::OffsetPolygons(pa,pa,offset);
		ClipperLib::OffsetPolygons(pb,pb,offset);
	}
	
	c.AddPolygons(pa, ClipperLib::ptSubject);
	c.AddPolygons(pb, ClipperLib::ptClip);
	c.Execute((ClipperLib::ClipType)type, sol, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	
	result.clear();
	for( int i = 0; i < sol.size(); i++ )
	{
		const ClipperLib::Polygon &poly = sol[i];
		Contour path;
		for( int j = 0; j < sol[i].size(); j++ )
		{
			const ClipperLib::IntPoint & p = poly[j];
			path.addPoint(convertToVec2(p,r));
		}
		path.close();
		result.addContour(path);
	}
}


const Shape & PolyClipper::apply( int type, const Contour & a, const Contour & b )
{
	Shape sa;
	sa.addContour(a);
	Shape sb;
	sb.addContour(b);
	return apply(type,sa,sb);
}

const Shape & PolyClipper::apply( int type, const Shape & a, const Shape & b )
{
	int conv[] = {
	ClipperLib::ctDifference,
	ClipperLib::ctIntersection,
	ClipperLib::ctUnion,
	ClipperLib::ctXor
	};
	
	if( a.size() == 0 )
		result = Shape(b);
	if( b.size() == 0 )
		result = Shape(a);
	else
		op(conv[type],a,b);
	
	return result;
}

const Shape& PolyClipper::merge( Shape & a, const Shape & b, double offset )
{
	op(ClipperLib::ctUnion,a,b,offset);
	return result;
}
	
const Shape& PolyClipper::merge( Shape & shape, double offset  )
{
	if( shape.size() == 0)
	{
		result.clear();
		return result;
	}
	
	if( shape.size() == 1 )
	{
		result = shape;
		return result;
	}
	

	Shape a,b;
	a = Shape(shape[0]);
	for( int i = 1; i < shape.size(); i++ )
	{
		
		b = Shape(shape[i]);
		a = merge( a,b,offset );
	}
	
	return result;
}

	
	
Shape shapeUnion( const Shape & a, const Shape & b )
{
	PolyClipper clip;
	clip.apply(CLIP_UNION,a,b);
	return clip.result;
}

Shape shapeDifference( const Shape & a, const Shape & b )
{
	PolyClipper clip;
	clip.apply(CLIP_DIFFERENCE,a,b);
	return clip.result;
}

Shape shapeIntersection( const Shape & a, const Shape & b )
{
	PolyClipper clip;
	clip.apply(CLIP_INTERSECTION,a,b);
	return clip.result;
}

Shape shapeXor( const Shape & a, const Shape & b )
{
	PolyClipper clip;
	clip.apply(CLIP_XOR,a,b);
	return clip.result;

}



}