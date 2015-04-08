/****************************************************
 *
 * A series of utilities for numeric manipulation with
 * the std library the numpy/matlab way.
 * Easy to use but very unoptimized since most functions
 * return by value.
 *
 * © Daniel Berio 2015
 * http://www.enist.org
 *
 ****************************************************/

#pragma once
#include "math/cmMathIncludes.h"
#include "math/cmMathDefs.h"
#include "math/cmVector.h"
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"
 
namespace cm
{
    typedef std::vector<float> vectorf;
    typedef std::vector<double> vectord;
    typedef std::vector<int> vectori;
    typedef std::vector<char> vectorc;
    typedef std::vector<unsigned char> vectoruc;
    typedef std::vector<Vec2> vector2f;
    
    typedef std::pair<float, float> pairf;
    typedef std::pair<double, double> paird;
    typedef std::pair<int, int> pairi;
    

    ///////////////////////////////////////////////////////
    // Vector ops
    
    template<typename T1,typename T2>
    std::vector<T1> operator + ( const std::vector<T1> & A, const std::vector<T2> & B );
    
    template<typename T1,typename T2>
    std::vector<T1> operator - ( const std::vector<T1> & A, const std::vector<T2> & B );
    
    template<typename T1,typename T2>
    std::vector<T1> operator + ( const std::vector<T1> & X, T2 v );
    
    template<typename T1,typename T2>
    std::vector<T1> operator - ( const std::vector<T1> & X, T2 v );

    template<typename T1,typename T2>
    std::vector<T1> operator - ( T2 v, const std::vector<T1> & X  );

    template<typename T1,typename T2>
    std::vector<T1> operator + ( T2 v, const std::vector<T1> & X  );

    template<typename T1,typename T2>
    std::vector<T1> operator * ( const std::vector<T1> & X, T2 v );
    
    template<typename T1,typename T2>
    std::vector<T1> operator * ( T2 v, const std::vector<T1> & X );
    
    template<typename T1,typename T2>
    std::vector<T1> operator * ( const std::vector<T1> & A, const std::vector<T2> & B );
    
    template<typename T1,typename T2>
    std::vector<T1> operator / ( const std::vector<T1> & A, const std::vector<T2> & B );

    
    template<typename T1,typename T2>
    std::vector<T1> operator / ( const std::vector<T1> & X, T2 v );
    
    template <typename T>
    std::vector<T> absv( const std::vector<T> & X );
    
    template <typename T>
    std::vector<T> cosv( const std::vector<T> & X );
    
    template <typename T>
    std::vector<T> sinv( const std::vector<T> & X );
    
    template <typename T1, typename T2>
    std::vector<T1> maximum( const std::vector<T1> & X, T2 v );
    
    template <typename T1, typename T2>
    std::vector<T1> minimum( const std::vector<T1> & X, T2 v );
    
    template<typename T1,typename T2>
    std::vector<T1> maximum ( const std::vector<T1> & A, const std::vector<T2> & B );
    
    template<typename T1,typename T2>
    std::vector<T1> minimum ( const std::vector<T1> & A, const std::vector<T2> & B );
    
    template <typename T>
    T min( const std::vector<T> & X );
    
    template <typename T>
    T max( const std::vector<T> & X );
    
    template <typename T>
    int maxIndex( const std::vector<T> & X );
    
    template <typename T>
    T absmin( const std::vector<T> & X );
    
    template <typename T>
    T absmax( const std::vector<T> & X );
    
    template <typename T>
    T sum( const std::vector<T> & X );
    
    template <typename T>
    std::vector<T> diff( const std::vector<T> & X );

    template <typename T>
    std::vector<T> normalize( const std::vector<T> & X, float lower = 0.0, float upper = 1.0 );
    
    template <typename T>
    std::vector<T> linspace( T a, T b, int n );

    template <typename T>
    std::vector<T> incspace( T start, T inc, int n );
    
    template <typename T>
    std::vector<T> range( const std::vector<T> & X, int a, int b );
    
    template <typename T, typename T2 >
    std::vector<T> filter( const std::vector<T> & X, T2 func );
    
