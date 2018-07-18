/********************************************************************
 --------------------------------------------------------------------
 --           _,(_)._
 --      ___,(_______).      ____
 --    ,'__.           \    /\___\-.
 --   /,' /             \  /  /     \
 --  | | |              |,'  /       \
 --   \`.|                  /    ___|________
 --    `. :           :    /     COLORMOTOR mini
 --      `.            :.,'        Graphics and Multimedia Framework
 --        `-.________,-'          © Daniel Berio
 --								   http://www.enist.org
 --								   drand48@gmail.com
 --
 --------------------------------------------------------------------
********************************************************************/

#pragma once

// Internal include
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>  //for ostringsream
#include <iomanip>  //for setprecision
#include <errno.h>
#include <functional>
#include <fstream>
#include <thread>
#include <complex>
typedef std::complex<double> complexd;
typedef std::complex<float> complexf;


#include <unistd.h>

#pragma clang diagnostic push // Ignores some warnings for armadillo mat44 etc
#pragma clang diagnostic ignored "-Wuninitialized"
#include "armadillo"
#pragma clang diagnostic pop

#define SAFE_DELETE( val )	if(val) { delete val; val=0; }
#define SAFE_DELETE_ARRAY( val ) if(val) { delete [] val; val = 0; }
#define ARRAYSIZE(v)      ((int)(sizeof(v)/sizeof(*v)))
#define STRINGIFY( expr ) #expr

#define debugPrint printf

// Detect platform
#ifdef _WIN32

////////////////////////
#define CM_WIN
#include <windows.h>
#ifdef _WIN64
// 64 bit?
#endif

#elif __APPLE__

#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
#error "Platform unsupported"
#elif TARGET_OS_IPHONE
#error "Platform unsupported"
#elif TARGET_OS_MAC
    ////////////////////////
    #define CM_OSX
    #define CM_MAC
#else
#   error "Unknown Apple platform"
#endif
#elif __linux__
    ////////////////////////
    #define CM_LINUX

#elif __unix__ // all unices not caught above
    #define CM_LINUX
#elif defined(_POSIX_VERSION)
    #define CM_LINUX
#else
#   error "Unknown compiler"
#endif

#if (defined CM_OSX || defined CM_LINUX)
#include <sys/time.h>
#endif

#ifdef CM_LINUX 
#include <mutex>
#endif



#ifdef CM_WIN
#pragma warning(disable : 4244) // conversion from 'int' to 'float' possible loss of data
#pragma warning(disable : 4267) // conversion from 'size_t' to 'int', possible loss of data
#pragma warning(disable : 4305) //'argument' : truncation from 'double' to 'float'
#pragma warning(disable : 4018) // signed/unsigned mismatch
#pragma warning(disable : 4996) // deprecated fopen etc
#define CM_INLINE	__inline
#else
#define CM_INLINE inline
#define CALLBACK
#endif

// Ext vector types
typedef float float4 __attribute__((ext_vector_type(4)));
typedef float float3 __attribute__((ext_vector_type(3)));
typedef float float2 __attribute__((ext_vector_type(2)));
CM_INLINE float4 _float4(float x, float y, float z, float w) { return {x,y,z,w}; }
CM_INLINE float3 _float3(float x, float y, float z) { return {x,y,z}; }
CM_INLINE float2 _float2(float x, float y) { return {x,y}; }


