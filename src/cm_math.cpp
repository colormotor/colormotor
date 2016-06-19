#include "cm_math.h"

namespace cm
{
    
// from http://www.atnf.csiro.au/computing/software/gipsy/sub/bessel.c
static double bessi0( double x )
/*------------------------------------------------------------*/
/* PURPOSE: Evaluate modified Bessel function In(x) and n=0.  */
/*------------------------------------------------------------*/
{
    double ax,ans;
    double y;
    
    
    if ((ax=fabs(x)) < 3.75) {
        y=x/3.75,y=y*y;
        ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
                                             +y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
    } else {
        y=3.75/ax;
        ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
                                              +y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2
                                                                                 +y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1
                                                                                                                      +y*0.392377e-2))))))));
    }
    return ans;
}


arma::vec pdfVonMises( const arma::vec& theta, double mu, double sigma )
{
    double kappa = 1.0/sigma;
    double A = PI*2.0*bessi0(kappa);
    return arma::exp( kappa * arma::cos(theta-mu) ) / A;
}
    
arma::vec pdfGauss( const arma::mat& X, const arma::colvec& Mu, const arma::mat& Sigma )
{
    // testme
    int nbVar = X.n_rows;
    int nbData = X.n_cols;
    
    arma::mat Data = X.t() - arma::repmat(Mu.t(), nbData, 1);
    arma::mat Lambda = arma::inv(Sigma);
    arma::vec prob = arma::sum((Data*Lambda) % Data, 1);
    return arma::exp(-prob*0.5) / sqrt( pow(2*PI, nbVar) * fabs(arma::det(Sigma)) + std::numeric_limits<double>::lowest());
}


void eigs_descend( arma::vec & d_out, arma::mat & V_out, const arma::mat & X )
{
    arma::mat V;
    arma::vec d;
    arma::eig_sym(d, V, X);
    
    arma::uvec inds = arma::sort_index(d, "descend");
    d_out = arma::zeros(d.size());
    V_out = arma::zeros(V.n_rows, V.n_cols);

    for( int i = 0; i < inds.size(); i++ )
    {
        V_out.col(i) = V.col(inds[i]);
        d_out[i] = d[inds[i]];
    }
}

arma::mat velocity( const arma::mat& X_, float dt)
{
    arma::mat X = arma::join_horiz(X_.col(0), X_);
    arma::mat Dx = arma::diff(X,1,1) / dt;
    return Dx;
}

arma::mat acceleration( const arma::mat& X, float dt )
{
    arma::mat Dx_=velocity(X, dt);
    arma::mat Dx = arma::join_horiz(Dx_.col(0), Dx_);
    arma::mat Ddx = arma::diff(Dx,1,1) / dt;
    return Ddx;
}

arma::mat jerk( const arma::mat& X, float dt )
{
    arma::mat Ddx_ = acceleration(X, dt);
    arma::mat Ddx = arma::join_horiz(Ddx_.col(0), Ddx_);
    arma::mat Dddx = arma::diff(Ddx,1,1) / dt;
    return Dddx;
}

arma::vec speed( const arma::mat& X, float dt )
{
    arma::mat V = velocity(X, dt);
    V = V % V;
    arma::rowvec v = arma::sum(V, 0);
    return arma::sqrt(v).t();
}

arma::vec col_norm( const arma::mat& X )
{
    return arma::sqrt(arma::sum(X,0)).t();
}

arma::vec normalizeSignal( const arma::vec& X )
{
    return (X-arma::min(X))/(arma::max(X)-arma::min(X));
}

arma::mat rotX3d( double theta, bool affine )
{
	int d = affine?4:3;
	arma::mat m = arma::eye(d,d);

	double ct = cos(theta);
	double st = sin(theta);

	m(1,1) = ct; m(1,2) = -st;
	m(2,1) = st; m(2,2) = ct;

	return m;
}

arma::mat rotY3d( double theta, bool affine )
{
	int d = affine?4:3;
	arma::mat m = arma::eye(d,d);

	double ct = cos(theta);
	double st = sin(theta);

	m(0,0) = ct; m(0,2) = st;
	m(2,0) = -st; m(2,2) = ct;

	return m;
}