    template <typename T, typename T2 >
    std::vector<T> map( const std::vector<T> & X, T2 func );
    
    /// Pad array for convolution
    enum
    {
        PAD_REPEAT = 0, /// <- Repeats the values at the extremities
        PAD_EXTRAPOLATE = 1, /// <- Extrapolates the values at the extremities
        PAD_WRAP, /// <- Wraps the values at the extremities
    };
    
    template <typename T>
    std::vector<T> pad1d( const std::vector<T> & S, int padding, int type=PAD_REPEAT );
    
    /// 1D correlation
    template <typename T>
    std::vector<T> correlate1d( const std::vector<T> & X, const std::vector<T> K, int padType=PAD_REPEAT  );
    
    /// 1D convolution
    template <typename T>
    std::vector<T> convolve1d( const std::vector<T> & X, const std::vector<T> K, int padType=PAD_REPEAT );
    
    // based on SCIPY implementation
    template <typename T, typename T2>
    std::vector<T> gaussian1d( const std::vector<T> & X, T2 sigma, int order=0, int padType=PAD_REPEAT, float truncate=4.0f  );
    
    template <typename T, typename T2>
    std::vector<T> smoothDiff( const std::vector<T> & X, T2 sigma );
    
    template <typename T, typename T2>
    std::vector<T> smoothDiff2( const std::vector<T> & X, T2 sigma );
    
    template <typename T, typename T2, typename T3>
    std::vector<int> maxima( const std::vector<T> & X, T2 diffSigma = 1.0f, T3 eps = 0.000001f, float thresh = 0.1, int offsetThresh = 15 );
    
    template <typename T, typename T2, typename T3>
    std::vector<int> extrema( const std::vector<T> & X, T2 diffSigma = 1.0, T3 eps = 0.000001, int offsetThresh = 15 );
    
    template <class T>
    vectori appendEndIndices( const vectori & I, const T & buf );
    
    template <typename T>
    std::vector<T> mergeSorted( const std::vector<T> & I1 , const std::vector<T> & I2 );
    
    template <typename T, typename FuncT, typename DFuncT >
    float newtonRaphson( FuncT f, DFuncT df, T a, T b, int maxiter=130, T tol=1.0e-5 );

    // A square matrix container
    template <typename T> struct matrix : public std::vector< std::vector<T> >
    {
    public:
        matrix()
        {
            
        }
        
        matrix( int r, int c, const T &val = T(0) )
        {
            this->assign(r,std::vector<T>());
            for( int i = 0; i < r; i++ )
                this->at(i).assign(c,val);
        }
        
        size_t rows() const { return this->size(); }
        
        // will only work if square
        size_t cols() const { return (rows())?this->at(0).size():0; }
        
        const T & operator()( int i, int j ) const
        {
            return this->at(j).at(i);
        }
        
        T & operator()( int i, int j )
        {
            return this->at(i).at(j);
        }
        
        
        void zeros()
        {
            for( int i = 0; i < rows(); i++ )
                for( int j = 0; j < cols(); j++ )
                {
                    this->at(i).at(j) = T(0);
                }
        }
        
        void incSize()
        {
            if(cols()==0)
            {
                addRow();
            }
            else
            {
                addRow();
                addColumn();
            }
        }
        
        void removeColumn( int n )
        {
            for( int i = 0; i < rows(); i++ )
                this->at(i).erase(this->at(i).begin()+n);
        }
        
        void removeRow( int n )
        {
            this->erase(this->begin()+n);
        }
        
        void addRow( const std::vector<T> & r )
        {
            this->push_back(r);
        }
        
        void addRow()
        {
            if(cols()==0)
                this->push_back( std::vector<T>(1) );
            else
                this->push_back( std::vector<T>(cols()) );
            
            for( int i = 0; i < cols(); i++ )
            {
                this->back().at(i) = 0;
            }
        }
        
        void addColumn()
        {
            for( int i = 0; i < this->size(); i++ )
                this->at(i).push_back(0);
        }
        
        /// We assume floats here
        operator cv::Mat ()
        {
            cv::Mat m(rows(),cols(),CV_32F);
            for( int i = 0; i < rows(); i++ )
                for( int j = 0; j < cols(); j++ )
                    m.at<float>(i,j) = (float)this->at(i).at(j);
            return m;
        }
        
    };
    
