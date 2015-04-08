
#include "Shape.h"
#include "cmGeomGfx.h"
#include "Path2d.h"

namespace cm
{
    
    
Contour::Contour()
:
closed(false)
{
    
}
    
Contour::Contour( const Contour & mom )
    :
closed(mom.closed),
points(mom.points)
{
    
}
    
Contour::Contour( const cm::Path2d & mom )
{
    closed = mom.isClosed();
    points = mom.subdivide();
}
    
Contour::Contour( const std::vector<Vec2> & P, bool closed )
    :
closed(closed)
{
    points = P;
}
    
Contour::Contour( const vectord & X, const vectord & Y, bool closed )
    :
closed(closed)
{
    assert(X.size() == Y.size());
    points.assign(X.size(),Vec2(0,0));
    for( int i = 0; i < X.size(); i++ )
        points[i](X[i],Y[i]);
}

Contour::Contour( float * buf, int n, int n2, bool closed )
    :
closed(closed)
{
    if(n2!=2)
    {
        printf("Can't convert numpy array to path2d, DIM2 is %d\n",n2);
        return;
    }
    
    points.assign(n, Vec2(0,0));
    for( int i = 0; i < n; i++ )
    {
        points[i].x = buf[(i<<1)];
        points[i].y = buf[(i<<1)+1];
    }
}

void Contour::insertPoint( int i, const Vec2 & p )
{
    if(i >= points.size() || points.size() == 0 )
        addPoint(p);
    
    points.insert(points.begin()+i,p);
}

void Contour::addPoint( const Vec2 & p )
{
    points.push_back(p);
}
    
void Contour::addPoint( float x, float y )
{
    points.push_back(Vec2(x,y));
}

void Contour::addPoints( const std::vector<Vec2> & P )
{
    for( int i = 0; i < P.size(); i++ )
        addPoint(P[i]);
}

void Contour::reverse()
{
    if( empty() )
        return;
    
    std::reverse( points.begin(), points.end() );
}

vectord Contour::X() const
{
    vectord res(points.size());
    for( int i = 0; i < points.size(); i++ )
        res[i] = points[i].x;
    return res;
}
    
vectord Contour::Y() const
{
    vectord res(points.size());
    for( int i = 0; i < points.size(); i++ )
        res[i] = points[i].y;
    return res;
}


bool Contour::contains( const Vec2 &p ) const
{
    return (bool)pointInPoly(points.size(), &points[0], p );
}
 

// local defs for simplify
// Adapted from LibCinder
    
typedef struct{
    Vec2 P0;
    Vec2 P1;
}Segment;

#define norm2(v)   dot(v,v)        // norm2 = squared length of vector
#define norm(v)    sqrt(norm2(v))  // norm = length of vector
#define d2(u,v)    norm2(u-v)      // distance squared = norm2 of difference
#define d(u,v)     norm(u-v)       // distance = norm of difference

void  Contour::simplifyDP(float tol, Vec2f* v, int j, int k, int* mk )
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
void Contour::simplify(float tol)
{
    int n = points.size();
    std::vector <Vec2> V = points;

    points.clear();
    
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
    
    points.assign(k,Vec2(0,0));
    
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
                points[m++] = p;
                prev = p;
            }
        }
        else
        {
            if (mk[i]) 
            {
                points[m++] = p;//vt[i];
                found = true;
                prev = p;
            }
        }
    }
    
    //get rid of the unused points
    if( m < (int)points.size() ) points.erase( points.begin()+m, points.end() );
    
    delete [] vt;
    delete [] mk;
}


