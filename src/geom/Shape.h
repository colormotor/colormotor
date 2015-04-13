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
 --                                http://www.enist.org
 --                                drand48@gmail.com
 --
 --------------------------------------------------------------------
 ********************************************************************/
 
#pragma once
#include "cmMath.h"
#include "cmCore.h"


namespace cm
{
class Path2d;
class Shape2d;

class Contour
    {
    public:
        Contour();
        Contour( const Contour & mom );
        Contour( const cm::Path2d & path );
        Contour( const std::vector<Vec2> & P, bool closed=false );
        Contour( const vectord & X, const vectord & Y, bool closed=false );

        // For numpy compatibility
        Contour( float * buf, int n, int n2, bool closed=false );

        //~Contour();
        
        void addPoint( const Vec2 & p );
        void addPoint( float x, float y );
        void addPoints( const std::vector<Vec2> & P );
        
        void insertPoint( int i, const Vec2 & p );
        
        const Vec2 & operator [] ( int i ) const { return points[mod(i, (int)points.size())]; }
        Vec2 & operator [] ( int i )  { return points[mod(i, (int)points.size())]; }
        const Vec2 & getPoint( int i ) const { return points[mod(i, (int)points.size())]; }
        Vec2 & getPoint( int i ) { return points[mod(i, (int)points.size())]; }
        const Vec2 & last() const { return points.back(); }

        void setPoint( int i, const Vec2 & p ) { points[i] = p; }
        
        vectord X() const;
        vectord Y() const;
        
        int size() const { return points.size(); }
        
        bool contains( const Vec2 &p ) const;
        
        Rect boundingBox() const;
        Vec2  centroid() const;
        
        /// Retreive a section of the contour from point a to point b
        Contour section( int a, int b ) const;

        /// Get a position along the contour interpolated between [0,1]
        Vec2 getPosition( float t ) const;
        /// Returns the length for each segment
        vectord segmentLengths() const;
        /// integrated lengths along contour between a and b
        /// default area of all contour
        vectord arcLengths( int a = -1, int b = -1 ) const;
        /// integrated length along contour between a and b
        /// default area of all contour
        float arcLength( int a = -1, int b = -1 ) const;
        /// area of contour between a and b
        /// default area of all contour
        float area( int a = -1, int b = -1 ) const;
        /// area of contour between a and b
        /// default area of all contour
        float signedArea( int a = -1, int b = -1 ) const;
        
        void reverse();
        void simplify(float tol);
        void cleanup( float tol );
        
        void transform( const M33 & m );
        /// Scales the contour to fit the given diagonal length
        /// returns the scaling matrix
        M33 scaleToDiagonal( float diagonalLength );
        /// Scales the contour to fit the diagonal length of the given rect
        /// returns the scaling matrix
        M33 scaleToDiagonal( const Rect & r );
        
        bool empty() const { return points.empty(); }
        void clear() { points.clear(); closed = false; }
        
        bool isClosed() const { return closed; }
        void close() { closed = true; }
        
        // Drawing utils
        void rect( const Rect & r );
        void rect( float x, float y, float w, float h );
        
        void draw() const;
        void fill() const;
        
        std::vector<Vec2> points;
    private:
        bool closed;
        
        void  simplifyDP(float tol, Vec2f* v, int j, int k, int* mk );
    };
    
    class Shape
    {
    public:

        Shape() {}
        Shape( const Shape2d & s2d );
        
        Shape( const Contour & ctr ) { contours.push_back(ctr); }
        
        ~Shape() {}
        
        void clear() { contours.clear(); }
        
        const Contour & operator [] ( int i ) const { return contours[i]; }
        Contour & operator [] ( int i )  { return contours[i]; }
        const Contour & getContour( int i ) const  { return contours[i]; }
        Contour & getContour( int i ) { return contours[i]; }
        const Contour & last() const { return contours.back(); }
        
        void addContour( const Contour & ctr ) { contours.push_back(ctr); }
        void appendContour() { contours.push_back(Contour()); }
        
        int size() const { return contours.size(); }
        
        bool contains( const Vec2 &p ) const;
        
        Rect boundingBox() const;
        Vec2  centroid() const;
        
        void simplify( float tol );

        bool operator == (const Shape & s ) const;
        
        void transform( const M33 & m );
        /// Scales the shape to fit the given diagonal length
        /// returns the scaling matrix
        M33 scaleToDiagonal( float diagonalLength );
        /// Scales the shape to fit the diagonal length of the given rect
        /// returns the scaling matrix
        M33 scaleToDiagonal( const Rect & r );
        
        void draw() const;
        void fill() const;
        
        std::vector<Contour> contours;
    };

    /// Path sampled at uniform arc length s
    Contour sampleAtDistance( const Contour & P, float s );
    float turningAngleAt( const Contour & P, int index, int res = 10 );
    vectord turningAngles( const Contour & P, int res = 10 );

    vectord curvature( const Contour & P_, int res = 10 );
    
    Contour smoothGaussian( const Contour & P, float sigma );
    
    //// Catmull rom
    Contour centripetalCatmull( const Contour &P_, int subd);
    vectord contourVelocity( const Contour &P,float sigma = 2.0f, float dt = 0.001);
    
    /// XML UTILS 

    void contourToXml( const std::string & path, const Contour & ctr );
    Contour contourFromXml( const std::string & path );

    void shapeToXml( const std::string & path, const Shape & ctr );
    Shape shapeFromXml( const std::string & path );
    
    void shapesToXml( const std::string & path, const std::vector<Shape> shapes );
    std::vector<Shape> shapesFromXml( const std::string & path );
    
    TiXmlElement * contourToXml( const Contour & ctr );
    Contour contourFromXml( TiXmlElement * elem );
    
    TiXmlElement * shapeToXml( const Shape & shape );
    Shape shapeFromXml( TiXmlElement * elem );
    
    TiXmlElement * shapesToXml( const std::vector<Shape> shapes );
    std::vector<Shape> shapesFromXml( TiXmlElement * elem );
    
}