    typedef matrix<float> matrixf;
    typedef matrix<double> matrixd;
    typedef matrix<Vec2> matrix2f;
    typedef matrix<Vec3> matrix3f;
    
    matrixf covariance2( const std::vector<Vec2> & P );
    matrixf covariance3( const std::vector<Vec3> & P );
    
    template <typename T>
    T mean( const std::vector<T> & V );
    
    template <typename T>
    T variance( const std::vector<T> & V );
    
    template <typename T>
    T standardDeviation( const std::vector<T> & V );
    
    template <typename T>
    T maxDeviation( const std::vector<T> & V  );

///// IMPL
    
    template<typename T1,typename T2>
    std::vector<T1> operator + ( const std::vector<T1> & A, const std::vector<T2> & B )
    {
        int n = std::min(A.size(), B.size());
        std::vector<T1> Y(n);
        for( int i = 0; i < n; i++ )
            Y[i] = A[i] + B[i];
        return Y;
    }

    template<typename T1,typename T2>
    std::vector<T1> operator - ( const std::vector<T1> & A, const std::vector<T2> & B )
    {
        int n = std::min(A.size(), B.size());
        std::vector<T1> Y(n);
        for( int i = 0; i < n; i++ )
            Y[i] = A[i] - B[i];
        return Y;
    }

    template<typename T1,typename T2>
    std::vector<T1> operator + ( const std::vector<T1> & X, T2 v )
    {
        std::vector<T1> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = X[i] + v;
        return Y;
    }

    template<typename T1,typename T2>
    std::vector<T1> operator - ( const std::vector<T1> & X, T2 v )
    {
        std::vector<T1> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = X[i] - v;
        return Y;
    }
    
    template<typename T1,typename T2>
    std::vector<T1> operator - ( T2 v, const std::vector<T1> & X  )
    {
        std::vector<T1> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = v - X[i];
            return Y;
    }

    template<typename T1,typename T2>
    std::vector<T1> operator + ( T2 v, const std::vector<T1> & X  )
    {
        std::vector<T1> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = X[i] + v;
            return Y;
    }

    
    template<typename T1,typename T2>
    std::vector<T1> operator * ( const std::vector<T1> & X, T2 v )
    {
        std::vector<T1> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = X[i] * v;
        return Y;
    }

    template<typename T1,typename T2>
    std::vector<T1> operator * ( T2 v, const std::vector<T1> & X )
    {
        std::vector<T1> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = X[i] * v;
        return Y;
    }

    
    template<typename T1,typename T2>
    std::vector<T1> operator / ( const std::vector<T1> & X, T2 v )
    {
        std::vector<T1> Y(X.size());
        v = 1.0/v;
        for( int i = 0; i < X.size(); i++ )
            Y[i] = X[i] * v;
        return Y;
    }
    
    template<typename T1,typename T2>
    std::vector<T1> operator * ( const std::vector<T1> & A, const std::vector<T2> & B )
    {
        int n = std::min(A.size(), B.size());
        std::vector<T1> Y(n);
        for( int i = 0; i < n; i++ )
            Y[i] = A[i] * B[i];
        return Y;
    }
    
    template<typename T1,typename T2>
    std::vector<T1> operator / ( const std::vector<T1> & A, const std::vector<T2> & B )
    {
        int n = std::min(A.size(), B.size());
        std::vector<T1> Y(n);
        for( int i = 0; i < n; i++ )
            Y[i] = A[i] / B[i];
        return Y;
    }
    
    
    template <typename T>
    std::vector<T> absv( const std::vector<T> & X )
    {
        std::vector<T> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = ::fabs(X[i]);
        return Y;
    }
    
    template <typename T>
    std::vector<T> cosv( const std::vector<T> & X )
    {
        std::vector<T> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = ::cos(X[i]);
        return Y;
    }
    
    template <typename T>
    std::vector<T> sinv( const std::vector<T> & X )
    {
        std::vector<T> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = ::sin(X[i]);
        return Y;
    }
    