namespace cm
{

unsigned int isPowerOf2(unsigned int x);
int nextPowerOf2(int n);
#define NPOT(n) nextPowerOf2(n)

inline unsigned int isPowerOf2(unsigned int x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

inline int nextPowerOf2(int n)
{
    if (isPowerOf2(n)) return n;
    int prevn = n;
    while(n &= n-1)
        prevn = n;
    return prevn * 2;
} 

inline int nd16(int n)
{
    int r = n%16;
    if (r)
        n = n + (16 - r);
    return n;
}

inline int nd4( int n )
{
    int r = n%4;
    if(r) n = n + (4 - r);    
    return n;
}

/// fuzzy float equality 
template <typename T>
bool    feq( T a, T b, T eps = 1e-10 )
{ return (fabs(b-a) < eps); }

/// Clamp in between lo and hi
template <typename T1, typename T2, typename T3>
T1   clamp( T1 in, T2 lo, T3 hi )
{
    if(in < lo) return lo; if(in > hi) return hi; return in;
}

/// return value wrapped in a b range
template <typename T>
T   wrap( T val, T a, T b )
{
    T range = b-a;
    
    while( val > b )
        val-=range;
    while( val < a )
        val+=range;
        
    return val;
}


/// Extensions on armadillo fixed vector types,
/// Quite bloated, but allows to use vector element accessors (x,y,z)
//@{

/// 2 Vector wrapper
class V2 : public arma::vec2
{
public:
    template <typename T2, typename eglue_type>
    V2( const arma::eGlue<V2, T2, eglue_type>& v)
    :
    arma::vec2(),
    x(this->at(0)),
    y(this->at(1))
    {
        x = v.at(0,0); y = v.at(1,0);
    }
    
                   
    V2(const V2& v)
    :
    arma::vec2(),
    x(this->at(0)),
    y(this->at(1))
    {
        x = v.at(0); y = v.at(1);
    }
    
    V2(const arma::vec& v)
    :
    arma::vec2(),
    x(this->at(0)),
    y(this->at(1))
    {
        x = v.at(0); y = v.at(1);
    }
    
    V2(const arma::subview_col<double>& v)
    :
    arma::vec2(),
    x(this->at(0)),
    y(this->at(1))
    {
        x = v.at(0,0); y = v.at(1,0);
    }
    
    
    V2()
    :
    arma::vec2(),
    x(this->at(0)),
    y(this->at(1))
    {
    }
    
    V2(double x, double y)
    :
    arma::vec2({x,y}),
    x(this->at(0)),
    y(this->at(1))
    {
        
    }
    
    V2& operator = (const arma::vec& v) {x = v.at(0); y = v.at(1); return *this; }
    V2& operator = (const V2& v) {x = v.at(0); y = v.at(1); return *this; }
    
    double &x;
    double &y;
};
    

/// 3 Vector wrapper
class V3 : public arma::vec3
{
public:
    V3(const arma::subview_col<double>& v)
    :
    arma::vec3(),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2))
    {
        x = v.at(0,0); y = v.at(1,0); z = v.at(2,0);
    }
    
    
    template <typename T2, typename eglue_type>
    V3( const arma::eGlue<V3, T2, eglue_type>& v)
    :
    arma::vec3(),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2))
    {
        x = v.at(0,0); y = v.at(1,0); z = v.at(2,0);
    }
    
    V3(const V3& v)
    :
    arma::vec3(),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2))
    {
        x = v.at(0); y = v.at(1); z = v.at(2);
    }
    
    
    V3(const arma::vec& v)
    :
    arma::vec3(),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2))
    {
        x = v.at(0); y = v.at(1); z = v.at(2);
    }
    
    V3()
    :
    arma::vec3(),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2))
    {
    }
    
    V3(double x, double y, double z)
    :
    arma::vec3({x,y,z}),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2))
    {
        
    }
    
    V3(const V2& xy, double z)
    :
    arma::vec3({xy.x,xy.y,z}),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2))
    {

    }
    
    V3& operator = (const arma::vec& v) {x = v.at(0); y = v.at(1); z = v.at(2); return *this; }
    V3& operator = (const V3& v) {x = v.at(0); y = v.at(1); z = v.at(2); return *this; }
    
    V2 xy() const { return V2(x,y); }
    
    double &x;
    double &y;
    double &z;
};

/// 4 Vector wrapper
class V4 : public arma::vec4
{
public:
    template <typename T2, typename eglue_type>
    V4( const arma::eGlue<V4, T2, eglue_type>& v)
    :
    arma::vec4(),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2)),
    w(this->at(3))
    {
        x = v.at(0,0); y = v.at(1,0); z = v.at(2,0); w = v.at(3,0);
    }

    
    V4(const V4& v)
    :
    arma::vec4(),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2)),
    w(this->at(3))
    {
        x = v.at(0); y = v.at(1); z = v.at(2); w = v.at(3);
    }
    
    V4(const arma::vec& v)
    :
    arma::vec4(),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2)),
    w(this->at(3))
    {
        x = v.at(0); y = v.at(1); z = v.at(2); w = v.at(3);
    }
    
    V4()
    :
    arma::vec4(),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2)),
    w(this->at(3))
    {
    }
    
    V4(double x, double y, double z, double w)
    :
    arma::vec4({x,y,z,w}),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2)),
    w(this->at(3))
    {
        
    }
    
    V4(const V3& xyz, double w)
    :
    arma::vec4({xyz.x,xyz.y,xyz.z,w}),
    x(this->at(0)),
    y(this->at(1)),
    z(this->at(2)),
    w(this->at(3))
    {

    }

    V4& operator = (const arma::vec& v) { x = v.at(0); y = v.at(1); z = v.at(2); w = v.at(3); return *this; }
    V4& operator = (const V4& v) { x = v.at(0); y = v.at(1); z = v.at(2); w = v.at(3); return *this; }

    V2 xyz() const { return V3(x,y,z); }
    
    /// For colors 
    V4& alpha(double v) { w=v; return *this; }

    double &x;
    double &y;
    double &z;
    double &w;
};