arma::mat rotZ3d( double theta, bool affine )
{
	int d = affine?4:3;
	arma::mat m = arma::eye(d,d);

	double ct = cos(theta);
	double st = sin(theta);

	m(0,0) = ct; m(0,1) = -st;
	m(1,0) = st; m(1,1) = ct;

	return m;
}

arma::mat trans3d( const arma::vec& xyz )
{
	arma::mat m = arma::eye(4,4);
	m(0,3) = xyz[0];
	m(1,3) = xyz[1];
	m(2,3) = xyz[2];
	return m;
}

arma::mat trans3d( double x, double y, double z )
{
	return trans3d( arma::vec({x, y, z}) );
}

arma::mat scaling3d( const arma::vec& xyz, bool affine )
{
	int d = affine?4:3;
	arma::mat m = arma::eye(d,d);

	m(0,0) = xyz[0];
	m(1,1) = xyz[1];
	m(2,2) = xyz[2];
	return m;
}

arma::mat scaling3d( double s, bool affine )
{
	return scaling3d( arma::vec({s, s, s}), affine );
}


arma::mat scaling3d( double x, double y, double z, bool affine )
{
	return scaling3d( arma::vec({x, y, z}), affine );
}

arma::mat rot2d( double theta, bool affine )
{
	int d = affine?3:2;
	arma::mat m = arma::eye(d,d);

	double ct = cos(theta);
	double st = sin(theta);

	m(0,0) = ct; m(0,1) = -st;
	m(1,0) = st; m(1,1) = ct;

	return m;
}

arma::mat trans2d( const arma::vec& xy, bool affine )
{
	arma::mat m = arma::eye(3,3);
	m(0,2) = xy[0];
	m(1,2) = xy[1];
	return m;
}

arma::mat trans2d( double x, double y, bool affine )
{
	return trans2d( arma::vec({x, y}) );
}

arma::mat scaling2d( const arma::vec& xy, bool affine )
{
	int d = affine?3:2;
	arma::mat m = arma::eye(d,d);

	m(0,0) = xy[0];
	m(1,1) = xy[1];
	return m;
}

arma::mat scaling2d( double s, bool affine )
{
	return scaling2d( arma::vec({s, s}), affine );
}

arma::mat scaling2d( double x, double y, bool affine )
{
	return scaling2d( arma::vec({x, y}), affine );
}


void x44( arma::mat & m, const arma::vec & v )
{
	m.submat(0,0,2,0) = v;
}


void y44( arma::mat & m, const arma::vec & v )
{
	m.submat(0,1,2,1) = v;
}

void z44( arma::mat & m, const arma::vec & v )
{
	m.submat(0,2,2,2) = v;
}

void trans44( arma::mat & m, const arma::vec & v )
{
	m.submat(0,3,2,3) = v;
}

arma::vec x44( const arma::mat & m )
{
	return m.submat(0,0,2,0);
}

arma::vec y44( const arma::mat & m )
{
	return m.submat(0,1,2,1);
}

arma::vec z44( const arma::mat & m )
{
	return m.submat(0,2,2,2);
}

arma::vec trans44( const arma::mat & m )
{
	return m.submat(0,3,2,3);
}



void x33( arma::mat & m, const arma::vec & v )
{
	m.submat(0,0,1,0) = v;
}


void y33( arma::mat & m, const arma::vec & v )
{
	m.submat(0,1,1,1) = v;
}


void trans33( arma::mat & m, const arma::vec & v )
{
	m.submat(0,2,1,2) = v;
}

arma::vec x33( const arma::mat & m )
{
	return m.submat(0,0,1,0);
}

arma::vec y33( const arma::mat & m )
{
	return m.submat(0,1,1,1);
}

arma::vec trans33( const arma::mat & m )
{
	return m.submat(0,2,1,2);
}

    
M44	perspective(double fov, double aspect, double znear, double zfar, bool wFriendly )
{
    M44 m = arma::zeros(4,4);
    
    double yscale = 1.0f / tanf(fov/2);
    double xscale = yscale / aspect;
    
    m(0,0) = xscale;
    m(1,1) = yscale;
    
    if(wFriendly)
    {
        m(2,2) = znear / ( zfar - znear );
        m(2,3) = (zfar * znear) / ( zfar - znear );
    }
    else
    {
        m(2,2) = zfar / ( znear - zfar );
        m(2,3) = znear*zfar/(znear - zfar ) ;
    }
    
    m(3,2) = -1.0f;
    return m;
}
    