    template <typename T1, typename T2>
    std::vector<T1> maximum( const std::vector<T1> & X, T2 v )
    {
        std::vector<T1> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = std::max(X[i], (T1)v);
        return Y;
    }
    
    template <typename T1, typename T2>
    std::vector<T1> minimum( const std::vector<T1> & X, T2 v )
    {
        std::vector<T1> Y(X.size());
        for( int i = 0; i < X.size(); i++ )
            Y[i] = std::min(X[i], (T1)v);
        return Y;
    }
    
    template<typename T1,typename T2>
    std::vector<T1> maximum ( const std::vector<T1> & A, const std::vector<T2> & B )
    {
        int n = std::min(A.size(), B.size());
        std::vector<T1> Y(n);
        for( int i = 0; i < n; i++ )
            Y[i] = std::max(A[i], (T1)B[i]);
        return Y;
    }
    
    template<typename T1,typename T2>
    std::vector<T1> minimum ( const std::vector<T1> & A, const std::vector<T2> & B )
    {
        int n = std::min(A.size(), B.size());
        std::vector<T1> Y(n);
        for( int i = 0; i < n; i++ )
            Y[i] = std::min(A[i], (T1)B[i]);
        return Y;
    }
    
    
    template <typename T>
    T min( const std::vector<T> & X )
    {
        if(!X.size())
        {
            assert(0);
        }
        
        T v = X[0];
        for( int i = 1; i < X.size(); i++ )
            v = std::min(X[i], v);
        return v;
    }
    
    template <typename T>
    T max( const std::vector<T> & X )
    {
        if(!X.size())
        {
            assert(0);
        }
        
        T v = X[0];
        for( int i = 1; i < X.size(); i++ )
            v = std::max(X[i], v);
        return v;
    }
    
    template <typename T>
    int maxIndex( const std::vector<T> & X )
    {
        if(!X.size())
        {
            assert(0);
        }

        T v = X[0];
        int imax = 0;
        
        for( int i = 1; i < X.size(); i++ )
        {
            if( fabs(X[i]) > fabs(v) )
            {
                v = X[i];
                imax = i;
            }
        }
        
        return imax;
    }
    
    template <typename T>
    T absmin( const std::vector<T> & X )
    {
        if(!X.size())
        {
            assert(0);
        }
        
        T v = X[0];
        for( int i = 1; i < X.size(); i++ )
        {
            if( fabs(X[i]) < fabs(v) )
                v = X[i];
        }

        return v;
    }
    
    template <typename T>
    T absmax( const std::vector<T> & X )
    {
        if(!X.size())
        {
            assert(0);
        }
        
        T v = X[0];
        for( int i = 1; i < X.size(); i++ )
        {
            if( fabs(X[i]) > fabs(v) )
                v = X[i];
        }
        return v;
    }
    
    template <typename T>
    T sum( const std::vector<T> & X )
    {
        if(!X.size())
        {
            assert(0);
        }
        
        T v = X[0];
        for( int i = 1; i < X.size(); i++ )
            v += X[i];
        return v;
    }
    
    template <typename T>
    std::vector<T> diff( const std::vector<T> & X )
    {
        std::vector<T> Y(X.size()-1);
        for( int i = 0; i < X.size()-1; i++ )
            Y[i] = X[i+1] - X[i];
        return Y;
    }
    
    template <typename T>
    std::vector<T> normalize( const std::vector<T> & X, float lower, float upper )
    {
        std::vector<T> Y(X.size());
        T Xmin = min(X);
        T Xmax = max(X);
        T range = Xmax-Xmin;
        for( int i = 0; i < X.size(); i++ )
            Y[i] = lower + ((X[i]-Xmin)/range)*(upper-lower);
        return Y;
    }
    
    template <typename T>
    std::vector<T> incspace( T start, T inc, int n )
    {
        std::vector<T> X(n);
        for( int i = 0; i < n; i++ )
        {
            X[i] = start + inc*i;
        }
        
        return X;
    }
    
