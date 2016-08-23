#pragma once
#include "cm.h"

namespace cm
{

void randomSeed( float seed );

//arma::vec pdfGauss( const arma::mat &X, const arma::vec& mu, const arma::mat& Sigma );
arma::vec pdfVonMises( const arma::vec& theta, double mu, double sigma );

// Returns eigenvalues and eigenvectors sorted by decreasing eigenvalue 
void eigs_descend( arma::vec & d_out, arma::mat & V_out, const arma::mat & X );

arma::mat velocity( const arma::mat& X_, float dt);
arma::mat acceleration( const arma::mat& X, float dt );
arma::mat jerk( const arma::mat& X, float dt );
arma::vec speed( const arma::mat& X, float dt );

/// Column wise norm
arma::vec col_norm( const arma::mat& X );

arma::vec normalizeSignal( const arma::vec& X );

/// Handy for armadillo matrices
/// 3d transformations
//@{
arma::mat rotX3d( double theta, bool affine=true );
arma::mat rotY3d( double theta, bool affine=true );
arma::mat rotZ3d( double theta, bool affine=true );
arma::mat trans3d( const arma::vec& xyz );
arma::mat trans3d( double x, double y, double z );
arma::mat scaling3d( const arma::vec& xyz, bool affine=true );
arma::mat scaling3d( double s, bool affine=true );
arma::mat scaling3d( double x, double y, double z, bool affine=true );
//@}

/// 2d transformations
//@{
arma::mat rot2d( double theta, bool affine=true );
arma::mat trans2d( const arma::vec& xy, bool affine=true );
arma::mat trans2d( double x, double y, bool affine=true );
arma::mat scaling2d( const arma::vec& xy, bool affine=true );
arma::mat scaling2d( double s, bool affine=true );
arma::mat scaling2d( double x, double y, bool affine=true );
//@}

/// Utils for getting and settings columns of matrix
//@{
void x44( arma::mat& m, const arma::vec& v );
void y44( arma::mat& m, const arma::vec& v );
void z44( arma::mat& m, const arma::vec& v );
void trans44( arma::mat& m, const arma::vec& v );

arma::vec x44( const arma::mat& m );
arma::vec y44( const arma::mat& m );
arma::vec z44( const arma::mat& m );
arma::vec trans44( const arma::mat& m );
    
void x33( arma::mat& m, const arma::vec& v );
void y33( arma::mat& m, const arma::vec& v );
void trans33( arma::mat& m, const arma::vec& v );

arma::vec x33( const arma::mat& m );
arma::vec y33( const arma::mat& m );
arma::vec trans33( const arma::mat& m );
//@}

// Projective/camera transformations
M44 perspective(double fov, double aspect, double znear, double zfar, bool wFriendly=false);
M44 frustum( double left, double right, double bottom, double top, double near, double far );
M44 orthoOffCenter(double l, double r, double b, double t , double zn, double zf );
M44 ortho( double w, double h, double znear, double zfar );
M44 pointAt(const V3 & pos, const V3 & target, const V3 & up );
M44 lookAt(const V3 & pos, const V3 & target, const V3 & up );

// Quaternions
V4 quatMul( const V4&a, const V4& b );
V4 quat( const M44& m, double eps=1e-10 );
M44 m44( const V4& q_ );
V4 quatAxisAngle( const V3& axis, double angle );
V4 quatIdentity();
V3 quatAxis( const V4& q, double eps=1e-10 );
V4 quatEuler( double x, double y, double z );
V4 quatEuler( const V3& v );
V4 quatSetAngle( const V4& q, double theta);
double quatAngle( const V4& q );
V4 quatConjugate( const V4& q );
V4 slerp( const V4& a, const V4& b, double t, double minAngle=0.001 );

template <typename T, typename FuncT, typename DFuncT >
float newtonRaphson( FuncT f, DFuncT df, T a, T b, int maxiter=130, T tol=1.0e-5 );

// Other utils
CM_INLINE float csc( float x ) { return 1.0/sin(x); }
CM_INLINE float ctg( float x ) { return 1.0/tan(x); }

template <typename T>
T   scurve( T t )   {return ( t * t * (3.0 - 2.0 * t)); }

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// IMPL

template <typename T, typename FuncT, typename DFuncT >
float newtonRaphson( FuncT f, DFuncT df, T a, T b, int maxiter, T tol )
{
    T fa = f(a);

    if(fa == 0.0)
        return a;
    
    T fb = f(b);
    
    if(fb == 0.0)
        return b;
            
    if(fabs(fa*fb) < tol) // ??
        return a;
    /*
    if(fa*fb > 0.0)
    {
        printf("Root is not bracketed!\n");
        assert(0);
        return 0.0;
    }
    */
    T x = 0.5*(a+b);
    
    T fx,dx;
    
    for( int i = 0; i < maxiter; i++ )
    {
        fx = f(x);
        
        if( fabs(fx) < tol )
        {
            // found solution
            return x;
        }
        
        // tighten brackets
        if( fa*fx < 0.0 )
            b = x;
        else
        {
            a = x;
            fa = fx;
        }
        
        T dfx = df(x);
        
        if( dfx == 0.0 )
        {
            dx = b-a;
        }
        else
        {
            dx = -fx / dfx;
        }
        
        x = x+dx;
        
        // if result is outside of brackets bisect
        if( (b - x)*(x - a) < 0.0 )
        {
            dx = 0.5*(b-a);
            x = a + dx;
        }
        
        // check if converged
        if( fabs(dx) < tol*std::max(fabs(b), 1.0) )
        {
            return x;
        }
    }
    
    printf("Over maximum number of iterations\n");
    assert(0);
    return x;
}




}