M44 frustum( double left, double right, double bottom, double top, double near, double far )
{
    M44 m = arma::zeros(4,4);

    float x = (2.0*near) / (right-left);
    float y = (2.0*near) / (top-bottom);
    float a = (right+left)/(right-left);
    float b = (top+bottom)/(top-bottom);
    float c = -(far+near)/(far-near);
    float d = -(2.0*far*near)/(far-near);
    
    m(0,0) = x; m(0,1) = 0; m(0,2) = a; m(0,3) = 0;
    m(1,0) = 0; m(1,1) = y; m(1,2) = b; m(1,3) = 0;
    m(2,0) = 0; m(2,1) = 0; m(2,2) = c; m(2,3) = d;
    m(3,0) = 0; m(3,1) = 0; m(3,2) = -1.0f; m(3,3) = 0;

    return m;
}
    
M44 orthoOffCenter(double l, double r, double b, double t , double zn, double zf )
{
    M44 m = arma::eye(4,4);
    
    m(0,0) = 2/(r-l);
    m(1,1) = 2/(t-b);
    m(2,2) =  1/(zn-zf);
    m(0,3) = (l+r)/(l-r);
    m(1,3) = (t+b)/(b-t);
    m(2,3) = zn/(zn-zf);
    
    return m;
}

M44 ortho( double w, double h, double znear, double zfar )
{
    return orthoOffCenter( -w/2, w/2, -h/2, h/2, znear, zfar );
}
    
M44 pointAt(const V3 & pos, const V3 & target, const V3 & up )
{
    M44 m = arma::eye(4,4);
    
    ///calc axes.
    V3 z = normalize(target - pos);
    
    V3 x = normalize( cross( up, z ) );
    V3 y = normalize( cross( z, x ) );
    
    m(0,0) = x.x;   m(1,0) = x.y;   m(2,0) = x.z;
    m(0,1) = y.x;   m(1,1) = y.y;   m(2,1) = y.z;
    m(0,2) = z.x;   m(1,2) = z.y;   m(2,2) = z.z;
    m(0,3) = pos.x; m(1,3) = pos.y; m(2,3) = pos.z;

    return m;
}
    