    template <typename T>
    std::vector<T> range( const std::vector<T> & X, int a, int b )
    {
        return std::vector<T> (X.begin() + a, X.begin() + b);
    }
    
    
    
    template <typename T, typename T2 >
    std::vector<T> filter( const std::vector<T> & X, T2 func )
    {
        std::vector<T> Y;
        for( int i = 0; i < X.size(); i++ )
        {
            if(func(X[i]))
                Y.push_back(X[i]);
        }
        return Y;
    }
    
    template <typename T, typename T2 >
    std::vector<T> map( const std::vector<T> & X, T2 func )
    {
        std::vector<T> Y(X.size());
        for( int i = 0; i < Y.size(); i++ )
            Y[i] = func(X[i]);
        return Y;
    }
    
    

    /// Pad array for convolution
    /// TODO: different padding types (mirror etc)
    template <typename T>
    std::vector<T> pad1d( const std::vector<T> & S, int padding, int type )
    {
        int n = S.size();
        
        std::vector<T> res(n + padding*2);
        for( int i = 0; i < n; i++ )
            res[i+padding] = S[i];
        
        if(type==PAD_REPEAT)
        {
            for( int i = 0; i < padding; i++ )
            {
                res[i] = S[0];
                res[n+padding+i] = S[n-1];
            }
        }
        else if( type==PAD_EXTRAPOLATE )
        {
            T dl = S[0]-S[1];
            T dr = S[n-1]-S[n-2];
            T l = S[0];
            T r = S[n-1];
            for( int i = 0; i < padding; i++ )
            {
                l += dl;
                r += dr;
                res[padding-1-i] = l;
                res[n+padding+i] = r;
            }
        }
        else if( type==PAD_WRAP )
        {
            for( int i = 0; i < padding; i++ )
            {
                res[i] = S[mod(-1-i, n)];
                res[n+padding+i] = S[mod(n+i, n)];
            }
        }
        
        return res;
    }
    
    /// 1D correlation
    template <typename T>
    std::vector<T> correlate1d( const std::vector<T> & X, const std::vector<T> K, int padType  )
    {
        if(! (K.size()&0x1) )
            assert(0);
        
        int hk = (K.size()-1)/2;
        
        std::vector<T> XP = pad1d(X, hk, padType);
        
        int n = X.size();
        
        std::vector<T> Y(n);
        
        int kernelSize = K.size();
        
        // we repeat the
        for ( int i = 0; i < n; i++ )
        {
            Y[i] = 0;
            
            for ( int j = 0; j < kernelSize; j++ )
            {
                Y[i] += XP[i+j] * K[j];
            }
        }
        
        return Y;
    }
    
    /// 1D convolution
    template <typename T>
    std::vector<T> convolve1d( const std::vector<T> & X, const std::vector<T> K, int padType  )
    {
        int n = K.size();
        std::vector<T> KR(n);
        for( int i = 0; i < n; i++ )
            KR[n-1-i] = K[i];
        return correlate1d(X, KR, padType);
    }
    
    
    // based on SCIPY implementation
    template <typename T, typename T2>
    std::vector<T> gaussian1d( const std::vector<T> & X, T2 sigma, int order, int padType, float truncate  )
    {
        int lw = (int)(truncate * sigma + 0.5);
        std::vector<T> K(2*lw+1);
        K[lw] = 1.0;
        float sum = 1.0;
        T sd = sigma*sigma;
        for( int i = 1; i <= lw; i++ )
        {
            float v = exp(-0.5 * (i*i) / sd);
            K[lw+i] = K[lw-i] = v;
            sum += 2.0 * v;
        }
        
        // normalize
        float div = 1.0/sum;
        for( int i=0; i < lw*2+1; i++ )
            K[i] *= div;
        
        // first derivative
        if(order == 1)
        {
            K[lw] = 0.0;
            for( int i = 1; i <= lw; i++ )
            {
                float x = i;
                float v = -x / sd * K[lw + i];
                K[lw + i] = -v;
                K[lw - i] = v;
            }
        }
        // second derivative
        else if(order == 2)
        {
            K[lw] *= -1.0 / sd;
            for( int i = 1; i <= lw; i++ )
            {
                float x = i;
                float v = (x * x / sd - 1.0) * K[lw + i] / sd;
                K[lw+i] = K[lw-i] = v;
            }
        }
        // todo third...
        
        return correlate1d(X, K, padType);
    }
    
