/*
 *  Image.h
 *
 *  Created by Daniel Berio on 5/30/12.
 *  http://www.enist.org
 *  Copyright 2012. All rights reserved.
 *
 */

#pragma once

#include "cmGfx.h"
#include "Shape2d.h"
#include "Shape.h"
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"

/*struct CvMat;

struct _IplImage;
typedef _IplImage IplImage;
*/

namespace cm
{
	class Image
	{
	public:
		Image();
		~Image();
		Image( int width, int height, int fmt = Image::RGB );
		Image( const std::string & path, int = Image::UNKNOWN );
		Image( const Image & img );
		//Image( const cv::Mat & mat );

		//cv::Mat getMat() const;

		enum FORMAT
		{
			RGB = 0,
			BGRA = 1,
			GRAYSCALE,
			UNKNOWN
		};
		
		void operator = ( const Image & img ) { copy(img); }
		void copy( const Image & img );

		void release();
		void create( int width, int height, int fmt = Image::RGB );
		void create( int width, int height, int fmt, void * buf );
		void copy( void * buf );
		
		bool load( const std::string & path, int = Image::UNKNOWN );
		bool save( const std::string & path );
		void draw( float x, float y, float w = 0, float h = 0, bool yUp = false);
		void grabFrameBuffer(  int w, int h, int fmt_ = Image::BGRA );
		
		void color( const Color & clr );
		void color( float r, float g, float b, float a = 1.0 );
		void color( float l, float a = 1.0 );

		void lineWidth( int w );

		void setView( float x, float y, float w, float h );
		void setView( float w, float h );
		
		void drawLine( const Vec2 & a, const Vec2 & b  );
		void drawRect( float x, float y, float w, float h );
		void fillRect( float x, float y, float w, float h );
		
		void fillShape( const cm::Shape2d & shape );
		void fillPath( const cm::Path2d & path );
		void drawContour( const std::vector<Vec2> pts, bool closed = false );
		void drawShape( const cm::Shape2d & shape );
		void drawPath( const cm::Path2d & path );

		void fillShape( const cm::Shape & shape );
		void fillContour( const cm::Contour & path );
		void drawShape( const cm::Shape & shape );
		void drawContour( const cm::Contour & path );

		void fillRect( const cm::Rectf & r );
		void drawRect( const cm::Rectf & r );
		void drawImage( const cm::Image & img, float x, float y );

		void identity();
		void scale( float x, float y );
		void rotate( float ang );
		void translate( float x, float y );

		void invert();
		void mirror( bool x, bool y );
		void thresh( int val, bool inv = false );
		void adaptiveThresh( float blockSize, float meanShift = 0.0, bool inv=false , bool gauss=false  );
		void range( int minr, int maxr );
		void erode();
    	void dilate( );                    // based on 3x3 shape
    	void blur( int value=3 );          // value = x*2+1, where x is an integer
    	void blurGaussian( int value=3 );  // value = x*2+1, where x is an integer
    	void brightnessContrast( float brightness, float contrast );
		
		void add( const Image & img );
		void subtract( const Image & img );

		void floodFill( int x, int y, const Color & clr );
		void floodFill( int x, int y, Image & mask, const Color & clr );
		
		enum
		{
			GABOR_REAL = 1,
			GABOR_IMAG = 2,
			GABOR_MAG,
			GABOR_PHASE,
			NUM_GABOR_TYPES
		};
		
		void gabor( float phi,float nu,float sigma, int type = Image::GABOR_MAG );
		void gabor4( float nu, float sigma, int type = Image::GABOR_MAG );
		
		int pixelsSum( int step = 1 ) const;
		
    	void warpPerspective( const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D );
    	void warpIntoMe( const Image& mom, const Vec2 *src, const Vec2 *dst );
    	
    	void absDifference( const cm::Image & img );

		Shape findContours( float w, float h, float minArea, float maxArea, float simplifyTol = 1.0, bool approx = true, bool holes = true , int maxContours = 500 );
		std::vector<Shape> segmentation(  int n,  float w , float h, float minArea, float maxArea, float simplifyTol = 1.0, bool approx = true, bool holes = true , int maxContours = 500 );

		void clear();
		
		Image::FORMAT getFormat() const { return (Image::FORMAT)fmt; }
		
		// TODO: 
		// initResampled( int w, int h, const Image & src );
		// morphological ops 
		// find contours?
		
		void setPixel( int x, int y, const Color & clr );
		Color getPixel( int x, int y ) const;
		float getIntensity( int x, int y  ) const ;
		
		bool hasTexture() const { return texture != 0; }
		
		Texture * getTexture();
		void updateTexture();
		
		int getWidth() const { return width; }
		int getHeight() const { return height; }
		int getSize() const { return stride*height*bytesPerPixel; }
		int getStride() const { return stride; }

		/// Get intensity over a convex shape, less precise but much faster than getIntensity
		float getIntensityConvex( const cm::Path2d & shape, float srcW = 0, float srcH = 0);
		/// Get intensity over a shape, can pass also a path in
		float getIntensity( const cm::Shape2d & shape, float srcW = 0, float srcH = 0);
		
		Color curColor;
		int linew;
		M33 matrix;
		
		bool retainBuffer;
		int width;
		int height;
		FORMAT fmt;
		int bytesPerPixel;
		int stride;
		unsigned char * buf;
		IplImage * cvImg;
		IplImage * cvImgTmp;
		
		bool dirty;
		
	private:
		
		void fillShape( IplImage * img, const Shape2d & shape, const Color & clr,float srcW, float srcH  );
		void fillShape( IplImage * img, const Shape & shape, const Color & clr,float srcW, float srcH  );
		void allocCv();
		//cv::Mat cvImg;
		//cv::Mat cvMask;
		
		CvMat*		briConLutMatrix;
		IplImage * cvMask;
		
		void rgbTo( int fmt_ );
		void bgraTo( int fmt_ );
		void grayscaleTo( int fmt_ );
		
		Texture * texture;
	};
}