void Contour::cleanup( float eps )
{
    if( !size() )
        return;
    
    bool hadNans = false;
    
    std::vector<Vec2>::iterator it = points.begin();
    
    while(it!=points.end())
    {
        const Vec2 & p = *it;
        if( isnan(p.x) || isnan(p.y) )
        {
            it = points.erase(it);
            hadNans = true;
        }
        else
            it++;
    }
    
    // path is completely corrupted clear it
    if( size() < 2 || (closed && size() < 3) )
    {
        printf("Path is corrupted! Clearing\n");
        clear();
        return;
    }
    
    std::vector <Vec2> tmp;

    Vec2 prev = points[0];
    tmp.push_back(prev);
    
    for( int i = 1; i < size(); i++ )
    {
        const Vec2 &p = points[i];
        
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
    
    tmp.back() = points.back();
    
    if( closed )
    {
        if( tmp.size() < 3 )
        {
            // what shal we do then?
            //assert(0);
            printf("Path is corrupted! Clearing\n");
            clear();
            return;
        }
    }
    
    points = tmp;
}

void Contour::transform( const M33 & m )
{
    for( int i = 0; i < size(); i++ )
    {
        points[i] = mul( m, points[i] );
    }
}

M33 Contour::scaleToDiagonal( float diagonalLength )
{
    float diag = boundingBox().getDiagonalLength();
    Vec2 cenp = centroid();
    M33 m;
    m.identity();
    m.translate(cenp);
    m.scale(diagonalLength / diag);
    m.translate(-cenp);
    transform(m);
    return m;
}

M33 Contour::scaleToDiagonal( const Rect & r )
{
    return scaleToDiagonal(r.getDiagonalLength());
}

Rect Contour::boundingBox() const
{
    Rect r(0,0,0,0);
    if( ! points.empty() )
    {
        r = Rect( points[0], points[0] );
        r.include( points );
    }
    
    return r;
}

Vec2  Contour::centroid() const
{
    Vec2 c(0,0);
    for( int i = 0; i < size(); i++ )
        c+=points[i];
    c/=size();
    return c;
}

Contour Contour::section( int a, int b ) const
{
    return Contour( std::vector<Vec2>( points.begin()+a, points.begin()+b ) );
}

/// Get a position along the contour interpolated between [0,1]
/// Adapted from LibCinder code
Vec2 Contour::getPosition( float t ) const
{
    if( t <= 0 )
        return points[0];
    else if( t >= 1 )
        return points.back();
    
    int totalSegments =  size()-1;
    float segParamLength = 1.0f / totalSegments;
    size_t seg = t * totalSegments;
    float subSeg = ( t - seg * segParamLength ) / segParamLength;
    
    float t1 = 1 - t;
    return points[seg]*t1 + points[seg+1]*t;
}

/// Returns the length for each segment
vectord Contour::segmentLengths() const
{
    vectord D(size());
    for( int i = 0; i < size()-1; i++ )
        D[i] = distance( points[i+1], points[i] );
    return D;
}
    
vectord Contour::arcLengths( int a , int b) const
{
    vectord L;
    
    L.push_back(0.0f);
    
    if( a == -1 )
        a = 0;
    if( b == -1 )
        b = size();
    for( int i = a; i < b-1; i++ )
    {
        L.push_back( L.back() + cm::distance( points[i+1], points[i] ) );
    }
    
    if( closed )
        L.push_back( L.back() + cm::distance( points[0], points.back() ) );
    
    return L;
}

float Contour::arcLength( int a , int b) const
{
    float l = 0.0;
    if( a == -1 )
        a = 0;
    if( b == -1 )
        b = size();
    for( int i = a; i < b-1; i++ )
    {
        l += cm::distance( points[i], points[i+1] );
    }
    return l;
}

float Contour::signedArea( int a , int b) const
{
    if( a == -1 )
        a = 0;
    if( b == -1 )
        b = size();
    
    int n = b-a;
    float area = 0.0f;
    for(int i = 0; i < n; i++)
    {
        int p0 = a+i;
        int p1 = a+((i+1)%n);
        area += points[p0].x * points[p1].y - points[p1].x * points[p0].y;
    }
    
    return area * 0.5;
}

float Contour::area( int a, int b ) const
{
    return fabs(signedArea(a,b));
}

void Contour::rect( const Rect & r )
{
    rect(r);
}

void Contour::rect( float x, float y, float w, float h )
{
    addPoint(x,y);
    addPoint(x+w,y);
    addPoint(x+w,y+h);
    addPoint(x,y+h);
    close();
}
    
void Contour::draw() const
{
    gfx::draw(*this);
}

void Contour::fill() const
{
    gfx::fill(*this);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
    
Shape::Shape( const Shape2d & s2d )
{
    for( int i = 0; i < s2d.size(); i++ )
        contours.push_back( Contour(s2d[i]) );
}
        
    
bool Shape::contains( const Vec2 &p ) const
{
    int n = 0;
    for( int i = 0; i < contours.size(); i++ )
    {
        if( contours[i].contains( p ) )
            n++;
    }
    
    return (n%2) == 1;
}


void Shape::transform( const M33 & m )
{
    for( int i = 0; i < size(); i++ )
    {
        contours[i].transform(m);
    }
}

M33 Shape::scaleToDiagonal( float diagonalLength )
{
    float diag = boundingBox().getDiagonalLength();
    Vec2 cenp = centroid();
    M33 m;
    m.identity();
    m.translate(cenp);
    m.scale(diagonalLength / diag);
    m.translate(-cenp);
    transform(m);
    return m;
}

M33 Shape::scaleToDiagonal( const Rect & r )
{
    return scaleToDiagonal(r.getDiagonalLength());
}

Rect Shape::boundingBox() const
{
    Rect r(0,0,0,0);
    bool first = true;
    for( int i = 0; i < size(); i++ )
    {
        const std::vector<Vec2> & points = contours[i].points;
        if( ! points.empty() )
        {
            if(first)
            {
                r = Rect( points[0], points[0] );
                first = false;
            }
            
            r.include( points );
        }
    }
    return r;
}

Vec2  Shape::centroid() const
{
    Vec2 c(0,0);
    int n = 0;
    for( int i = 0; i < size(); i++ )
    {
        const Contour & ctr = contours[i];
        for( int j = 0; j < ctr.size(); j++ )
        {
            c+=ctr[j];
            n++;
        }
    }
    c/=size();
    return c;
}

void Shape::simplify( float tol )
{
    for( int i = 0; i < contours.size(); i++ )
        contours[i].simplify(tol);
}

    
bool Shape::operator == (const Shape & s ) const
{
    float eps = 0.001;
    eps*=eps;
    
    if(s.size() != size())
        return false;
    
    for( int i = 0; i < contours.size(); i++ )
    {
        if(contours[i].size() != s.contours[i].size())
            return false;
        
        for( int j = 0; j < contours[i].size(); j++ )
        {
            if(squareDistance( contours[i][j], s.contours[i][j] ) > eps)
                return false;
        }
    }
    
    return true;
}
    

void Shape::draw() const
{
    gfx::draw(*this);
}

void Shape::fill() const
{
    gfx::fill(*this);
}



float turningAngleAt( const Contour & P, int index, int res )
{
    bool closed = P.isClosed();
    
    Vec2 l(0,0);
    Vec2 r(0,0);
    int cr = 0;
    int cl = 0;
    
    int n = P.size();
    
    float ang = 0.0;
    
    if( closed )
    {
        const Vec2 & p = P[index];
    
        for( int i = 1; i <= res; i++ )
        {
            int il = mod(index-i, n);
            int ir = mod(index+i, n);
            
            l += P[il] - p;
            cl ++;
            
            r += P[ir] - p;
            cr ++;
        }
    }
    else
    {
        if( index == 0 )
            return 0.0f;
        if( index == n-1 )
            return 0.0f;
        
        const Vec2 & p = P[index];
        
        for( int i = 1; i <= res; i++ )
        {
            int il = index-i;
            int ir = index+i;
            
            if( il >= 0 && il < n )
            {
                //l += p - P[il];//P[il] - p;
                l = p - P[il];
                cl ++;
            }
            
            if( ir >= 0 && ir < n )
            {
//                r += P[ir] - p;
                r = P[ir] - p;
                cr ++;
            }
            
            ang += angleBetween(l,r);
        }
        
//        l/=cl;
 //       r/=cr;
        
    }
    
    return ang / res; //angleBetween(l,r) / res;
}
    
vectord turningAngles( const Contour & P, int res )
{
    vectord A(P.size());
    for( int i = 0; i < P.size(); i++ )
        A[i] = turningAngleAt(P, i, res);
    return A;
}
    

float curvatureAt( const std::vector<Vec2> & P, int index, int n, int res )
{
    Vec2 l(0,0);
    Vec2 r(0,0);
    
    float k = 0.0;
    
    if( index == 0 )
        return 0.0f;
    if( index == n-1 )
        return 0.0f;
    
    const Vec2 & p = P[index];
    Vec2 vl,vr;
    
    for( int i = 1; i <= res; i++ )
    {
        int il = index-i;
        int ir = index+i;
        
        if( il >= 0 && il < n )
        {
            vl = p - P[il];
        }
        
        if( ir >= 0 && ir < n )
        {
            vr = P[ir] - p;
        }
        
        // could assume arc length here
        float ll = vl.length();
        float lr = vr.length();
        

        float theta = angleBetween(vl, vr);//
        
        k += (2.0 * ::sin(theta/2)) / sqrt(ll*lr);
    }
    
    
    return k/res; //angleBetween(l,r) / res;
}

vectord curvature( const Contour & P_, int res )
{
    std::vector<Vec2> P = pad1d(P_.points, res, P_.isClosed()?PAD_WRAP:PAD_EXTRAPOLATE);
    vectord A(P_.size());
    int n = P_.size();
    for( int i = 0; i < n; i++ )
        A[i] = curvatureAt(P, i+res, n, res);
    return A;
}

static float calcSegmentLength( const vectord & D, size_t segment, float minT, float maxT )
{
    if( segment >= D.size() )
        return 0;
    
    return D[segment] * (maxT - minT);
}

static float segmentSolveTimeForDistance( const Contour & ctr, const vectord & lengths, int segment, float segmentLength, float segmentRelativeDistance, float tolerance = 1.0e-03f, int maxIterations = 16 )
{
    // initialize bisection endpoints
    float a = 0, b = 1;
    float p = segmentRelativeDistance / segmentLength;    // make first guess
    
    // we want to calculate a value 'p' such that segmentLength( mCurrentSegment, mCurrentT, mCurrentT + p ) == lengthIncrement
    
    // iterate and look for zeros
    float lastArcLength = 0;
    float currentT = 0;
    for( int i = 0; i < maxIterations; ++i )
    {
        // compute function value and test against zero
        lastArcLength = calcSegmentLength( lengths, segment, currentT, currentT + p );
        
        float delta = lastArcLength - segmentRelativeDistance;
        
        if( fabs( delta ) < tolerance )
        {
            break;
        }
        
        // update bisection endpoints
        if( delta < 0 )
            a = p;
        else
            b = p;
        
        // get speed along polyline
        const float speed = lengths[segment];
        
        // if result will lie outside [a,b]
        if( ((p-a)*speed - delta)*((p-b)*speed - delta) > -tolerance )
            p = 0.5f*(a+b); // do bisection
        else
            p -= delta/speed; // otherwise Newton-Raphson
    }
    // If we failed to converge, hopefully 'p' is close enough
    
    return p;//( p + segment ) / (float)D.size();
}


float arcLengthParam( const Contour & ctr, float distance )
{
    if(ctr.size()<2)
        return 0;
    
    float len = ctr.arcLength();
    
    if( distance > len )
    {
        return 1.0f;
    }
    
    vectord lengths = ctr.segmentLengths();
    
    // Iterate the segments to find the segment defining the rawhnge containing our targetLength
    int currentSegment = 0;
    float currentSegmentLength = lengths[0];
    while( distance > currentSegmentLength )
    {
        distance -= currentSegmentLength;
        currentSegmentLength = lengths[++currentSegment];
    }
    
    return (segmentSolveTimeForDistance( ctr, lengths, currentSegment, currentSegmentLength, distance ) + currentSegment) / lengths.size();
}

Contour sampleAtDistance( const Contour & ctr, float s )
{
    vectord segLengths = ctr.segmentLengths();
    vectord D = ctr.arcLengths();
    float len = ctr.arcLength();
    
    float d = 0.0;
    int seg = 0;

    Contour res;
    
    while(d < len)
    {
        while(d > D[seg]+segLengths[seg] || segLengths[seg]==0 )
        {
            seg++;
        }
        
        float segLen = segLengths[seg];
        float relDist = d-D[seg];
        float t = segmentSolveTimeForDistance( ctr, segLengths, seg, segLengths[seg], relDist );
        Vec2 p = lerp(ctr[seg], ctr[seg+1],t);
        res.addPoint(p);
        //gfx::color(0,1,0);
        //gfx::drawCircle(p, 1);
        d+=s;
    }
    
    /*vectord debug = segmentLengths(res);
    for( int i = 0; i < debug.size(); i++ )
    {
        printf("%g, ",debug[i]);
    }
    printf("\n");*/
    return res;
}

/// Path sampled at uniform arc length s
/*
Contour sampleAtDistance( const Contour & P_, float s )
{
    Contour P(P_);
    P.cleanup(0.1);
    Contour res;
    
    vectord D = P.arcLengths();
    
    float d = 0.0;
    float startd = 0.0;
    
    bool firstp = true;
    Vec2 prevp;
    for( int i = 0; i < D.size()-1; i++ )
    {
        startd = D[i];
        
        Vec2 a = P[i];
        Vec2 b = P[i+1];
        
        double l = D[i+1] - D[i];
        
        while( d < D[i+1] )
        {
            double t = (d - startd) / l;
            Vec2 target = lerp(a, b, t);
            if(firstp)
            {
                prevp = target;
                res.addPoint(target);
                firstp = false;
            }
            else
            {
                Vec2 dv = target-prevp;
                dv.normalize();
                prevp = prevp + dv*s;
                res.addPoint(prevp);
            }
            d += s;
        }
    }
    
    if( P.isClosed() )
        res.close();
    
    //P.cleanup(std::min(0.1f, s));
    return res;
}
*/

Contour smoothGaussian( const Contour & P, float sigma )
{
    int padType=PAD_EXTRAPOLATE;
    if( P.isClosed() )
        padType = PAD_WRAP;
    
    vectord SX = gaussian1d( P.X(), sigma, 0.0, padType );
    vectord SY = gaussian1d( P.Y(), sigma, 0.0, padType );
    return Contour(SX, SY, P.isClosed());
}

Vec2 catmull( Vec2 P[4], float T[4], float t)
{
    Vec2 L01 = P[0] * (T[1] - t) / (T[1] - T[0]) + P[1] * (t - T[0]) / (T[1] - T[0]);
    Vec2 L12 = P[1] * (T[2] - t) / (T[2] - T[1]) + P[2] * (t - T[1]) / (T[2] - T[1]);
    Vec2 L23 = P[2] * (T[3] - t) / (T[3] - T[2]) + P[3] * (t - T[2]) / (T[3] - T[2]);
    Vec2 L012 = L01 * (T[2] - t) / (T[2] - T[0]) + L12 * (t - T[0]) / (T[2] - T[0]);
    Vec2 L123 = L12 * (T[3] - t) / (T[3] - T[1]) + L23 * (t - T[1]) / (T[3] - T[1]);
    Vec2 C12 = L012 * (T[2] - t) / (T[2] - T[1]) + L123 * (t - T[1]) / (T[2] - T[1]);
    return C12;
}   
    
void catmullInterpolate( Contour * res, Vec2 pts[4], int subd )
{
    float total = 0.0;
    
    float T[4] = {0, 0, 0, 0};
    
    for( int i = 0; i < 3; i++ )
    {
        Vec2 d = pts[i+1] - pts[i];
        total += ::pow(d[0]*d[0]+d[1]*d[1],0.25);
        T[i+1] = total;
    }
    
    float tstart = T[1];
    float tend = T[2];
    
    int segments = subd-1;
    
    for( int i = 0; i < segments; i++ )
        res->addPoint( catmull(pts,T,tstart + (float(i) * (tend - tstart)) / segments) );

    res->addPoint( pts[2] );
}
    
Contour centripetalCatmull( const Contour &P_, int subd)
{
    if(P_.size() <=2)
        return P_;
    
    Contour P(P_);

    //  use control points to extrapolate addtional control points
    Vec2 d = P[1]-P[0];
    Vec2 p1 = P[0]-d;
    d = P[-1]-P[-2];
    Vec2 p2 = P[-1]+d;
    
    P.insertPoint(0, p1);
    P.addPoint(p2);
    
    Contour O;
    int n = P.size();
    for( int i = 0; i < n; i+=4 )
    {
        Vec2 pts[4] = { P[i], P[i+1], P[i+2], P[i+3] };
        catmullInterpolate(&O, pts, subd);
    }
    
    return O;
}

vectord contourVelocity( const Contour &P,float sigma, float dt )
{
    vectord V(P.size()-1);
    for( int i = 0; i < P.size()-1; i++ )
    {
        V[i] = cm::distance(P[i+1], P[i])/dt;
    }
    V = gaussian1d(V, sigma);
    return V;
} 
    
    

TiXmlElement * contourToXml( const Contour & ctr )
{
    TiXmlElement * elem = new TiXmlElement("Contour");
    elem->SetAttribute("closed",(int)ctr.isClosed());
    for( int i = 0; i < ctr.points.size(); i++ )
    {
        TiXmlElement * p = new TiXmlElement("p");
        p->SetAttribute("x",floatToString(ctr.points[i].x).c_str());
        p->SetAttribute("y",floatToString(ctr.points[i].y).c_str());
        elem->LinkEndChild(p);
    }
    return elem;
}
    
Contour contourFromXml( TiXmlElement * elem )
{
    Contour ctr = Contour();
    if(stringToInt(elem->Attribute("closed")))
        ctr.close();
    
    TiXmlElement * e = elem->FirstChildElement();
    while(e)
    {
        float x = stringToFloat(e->Attribute("x"));
        float y = stringToFloat(e->Attribute("y"));
        ctr.addPoint(x,y);
        e = e->NextSiblingElement();
    }
    
    return ctr;
}

TiXmlElement * shapeToXml( const Shape & shape )
{
    TiXmlElement * elem = new TiXmlElement("Shape");
    for( int i = 0; i < shape.size(); i++ )
    {
        elem->LinkEndChild( contourToXml(shape.contours[i]) );
    }
    
    return elem;
}
    
Shape shapeFromXml( TiXmlElement * elem )
{
    Shape shape;
    TiXmlElement * e = elem->FirstChildElement();
    while(e)
    {
        shape.addContour( contourFromXml(e) );
        e = e->NextSiblingElement();
    }
    return shape;
}
    

TiXmlElement * shapesToXml( const std::vector<Shape> shapes )
{
    TiXmlElement * elem = new TiXmlElement("Shapes");
    for( int i = 0; i < shapes.size(); i++ )
        elem->LinkEndChild( shapeToXml(shapes[i]) );
    return elem;
}
    
std::vector<Shape> shapesFromXml( TiXmlElement * elem )
{
    std::vector<Shape> shapes;
    TiXmlElement * e = elem->FirstChildElement();
    while(e)
    {
        shapes.push_back(shapeFromXml(e));
        e = e->NextSiblingElement();
    }
    return shapes;
}

void contourToXml( const std::string & path, const Contour & s )
{
    TinyXmlObject::saveElementToFile(path, contourToXml(s) );
}

Contour contourFromXml( const std::string & path )
{
    TiXmlDocument doc( path.c_str() );
    if(!doc.LoadFile())
        std::vector<Shape2d>();
    
    TiXmlElement * root = doc.FirstChildElement();
    
    return contourFromXml(root);
}

void shapeToXml( const std::string & path, const Shape & s )
{
    TinyXmlObject::saveElementToFile(path, shapeToXml(s) );
}

Shape shapeFromXml( const std::string & path )
{
    TiXmlDocument doc( path.c_str() );
    if(!doc.LoadFile())
        std::vector<Shape2d>();
    
    TiXmlElement * root = doc.FirstChildElement();
    
    return shapeFromXml(root);
}


void shapesToXml( const std::string & path, const std::vector<Shape> shapes )
{
    TinyXmlObject::saveElementToFile(path, shapesToXml(shapes) );
}

std::vector<Shape> shapesFromXml( const std::string & path )
{
    TiXmlDocument doc( path.c_str() );
    if(!doc.LoadFile())
        std::vector<Shape2d>();
    
    TiXmlElement * root = doc.FirstChildElement();
    
    return shapesFromXml(root);
}



}