    template <typename T, typename T2>
    std::vector<T> smoothDiff( const std::vector<T> & X, T2 sigma )
    {
        return gaussian1d(X, sigma, 1, PAD_REPEAT);
    }
    
    template <typename T, typename T2>
    std::vector<T> smoothDiff2( const std::vector<T> & X, T2 sigma )
    {
        return gaussian1d(X, sigma, 2, PAD_REPEAT);
    }
    
    
    template <typename T, typename T2, typename T3>
    std::vector<int> maxima( const std::vector<T> & X, T2 diffSigma, T3 eps, float thresh, int offsetThresh )
    {
        std::vector<T> Xn = X; //normalize(X);
        std::vector<T> D = smoothDiff(Xn, diffSigma);
        
        // threshold is relative to height....
        
        std::vector<int> MM;
        
        int ind = 0;
#define THR(a,b) ((fabs(a) + fabs(b)) > eps)
        
        int c = 0;
        
        for( int i = 0; i < D.size()-1; i++ )
        {
            ind++;
            T d1 = D[i];
            T d2 = D[i+1];
            if( d1 > 0.0 && d2 < 0.0 && THR(d1, d2) && X[i] > thresh) //&& c > offsetThresh )
            {
                c = 0;
                MM.push_back(ind);
            }
            c++;
        }
        
        return MM;
    }
    
    template <typename T, typename T2, typename T3>
    std::vector<int> extrema( const std::vector<T> & X, T2 diffSigma, T3 eps, int offsetThresh )
    {
        std::vector<T> D = smoothDiff(X, diffSigma);
        
        std::vector<int> MM;
        
        int ind = 0;
//#define THR(a,b) ((fabs(a) + fabs(b)) > eps)
        
        int c = 0;
        
        for( int i = 0; i < D.size()-1; i++ )
        {
            ind++;
            T d1 = D[i];
            T d2 = D[i+1];
            if( (d1*d2)<0.0 && THR(d1, d2) )// && c > offsetThresh )
            {
                c = 0;
                MM.push_back(ind);
            }
            c++;
        }
        
        return MM;
    }
    
    template <class T>
    vectori appendEndIndices( const vectori & I, const T & buf )
    {
        vectori res = I;
        // endpoints
        res.insert(res.begin(),0);
        res.push_back(buf.size()-1);
        return res;
    }
    
    template <typename T>
    std::vector<T> mergeSorted( const std::vector<T> & I1 , const std::vector<T> & I2 )
    {
        std::vector<T> I = I1;
        I.insert( I.end(), I2.begin(), I2.end() );
        std::sort(I.begin(), I.end());
        return I;
    }

    
    
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

    
template <typename T>
std::vector<T> linspace( T a, T b, int n )

{
    std::vector<T> res;
    T inc = (b-a)/(n-1);
    T v = a;
    for( int i = 0; i < n; i++ )
    {
        res.push_back(v);
        v+=inc;
    }
    res[res.size()-1]=b;
    return res;
}

template <typename T>
T mean( const std::vector<T> & V )
{
    T sum = 0.0f;
    for( int i = 0; i < V.size(); i++ )
        sum+=V[i];
    return sum/V.size();
}

template <typename T>
T variance( const std::vector<T> & V )
{
    T m = mean(V);
    T tmp = 0.0f;
    for( int i = 0; i < V.size(); i++ )
    {
        T a = V[i];
        tmp += (m-a)*(m-a);
    }
    
    return tmp/V.size();
}

template <typename T>
T standardDeviation( const std::vector<T> & V )
{
    return sqrt(variance(V));
}

template <typename T>
T maxDeviation( const std::vector<T> & V  )
{
    T m = mean(V);
    T cur = 0.0f;
    for( int i = 0; i < V.size(); i++ )
    {
        T d = fabs(V[i]-m);
        if( d > cur )
            cur = d;
    }
    
    return cur;
}
    

    
    
}