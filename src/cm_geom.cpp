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


#include "cm_geom.h"
#include "deps/clipper/clipper.hpp"

namespace cm
{

#define RANGE	0xFFFFFFFFFF

double scaleFactor( const Box& rect, double range )
{
	return (1. / std::max(rect.width(), rect.height())) * range;
}

ClipperLib::IntPoint convertToIntPoint( const arma::vec & p, const Box& rect, double range )
{
    arma::vec v = p-arma::vec({rect.l(),rect.t()});
	v *= scaleFactor(rect, range);
    return ClipperLib::IntPoint(v[0], v[1]);
}

arma::vec convertToVec( const ClipperLib::IntPoint & p, const Box& rect, double range )
{
	arma::vec v({p.X,p.Y});
	v /= scaleFactor(rect, range);
	v += arma::vec({rect.l(),rect.t()});
	return v;
}

void shapeToPolygons( ClipperLib::Paths& poly, const Box& r, const Shape& shape, double range=RANGE )
{
	poly.resize(shape.size());
	
	for( int i = 0; i < shape.size(); i++ )
	{
		const Contour & p = shape.getContour(i);
		for( int j = 0; j < p.size(); j++ )
			poly[i].push_back(convertToIntPoint(p.getPoint(j), r, range));
	}	
}

void polygonsToShape( Shape& shape, const Box& r, const ClipperLib::Paths& poly, double range=RANGE )
{
	shape.clear();
	for( int i = 0; i < poly.size(); i++ )
	{
		const ClipperLib::Path &P = poly[i];
		Contour C;
		for( int j = 0; j < P.size(); j++ )
		{
			const ClipperLib::IntPoint & p = P[j];
			C.addPoint(convertToVec(p, r, range));
		}
		C.closed = true;
		shape.add(C);
	}
}

void PolyClipper::op( int type, const Shape & a, const Shape & b, double offset )
{	
	Box r = a.boundingBox();
	r.include(b.boundingBox());	
	
	ClipperLib::Paths pa;
	ClipperLib::Paths pb;
	ClipperLib::Paths sol;
	
	shapeToPolygons(pa, r, a);
	shapeToPolygons(pb, r, b);
	
	ClipperLib::Clipper c;
	
	// expand ( or shrink? ) polys, this can be useful when making an union of hardly intersecting shapes
	// if(false)// fabs( offset ) > 0.0 )
	// {
	// 	ClipperLib::OffsetPolygons(pa, pa, offset);
	// 	ClipperLib::OffsetPolygons(pb, pb, offset);
	// }
	
	c.AddPaths(pa, ClipperLib::ptSubject, true);
	c.AddPaths(pb, ClipperLib::ptClip, true);
	c.Execute((ClipperLib::ClipType)type, sol, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	
	polygonsToShape(result, r, sol);
}


const Shape & PolyClipper::apply( int type, const Contour & a, const Contour & b )
{
	Shape sa;
	sa.add(a);
	Shape sb;
	sb.add(b);
	return apply(type,sa,sb);
}

const Shape & PolyClipper::apply( int type, const Shape & a, const Shape & b )
{
	ClipperLib::ClipType conv[] = {
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
		op(conv[type], a, b);
	
	return result;
}

const Shape& PolyClipper::merge( const Shape & a, const Shape & b, double offset )
{
	op(ClipperLib::ctUnion,a,b,offset);
	return result;
}
	
const Shape& PolyClipper::merge( const Shape & shape, double offset  )
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

const Shape& PolyClipper::offset( const Shape & shape, double offset, int joinType, double miterLimit )
{
	ClipperLib::JoinType conv[] = {
	ClipperLib::jtSquare,
	ClipperLib::jtMiter,
	ClipperLib::jtRound
	};
	
	ClipperLib::Paths in;
	ClipperLib::Paths out;
    
    Box r = shape.boundingBox();
    double range = std::max(r.width(), r.height())*2.;
	offset *= scaleFactor(r, range);

    shapeToPolygons(in, r, shape, range);
	SimplifyPolygons(in, ClipperLib::pftNonZero); // TODO handle different fill rules

	ClipperLib::ClipperOffset c(miterLimit);
	c.AddPaths(in, conv[joinType], ClipperLib::etClosedPolygon);
	c.Execute(out, offset);
	
	// convert back
	polygonsToShape(result, r, out, range);
	
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

Shape shapeOffset( const Shape& shape, double offset, int joinType, double miterLimit )
{
	PolyClipper clip;
	return clip.offset(shape, offset, joinType, miterLimit);
}

double distance( const arma::vec& a, const arma::vec&b )
{
	return norm(a-b);
}

double distance( float a, float b )
{
	return fabs(a-b);
}

arma::vec chordLengths( const Contour & P )
{
    std::vector<double> L;
    int n = P.size();
    for( int i = 0; i < n-1; i++ )
    	L.push_back( norm( P.points.col(i) - P.points.col(i+1) ) );
    return arma::vec(L);
}

arma::vec cumChordLengths( const Contour & P )
{
    arma::vec L = chordLengths(P);
	L = arma::join_cols<arma::vec>( arma::vec({0.0}), L );
	return cumsum(L);
}

double chordLength( const Contour & P )
{
	arma::vec L = cumChordLengths(P);
	return L( L.n_rows-1 );
}

Contour interpolate( const Contour & P, const arma::vec &X, const arma::vec &Xi, const char* method )
{
	arma::mat Y = arma::zeros(P.dimension(), Xi.n_rows);
	for( int i = 0; i < P.dimension(); i++ )
    {
        arma::vec yy;
        arma::interp1(X, P.points.row(i), Xi, yy, method);
        Y.row(i) = yy.t();
    }	
    return Contour(Y, P.closed);
}

Contour interpolate( const Contour & P, const arma::vec& Xi, const char* method)
{
    return interpolate( P, arma::linspace(0., 1., P.size()), Xi, method );
}
    
arma::vec interpolate( const arma::vec& Y, const arma::vec& Xi, const char* method)
{
    arma::vec yy;
    arma::interp1(arma::linspace(0., 1., Y.size()), Y, Xi, yy, method);
    return yy;
}


Contour uniformSample( const Contour & P, float step )
{
    arma::vec X = cumChordLengths(P);
    double l = X(X.n_cols-1);
    X = X / l;
    arma::vec Xi = regspace(0.0, step, l);
    return interpolate( P, X, Xi );
}
    
double angleBetween( const V2& a, const V2& b)
{
    return ::atan2( a[0]*b[1] - a[1]*b[0], a[0]*b[0] + a[1]*b[1] );
}

double triangleArea( const V2& a, const V2& b, const V2& c )
{
    V2 da = a-b;
    V2 db = c-b;
    return (da.x * db.y - da.y * db.x)*0.5;
}

V2 closestPointOnSegment( const V2& p, const V2& a, const V2& b )
{
    V2 v = b-a;
    V2 w = p-a;
    
    float d1 = dot(w,v);
    if( d1 <= 0.0f )
        return a;
    float d2 = dot(v,v);
    if( d2 <= d1 )
        return b;
    
    float t = d1/d2;
    return a + v*t;
}
    

double distanceToSegment( const V2& p, const V2& a, const V2& b )
{
    return arma::norm(p - closestPointOnSegment(p,a,b));
}

double squareDistanceToSegment( const V2& p, const V2& a, const V2& b )
{
    V2 d = p - closestPointOnSegment(p,a,b);
    return dot(d,d);
}

double distanceToLine( const V2& p, const V2& a, const V2& b )
{
	if(feq(a.x, b.x) && feq(a.y, b.y))
		return arma::norm(p-a);
    V2 da = b-a;
    V2 db = a-p;
    return fabs(da.x * db.y - da.y * db.x) / arma::norm(b-a);
}
    
int pointIn( const V2& p , const Contour & ctr )
{
    int i, j, c = 0;
    
    for (i = 0, j = ctr.size()-1; i < ctr.size(); j = i++)
    {
        if ( ((ctr[i][1] > p.y) != (ctr[j][1] > p.y)) &&
            (p.x < (ctr[j][0]-ctr[i][0]) * (p.y-ctr[i][1]) / (ctr[j][1]-ctr[i][1]) + ctr[i][0]) )
            c = !c;
    }
    return c;
}

M33 rectTransform( const Box &src, const Box&dst, float padding )
{
    float srcw = src.width();
    float srch = src.height();
    float dstw = dst.width()-padding*2;
    float dsth = dst.height()-padding*2;
    float ratio = std::min(dstw/srcw, dsth/srch);
    return trans2d((V2)dst.center()) * scaling2d(V2(ratio,ratio)) * trans2d((V2)-src.center());
}


int lineIntersection( V2 * intersection,  V2 * uv, 
					  const V2& a1, 
					  const V2& a2, 
					  const V2& b1, 
					  const V2& b2, 
					  bool aIsSegment, bool bIsSegment )
{
	const double EPS = 0.00001;
	double denom  = (b2.y-b1.y) * (a2.x-a1.x) - (b2.x-b1.x) * (a2.y-a1.y);
	double numera = (b2.x-b1.x) * (a1.y-b1.y) - (b2.y-b1.y) * (a1.x-b1.x);
	double numerb = (a2.x-a1.x) * (a1.y-b1.y) - (a2.y-a1.y) * (a1.x-b1.x);
   
	if (fabs(denom) < EPS) 
	{
		intersection->x = 0;
		intersection->y = 0;
		return 0;
	}

	uv->x = numera / denom;
	uv->y = numerb / denom;

	intersection->x = a1.x + uv->x * (a2.x - a1.x);
	intersection->y = a1.y + uv->x * (a2.y - a1.y);

   bool isa = true;
   if( aIsSegment && (uv->x  < 0 || uv->x  > 1) )
		isa = false;
   bool isb = true;
   if( bIsSegment && (uv->y < 0 || uv->y  > 1) )
		isb = false;

	return isa&&isb;
}

	  
int lineIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 )
					  {
						V2 uv;
						return lineIntersection(intersection,&uv,a1,a2,b1,b2,false,false);
					  }
					  
int lineSegmentIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 )
					  {
						V2 uv;
						return lineIntersection(intersection,&uv,a1,a2,b1,b2,false,true);
					  }
			
	
//template <typename double> double median( const std::vector<double> & vals );
int segmentLineIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 )
					  {
						V2 uv;
						return lineIntersection(intersection,&uv,a1,a2,b1,b2,true,false);
					}
	
int segmentIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 )
					  {
						V2 uv;
						return lineIntersection(intersection,&uv,a1,a2,b1,b2,true,true);
					}

int lineRayIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 )
					  {
						V2 uv(0,0);
						return lineIntersection(intersection,&uv,a1,a2,b1,b2,false,false) && uv.y > 0;
					  }

int rayLineIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 )
					  {
						V2 uv(0,0);
						return lineIntersection(intersection,&uv,a1,a2,b1,b2,false,false) && uv.x > 0;
					  }

int rayIntersection( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2 )
					  {
						V2 uv(0,0);
						return lineIntersection(intersection,&uv,a1,a2,b1,b2,false,false) && uv.x > 0 && uv.y > 0;
					  }

int lineIntersectionRange( V2 * intersection, 
					  const  V2& a1, 
					  const  V2& a2, 
					  const  V2& b1, 
					  const  V2& b2,
					  float aRangeMin,
					  float aRangeMax,
					  float bRangeMin,
					  float bRangeMax
					   )
					  {
						V2 uv(0,0);
						return lineIntersection(intersection,&uv,a1,a2,b1,b2,false,false) 
								&& uv.x > aRangeMin && uv.x < aRangeMax 
								&& uv.y > bRangeMin && uv.y < bRangeMax ;
					  }


/*
void saveHpgl( const std::string& path, const Box& srcBox=Box(0, 0, 512, 512), const Box& dstBox = Box(-3000, -3000, 6000, 6000 ) )
{
    // 4000
    // scaling to fit
    Box bb = Box(0,0,appHeight(),appHeight()); //all.boundingBox();
    float boxw = bb.width();
    float boxh = bb.height(); //:S
    V2 topleft = (V2)bb.minmax.col(0);
    
    
    FILE * f = fopen("render.hpgl","w");
    for( int i = 0; i < all.size(); i++ )
    {
        
        Contour P = all[i];
        ::fprintf(f,"PU;");
        for( int j = 0; j < P.size(); j++ )
        {
            
            V2 p = P[j];
            p.x = (p.x - topleft.x) / boxw;
            p.y = (p.y - topleft.y) / boxh;
            
            ::fprintf(f,"PA%d,%d;",
                      l+(int)(p.x*(r-l)),
                      b+(int)(p.y*(t-b)));
            
            if(j==0)
            {
                ::fprintf(f, "PD;");
            }
        }
    }
    
    ::fprintf(f, "\n");
    printf("Finished rendering hpgl\n");
    fclose(f);
}*/

}