M44 lookAt(const V3 & pos, const V3 & target, const V3 & up )
{
    M44 m = arma::eye(4,4);

    ///calc axes.
    V3 z = normalize(target - pos);
    V3 x = normalize( cross( up, z ) );
    V3 y = normalize( cross( z, x ) );

    m(0,0) = x.x;   m(1,0) = x.y;   m(2,0) = x.z;
    m(0,1) = y.x;   m(1,1) = y.y;   m(2,1) = y.z;
    m(0,2) = z.x;   m(1,2) = z.y;   m(2,2) = z.z;
    m = m.t();
    m = m * trans3d(-pos);
    return m;
}

    
    
    V4 quatMul( const V4&a, const V4& b )
    {
        V4 o;
        o.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
        o.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
        o.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
        o.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
        return o;
    }
    
    
    V4 quat( const M44& m, double eps )
    {
        V4 o;
        // Adapted from G3d engine (http://g3d.cs.williams.edu/g3d/G3D10/G3D.lib/source/Quat.cpp)
        static const int plus1mod3[] = {1, 2, 0};
        
        // Find the index of the largest diagonal component
        // These ? operations hopefully compile to conditional
        // move instructions instead of branches.
        int i = (m(1,1) > m(0,0)) ? 1 : 0;
        i = (m(2,2) > m(i,i)) ? 2 : i;
        
        // Find the indices of the other elements
        int j = plus1mod3[i];
        int k = plus1mod3[j];
        
        double nc2 = ((m(j,j) + m(k,k)) - m(i,i)) - 1.0;
        o[i] =  nc2;
        o[3] =  (m(j,k) - m(k,j));
        o[j] = -(m(i,j) + m(j,i));
        o[k] = -(m(i,k) + m(k,i));
        
        // We now have the correct result with the wrong magnitude, so normalize it:
        double s = arma::norm(o);
        if (s > eps) {
            s = 1.0 / s;
            o.x *= s;
            o.y *= s;
            o.z *= s;
            o.w *= s;
        } else {
            // nearly zero so make identity
            o.x = 0.0f;
            o.y = 0.0f;
            o.z = 0.0f;
            o.w = 1.0f;
        }
        
        return o;
    }
    
    M44 m44( const V4& q_ )
    {
        V4 q = normalize(q_);
        
        double xx = 2.0f * q.x * q.x;
        double xy = 2.0f * q.x * q.y;
        double xz = 2.0f * q.x * q.z;
        double xw = 2.0f * q.x * q.w;
        
        double yy = 2.0f * q.y * q.y;
        double yz = 2.0f * q.y * q.z;
        double yw = 2.0f * q.y * q.w;
        
        double zz = 2.0f * q.z * q.z;
        double zw = 2.0f * q.z * q.w;
        
        M44 m = arma::eye(4,4);
        m.col(0) = V4( 1.0f - yy - zz, xy + zw, xz - yw, 0.);
        m.col(1) = V4( xy - zw, 1.0 - xx - zz, yz + xw, 0.);
        m.col(2) = V4( xz + yw, yz - xw, 1.0 - xx - yy, 0.);
        return m;
    }
    
    V4 quatAxisAngle( const V3& axis, double angle )
    {
        V4 o;
        double hang = angle * 0.5f;
        double sa = sin(hang); // /2?
        o.x = axis.x * sa;
        o.y = axis.y * sa;
        o.z = axis.z * sa;
        o.w = cos(hang);
        return o;
    }
    
    V4 quatIdentity()
    {
        return V4(0,0,0,1);
    }
    
    V3 quatAxis( const V4& q, double eps )
    {
        double scale = sin( acos( q.w ) );
        
        if ( fabs(scale) < eps )
            return V3(0,0,0);
        else
            return  (V3)(V3(q.x,q.y,q.z) / scale);
    }
    
    /// from matrix TQuaternion FAQ
    V4 quatEuler( double x, double y, double z )
    {
        V4 qx(V3(1,0,0),x);
        V4 qy(V3(0,1,0),y);
        V4 qz(V3(0,0,1),z);
        
        V4 qt = quatMul(qx, qy);
        return quatMul(qt, qz);
    }
    
    V4 quatEuler( const V3& v )
    {
        return quatEuler(v.x,v.y,v.z);
    }
    
    
    // in radians
    V4 quatSetAngle( const V4& q, double theta)
    {
        V3 v = quatAxis(q);
        V4 o;
        o.w = cos(theta/2.0);
        v *= sin(theta/2.0);
        o.x = v.x;
        o.y = v.y;
        o.z = v.z;
        return o;
    }
    
    double quatAngle( const V4& q )
    {
        return 2.0 * acos(q.w);
    }
    
    V4 quatConjugate( const V4& q )
    {
        return V4(-q.x,-q.y,-q.z,q.w);
    }
    
    V4 slerp( const V4& a, const V4& b, double t, double minAngle )
    {
        if (t <= 0.0)
            return  a;
        if (t >= 1.0)
            return b;
        
        // angle between rotations
        double cosphi = dot(a,b);
        
        V4 qa = a;
        
        if (cosphi < 0)
        {
            // Change the sign and fix the dot product; we need to
            // loop the other way to get the shortest path
            qa = -qa;
            cosphi = -cosphi;
        }
        
        if(cosphi < -1.0)
            cosphi = -1.0;
        if(cosphi > 1.0)
            cosphi = 1.0;
        
        double phi = acos(cosphi);
        
        if (phi >= minAngle)
        {
            double s0 = sin((1.0 - t) * phi);
            double s1 = sin(t * phi);
            //return arma::vec({3,2,3,5});
            return  V4(( qa*s0 + b*s1 ) / sin(phi));
        }
        
        // For small angles, linear interpolate
        return  lerp(qa,b,t);
    }
    
}