typedef arma::mat22 M22;
typedef arma::mat33 M33;
typedef arma::mat44 M44;

/// Construct 4x4 matrix from float buf
CM_INLINE M44 m44(float * buf)
{
    M44 m = arma::eye(4,4);
    m.col(0) = V4(buf[0], buf[1], buf[2], buf[3]);
    m.col(1) = V4(buf[4], buf[5], buf[6], buf[7]);
    m.col(2) = V4(buf[8], buf[9], buf[10], buf[11]);
    m.col(3) = V4(buf[12], buf[13], buf[14], buf[15]);
    return m;
}

/// Matrix vector multiply, handles also affine transforms.
CM_INLINE arma::vec mul(const arma::mat& m, const arma::vec& v)
{
    arma::vec va = arma::ones(m.n_cols,1);
    va.subvec(0,v.size()-1) = v;
    va = m*va;
    return va.subvec(0,v.size()-1);
}

CM_INLINE arma::vec normalize( const arma::vec& v ) { return v / arma::norm(v); }
CM_INLINE V2 perp( const V2& v ) { return V2(-v.y, v.x); }
CM_INLINE arma::vec lerp( const arma::vec& a, const arma::vec& b, double t ) { return a + (b-a)*t; }
CM_INLINE V3 cross( const V3& a, const V3& b ) { return (V3)arma::cross(a,b); }
    
//@}

}

// Defs 

// http://www.youtube.com/watch?v=eDiSYp_51iY&feature=related
#ifndef PI
#define PI				3.14159265358979323846
#endif

#define TWOPI           6.283185307179586476925287
#define PID2            1.570796326794896619231322
#define dia80PI			57.29577950560105
#define PIDIV180		0.0174532925199433

#define SQRTOFTWOINV 	0.707107
#define SQRTOFTWO		1.414213562373095

#define PHI				1.618033988749895
#define PHISQUARED		2.618033988749895
#define PHIINV			0.618033988749895

#ifndef ONETHIRD
#define ONETHIRD 0.333333333333333333333333333333333333333333333333333333333333
#endif

#define EPSILON 4.37114e-05

/// approx 1.0/255.0
#define DIV255			0.00392156862745098


namespace cm
{
    
//#define UNDEFINED		INFINITY
template <typename T> T signum(T val) 
{
    if( val >= 0 )
    	return 1;
    return -1;
}

CM_INLINE double radians( double x ) { return PIDIV180*(x); }
CM_INLINE double degrees( double x ) { return x * dia80PI; }

CM_INLINE float sqr( float x ) { return x*x; }

/// Random float between 0 and 1
#ifdef CM_WIN
CM_INLINE float	frand() { return (float)(rand())/RAND_MAX; }
#else
CM_INLINE float	frand() { return drand48(); }
#endif

/// Random float between a and b
CM_INLINE float frand( float a, float b ) { return a+frand()*(b-a); }

/// Random double between 0 and 1
CM_INLINE double drand() { return (double)(rand())/RAND_MAX; }

/// Random double between a and b
CM_INLINE double drand( double a, double b ) { return a+drand()*(b-a); }

/// random number returns a >= a && a < b
CM_INLINE int random( int a, int b ) { return a+(rand()%(b-a)); }

/// get rounded value
CM_INLINE float	roundf( float x) { return floorf((x) + 0.5f); }

/// Bernoulli distribtion, basically a coin flip with a fancy name
CM_INLINE bool bernoulli( float p=0.5 ) { if(frand() < p) return true; return false; }
    
/// Till homebrew armadillo supports regspace!
CM_INLINE arma::vec regspace( double a, double inc, double b )
{
	int n = 1 + std::floor(fabs(a-b) / inc);
	arma::vec x = arma::vec(n);
	double sgn = 1.;
	if(a > b)
		sgn = -1.;

	for( int i = 0; i < n; i++ )
		x[i] = a + sgn*inc*i;
	x[n-1] = b;
	return x;
}
    
CM_INLINE arma::uvec range( int a, int b )
{
    int n = b-a;
    return arma::linspace<arma::uvec>(a, b-1, n);
}


/// Argmax for armadillo vectors
template <class T>
int argmax( const T& x )
{
    arma::uword ind;
    x.max(ind);
    return ind;
}

/// Vector arctangent 2
CM_INLINE arma::vec atan2( const arma::vec& y, const arma::vec& x )
{
    arma::vec theta = arma::zeros(y.size());
    for( int i = 0; i < y.size(); i++ )
        theta[i] = ::atan2(y[i], x[i]);
    return theta;
}


/// ref counting object
class  RefCounter
{
public:
    RefCounter()
    {
        refCount = 0;
    }
    
    virtual ~RefCounter()
    {
        refCount = 0;
    }
    
    void	incRefCount() { refCount++; }
    void	decRefCount() { if(refCount)refCount--; }
    int		getRefCount() { return refCount; }
private:
    int		refCount;
    
};
    
/// Delete object if refcount is 0
#define REFDELETE( val )  if(val) { val->decRefCount(); if(val->getRefCount()==0) delete val; val = 0; }
    
template <class T>
class RefPtr
{
public:
    RefPtr()
    {
        ptr = 0;
    }
    
    ~RefPtr()
    {
        REFDELETE(ptr);
    }
    
    RefPtr( const RefPtr<T> & ptr_ )
    {
        ptr = 0;
        set(ptr_.ptr);
    }
    
    
    RefPtr( T * ptr_ )
    {
        ptr = 0;
        set(ptr_);
    }
    
    inline RefPtr<T> &	operator=( const RefPtr<T> & ptr_ )
    {
        set(ptr_.ptr);
        return *this;
    }
    
    inline RefPtr<T> &	operator=( T * ptr_ )
    {
        set(ptr_);
        return *this;
    }
    
    inline void	set( T * ptr_ )
    {
        REFDELETE(ptr);
        if(ptr_)
            ptr_->incRefCount();
        ptr = ptr_;
    }
    
    inline operator T * () const { return ptr; }
    //inline operator const T * () const { return ptr; }
    
    inline T *			operator->() const { return ptr; }
    //inline const T *	operator->() const {  return ptr; }
    
    inline T &			operator*()
    {
        return *ptr;
    }
    
    inline const T &		operator*() const
    {
        return *ptr;
    }
    
    
    
    template <typename S> inline operator RefPtr<S> ()
    {
#ifdef	_DEBUG
        // Type checking
        S * pS = static_cast<S *>( static_cast<T *>( *this ));
#endif
        return *reinterpret_cast<RefPtr<S> *>( this );
    }
    
    
    template <typename S> inline RefPtr<T> &	operator=( S * p )
    {
        set(p);
        return *this;
    }
    
    
    inline bool operator ! () const { return ptr == 0; }
    //inline bool operator == ( void * p ) const	{ return (void*)ptr == p; }
    //inline bool operator == ( const RefPtr<T> & p )	const { return ptr == p.ptr; }
    
    //inline bool operator != ( void * p )			const	{ return (void*)ptr != p; }
    //inline bool operator != ( const RefPtr<T> & p )	const { return ptr != p.ptr; }
    
    //inline bool operator > ( void* p )			const	{ return (void*)ptr > p; }
    //inline bool operator > ( const RefPtr<T> & p )	const{ return ptr > p.ptr; }
    
    //inline bool operator < ( void * p )			const	{ return (void*)ptr < p; }
    //inline bool operator < ( const RefPtr<T> & p )const	{ return ptr < p.ptr; }
    
    T* ptr;
};

    
}

// Necessary for conversions
//namespace arma
//{
template<>
struct arma::is_arma_type<cm::V2>
{
    static const bool value=true;
};

template<>
struct arma::is_arma_type<cm::V3>
{
    static const bool value=true;
};
    
template<>
struct arma::is_arma_type<cm::V4>
{
    static const bool value=true;
};

template<>
struct arma::is_same_type<cm::V2,cm::V2>
{
    static const bool value = true;
    static const bool yes   = true;
    static const bool no    = false;
};

template<>
struct arma::is_same_type<arma::vec2,cm::V2>
{
    static const bool value = true;
    static const bool yes   = true;
    static const bool no    = false;
};

template<>
struct arma::is_same_type<cm::V2, arma::vec2>
{
    static const bool value = true;
    static const bool yes   = true;
    static const bool no    = false;
};


//}

