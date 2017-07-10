
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
#include "cm.h"

// GL includes
#ifdef CM_WIN
    #include <windows.h>
    #include <GL/glu.h>
    #ifndef GL_CLAMP_TO_EDGE
        #define GL_CLAMP_TO_EDGE 0x812F
    #endif
#elif defined CM_LINUX
    #ifndef __glew_h__
        #define GL_GLEXT_PROTOTYPES
        #include <GL/glew.h>
        #include <GL/glu.h>
        #include <GL/gl.h>
 		#include <GL/glx.h>
    #endif

#elif defined CM_OSX
    #include <OpenGL/glu.h>
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>

#endif

#include <math.h>
#include "cm_math.h"

#include "opencv2/opencv.hpp"
#include "opencv2/core/opengl_interop.hpp"
#include "opencv2/highgui/highgui.hpp"

#define CM_GLERROR		cm::gfx::getGLError()

#ifndef GL_TEXTURE_RECTANGLE
#ifdef CM_WIN32
#define GL_TEXTURE_RECTANGLE	GL_TEXTURE_RECTANGLE_ARB
#else
#define GL_TEXTURE_RECTANGLE	GL_TEXTURE_RECTANGLE_ARB
#endif
#endif


#define GL_COLOR_B( clr )  (( (clr) >> 16 )  &0xFF)
#define GL_COLOR_G( clr )  (( (clr) >> 8  )  &0xFF)
#define GL_COLOR_R( clr )  (  (clr)  	    &0xFF)
#define GL_COLOR_A( clr )  (( (clr) >> 24 )  &0xFF)
		
#define GL_MAKERGBA(r,g,b,a) ((unsigned int)((((unsigned char)(a*255.0f)&0xff)<<24)|(((unsigned char)(b*255.0f)&0xff)<<16)|(((unsigned char)(g*255.0f)&0xff)<<8)|((unsigned char)(r*255.0f)&0xff)))
#define GL_MAKERGBA255(r,g,b,a) ((unsigned int)((((unsigned char)(a)&0xff)<<24)|(((unsigned char)(r)&0xff)<<16)|(((unsigned char)(g)&0xff)<<8)|((unsigned char)(b)&0xff)))



namespace cm
{

/// An axis aligned box, can be 2d or 3d
struct Box
{
public:
    Box()
	{
        minmax = arma::zeros(2,2);
	}

    Box( double x, double y, double w, double h  )
	{
	    minmax = arma::zeros(2,2);
	    minmax.col(0) = arma::vec({x, y});
	    minmax.col(1) = arma::vec({x+w, y+h});
	}


    Box( const arma::vec& min, const arma::vec& max )
	{
	    minmax = arma::zeros< arma::mat >(min.size(),2);
	    minmax.col(0) = min;
	    minmax.col(1) = max;
	}


    arma::vec ltrb() const { return arma::vec({l(), t(), r(), b()}); }
    void ltrb( double l, double t, double r, double b ) { minmax(0,0)=l; minmax(1,0)=t; minmax(0,1)=r; minmax(1,1)=b; }

    void l( double v ) { minmax(0,0)=v; }
    void t( double v ) { minmax(1,0)=v; }
    void r( double v ) { minmax(0,1)=v; }
    void b( double v ) { minmax(1,1)=v; }

    double l() const { return minmax(0,0); }
    double t() const { return minmax(1,0); }
    double r() const { return minmax(0,1); }
    double b() const { return minmax(1,1); }

    int dimension() const { return minmax.n_rows; }

    double width() const { return fabs( minmax(0,1) - minmax(0,0) ); }
    double height() const { return fabs( minmax(1,1) - minmax(1,0) ); }
    double depth() const { return fabs( minmax(2,1) - minmax(2,0)); }
    
    
    arma::vec center() const { return (minmax.col(0) + minmax.col(1))/2; }
    void setCenter( const arma::vec& cenp );

    /// Get (2d) rect corners as columns in a 2x4 matrix
    /// corners are ordered clockwise starting from the top-left
    arma::mat corners() const;
    
    bool contains( const V2& p ) const { return ( p.x >= min()[0] && p.y >= min()[1]
                                                 && p.x <= max()[0] && p.y <= max()[1] ); }
    /// include a point in rect, if index==0 it will force the point,
    /// otherwise adjusts min and max.
    void includeAt( int index, const arma::vec& p );
    void include( const arma::vec &p ) { includeAt(1, p); }
    void include( const Box & b ) { include(b.min()); include(b.max()); }
    
    arma::vec min() const { return minmax.col(0); }
    arma::vec max() const { return minmax.col(1); }
	 
    // Rect is stored as min and max positions (columns) 
    arma::mat minmax;
};


/// Simple contour class, stores positions as columns a matrix.
struct Contour
{
Contour()
:
    closed(false)
	{
	    // we use a 1x1 size for an empty contour
	    // maybe armadillo has a better way to handle this
        points = arma::zeros(1,1);
	}

    Contour( const std::vector<V2> & ptList, bool closed = false )
:
    closed(closed)
	{
	    points = arma::zeros(2, ptList.size());//,2);
	    for( int i = 0; i < ptList.size(); i++ )
			points.col(i) = ptList[i];
	}

	Contour( const std::vector<V3> & ptList, bool closed = false )
	:
    closed(closed)
	{
	    points = arma::zeros(3, ptList.size());//,2);
	    for( int i = 0; i < ptList.size(); i++ )
			points.col(i) = ptList[i];
	}

	Contour( const std::vector< arma::vec > & ptList, bool closed = false )
	:
    closed(closed)
	{
	    points = arma::zeros(ptList[0].size(), ptList.size());//,2);
	    for( int i = 0; i < ptList.size(); i++ )
		points.col(i) = ptList[i];
	}

	Contour( const arma::mat& mat, bool closed = false  )
	:
    closed(closed)
	{
	    points = mat;
	}
    
    void clear() { points =  arma::zeros(1,1); }
    
    bool empty() const { return points.n_rows == 1; }

    int size() const { if(empty()) return 0; return points.n_cols; }

    int dimension() const { return points.n_rows; }

    int isValid() const { return !empty() && size() > 1; }

    // Accessors
    arma::vec operator [] ( int i ) const { return points.col(i); }
    arma::vec last() const { return points.col(size()-1); }
    arma::vec getPoint( int i ) const { return points.col(i); }
    
    //void set( int i, const V2& p ) { points.col(i) = (arma::vec)p; }
    //void set( int i, const V3& p ) { points.col(i) = (arma::vec)p; }
    
    //arma::vec& last() { return points.col(size()-1); }

    // return a point linearly interpolated along the contour with t[0..1]
    arma::vec interpolate( double t ) const;

    /// Lenght of contour between a and b (-1 is the last point in the contour)
    double length( int a = 0, int b = -1) const;

    arma::vec centroid() const { return mean(points,1); }

    bool contains( const V2& p ) const;

    /// Bounding box of the contour	
    Box boundingBox() const;

    /// Adds one dimension
    void addDimension();

    void addPoint( const V2& pt );
    void addPoint( const V3& pt );
    void addPoint( const arma::vec& pt );
    void addPoint( double x, double y );

    void close(bool f=true) { closed=f; }
    
    /// Save points to file of type @type, if @savePointsAsRows is true (default) the 
    /// contour will be saved transposed with respect to the armadillo matrix layout
    void save( const std::string & path, arma::file_type type = arma::csv_ascii, bool savePointsAsRows = true );

    /// Load points from file of type @type, if @savePointsAsRows is true (default) the 
    /// contour is assumed to be saved transposed with respect to the armadillo matrix layout
    void load( const std::string & path, arma::file_type type = arma::csv_ascii, bool savePointsAsRows = true );

    /// Transforms the contour with a nXn homogeneous matrix
    /// if the matrix is 4x4 and the contour is 2d, 
    /// then the contour points will be converted to 3d space. 
    void transform( const arma::mat& m );

    /// Returns a transformed version of the contour
    Contour transformed( const arma::mat& m ) const;
    
    /// note: contour points stored as columns
    arma::mat points;

    bool closed;
};

/// A set of contours
struct Shape
{
    Shape()
	{

	}

    Shape( const Contour& ctr )
	{
	    contours.push_back(ctr);
	}
    
    void clear() { contours.clear(); }
    
    int size() const { return contours.size(); }

    // Accessors
    const Contour& operator [] ( int i ) const { return contours[i]; }
    Contour& operator [] ( int i )  { return contours[i]; }
    const Contour& last() const { return contours.back(); }
    Contour& last() { return contours.back(); }
    const Contour& getContour( int i ) const { return contours[i]; }
    Contour& getContour( int i )  { return contours[i]; }
    
    void add( const Shape& s ) { for( int i = 0; i < s.size(); i++ ) add(s[i]); }
    void add( const Contour& c ) { contours.push_back(c); }

    void loadSvg( const std::string & f, int subd = 100);
    
    arma::vec centroid() const;
    Box boundingBox() const;

    bool contains( const V2& p ) const;
   
    void transform( const arma::mat& m );
    
    Shape transformed( const arma::mat& m ) const;
    
    std::vector< Contour > contours;
};

/// Generic OpenGL object
class GfxObject : public RefCounter
{
public:
    virtual ~GfxObject() {};
};

struct GLObj
{
	enum TYPE
	{
		TEXTURE = 0,
		SHADER = 1,
		PROGRAM,
		VB,
		IB,
		FBO,
		RENDERBUFFER
	};
	
	GLObj( GLuint glId, TYPE type )
	:
	glId(glId),
	type(type)
	{
	}
	
	GLObj()
	{
	}
	
	GLuint glId;
	TYPE type;
};

class Texture : public GfxObject
{
	public:
		enum FORMAT
		{	
			FORMAT_UNKNOWN = -1,
			
			#ifndef CM_GLES
			R8G8B8 = 0,
			A8R8G8B8,
			L8,
			// FLOAT TEXTURE FORMATS
			// 16-BIT FLOATING POINT
			R16F,
			// 32-BIT FLOATING POINT
			G16R16F,
			// 64-BIT FLOATING POINT
			A16B16G16R16F,
			// IEEE 32-BIT FLOATING POINT
			R32F,
			// IEEE 64-BIT FLOATING POINT
			G32R32F,
			// IEEE 128-BIT FLOATING POINT
			A32B32G32R32F,
			#else
			
			R8G8B8 = 0,
			A8R8G8B8,
			L8,
			R5G6B5,
			A4R4G4B4,
			A1R5G5B5,
			#endif
			
			NUM_SUPPORTED_FORMATS
		};
		
		enum LOCKFLAGS
		{
			LOCK_READ = 1,
			LOCK_WRITE = 2,
			LOCK_READWRITE = 3
		};
		
		enum WRAP
		{
			CLAMP_TO_EDGE = 0,
			CLAMP,
			REPEAT,
			NUM_WRAP_TYPES
		};
		
		enum FILTER
		{
			NEAREST = 0,
			LINEAR,
			NUM_FILTER_TYPES
			// TODO MIPMAPS
		};
    
        struct Info
        {
            float	maxU;
            float	maxV;
            float	maxW; // for volume textures
            
            int		width;
            int		height;
            int		depth;
            
            int		hwWidth;
            int		hwHeight;
            int		hwDepth;
            
            int		mipLevels;
            
            GLuint	glTarget;
            
            GLuint	glId;
            
            GLint	glFormat;
            GLenum	glDataFormat;
            GLenum	glDataType;
            
            int		bytesPerPixel;
        };
        
        ///////////////////////////////////////////////////////////////////////////
        /// holds information on texture lock
        struct 	LockInfo
        {
            void * buf;
            int	pitch;
            
            // volume texture specific
            int rowPitch;
            int slicePitch;
            /// mip level that has been locked
            int	level;
            /// this is trash if texture isnt a cube map
            //CUBE_FACE face;
        };

	public:
		Texture();
		Texture( const char * path, int miplevels = 1 );
		Texture( int w, int h, int  fmt, int miplevels = 1 );
	
		~Texture();
		
		void	release();
		
		void	bind( int sampler = 0 );
		void	unbind();
		
        int     getId() const { return info.glId; }
        bool    empty() const { return info.glId == -1; }
    
		void	init();
		
		bool	load( const char * path, int miplevels = 1 );
		
		bool	create( void * data, int w, int h, int fmt = A8R8G8B8, int mipLevels = 1 );
		bool	create( int w, int h,  int fmt = A8R8G8B8, int mipLevels = 1 );
		void	setGLData( int w, int h,
							  int hwW, int hwH,
							  GLuint	glTarget,
							  GLuint	glId,
							  GLint		glFormat );
							  
		void	createFromGL( int w, int h,
							  GLuint	glTarget,
							  GLuint	glId,
							  GLint		glFormat );
		
		bool	isValid();
		bool	update( void * data, int w, int h);
		bool	update( void * data, int x, int y, int w, int h);

		
		void	setWrap( int wrap );
		void	setWrapS( int wrap );
		void	setWrapT( int wrap );
		void	setMinFilter( int filter );
		void	setMagFilter( int filter );

		/// Sets size of texture, will recompute maxU and maxV
		void 	setSize( int width, int height );

		/// Copy contents of frame buffer into texture
		bool	grabFrameBuffer();
		bool	grabFrameBuffer(int x, int y, int width, int height);
		
		// this reads pixels of texture into a buffer,
		// it will work also with render target textures, since it uses glReadPixels
		bool	readPixels( void * data, int width, int height, FORMAT fmt );
		 
		int		getWidth() const { return info.width; }
		int		getHeight() const { return info.height; }
		int		getDepth() const { return info.depth; }
		
		int		getHardwareWidth() const { return info.hwWidth; }
		int		getHardwareHeight() const { return info.hwHeight; }
		int		getHardwareDepth() const { return info.hwDepth; }
		
		int		getFormat() const { return _format; }
		
		bool	isBound() const { return _boundSampler >= 0; }
		
#ifdef CM_OFX
		bool	fromOfImage( ofImage & img, int mipLevels = 1 );
		void	fromOfTexture( ofTexture & tex );
#endif
		
		void	draw( float x = -1.0f , float y = -1.0f , float w = 2.0f , float h = 2.0f, bool yUp = false  )
		{
			draw( x,y,w,h,yUp,1.0,1.0);
		}
		
		void	draw( float x , float y, float w, float h, bool yUp, float uscale, float vscale  );
		
		/// draw a part of the texture
		void	drawPart( float x, float y, float w, float h, float tx, float ty, float tw, float th );
		
	public:
		/// lock texture for reading and writing 
		void *		lock( int flags = LOCK_READWRITE, int miplevel = 0);
		/// unlock texture
		bool		unlock();
	
		// PIXEL MANIPULATION FUNCS
		// must lock texture before calling, but we don't check for optimizing
		///   Set pixel
		inline void			pixel( int x, int y, float r, float g, float b, float a )
		{
			_pixelfunc(_lockInfo.buf,x,y,0,_lockInfo.pitch,0,r,g,b,a);
		}

		///   Set pixel grayscale
		inline void			pixel( int x, int y, float val )
		{
			_pixelfunc(_lockInfo.buf,x,y,0,_lockInfo.pitch,0,val,val,val,val);
		}

		///   Set voxel 3d
		inline void			pixel( int x, int y, int z, float r, float g, float b, float a )
		{
			_pixelfunc(_lockInfo.buf,x,y,z,_lockInfo.rowPitch,_lockInfo.slicePitch,r,g,b,a);
		}

		///   Set voxel grayscale
		inline void			pixel( int x, int y, int z, float val )
		{
			_pixelfunc(_lockInfo.buf,x,y,z,_lockInfo.rowPitch,_lockInfo.slicePitch,val,val,val,val);
		}


		bool	flipX;
		bool	flipY;
		
		static void	resetAllSamplers();
		
		float matrix[16];
		
		/// gl and non information
		Info info;
	protected:
		/// sampler that this texture has been bound to, -1 is no sampler
		int		_boundSampler;
		
		int		_format;
		
		bool	setPixelFunc();
		/// pixel manipulation callback
		/// gets set when format is specified.
		void	(*_pixelfunc) ( void * out, int x, int y, int z, int pitch, int slicepitch, float r, float g, float b, float a );
	
		/// information on locked texture
		LockInfo _lockInfo;
		int		_lockFlags;
		
		bool	_initialized;	
};


	/// Depth stencil formats  
    class DepthStencil
    {
    public:
        
        enum FORMAT
        {
            FORMAT_UNKNOWN = -1,
            D16 = 0		,
            D24,
            D24_S8,
            D32,
            NUM_FORMATS
        };
    };
    
    
#define MAX_COLOR_ATTACHMENTS	4 // THIS CAN BE DINAMICALLY CREATED BASED ON HW.
    
    
#define DEFAULT_COLOR_FMT	Texture::A8R8G8B8
#define DEFAULT_DEPTH_FMT	DepthStencil::D24_S8


class RenderTarget : public GfxObject
{
public:
    RenderTarget();
    RenderTarget( int w, int h,
                 int pixelFormat = DEFAULT_COLOR_FMT,
                 bool depth = true,
                 int depthFormat = DEFAULT_DEPTH_FMT,
                 int numSamples = 0 );
    
    void zero();
    
    ~RenderTarget();
    
    /// Release render target
    void	release();
    
    /// Create a 2D render target
    /// \param w Width.
    /// \param h Height.
    /// \param pixelFormat Format for the render target texture.
    /// \param depth If true also a depth buffer is created for the render target.
    /// \param depthFormat Optional format for the depth buffer if present.
    bool createMrt( int w, int h,
                     std::vector<int> formats,
                     bool hasDepthStencil = true,
                     int depthStencilFormat = DEFAULT_DEPTH_FMT,
                     int numSamples = 0 );
    
    bool create2d( int w, int h,
                     int pixelFormat = DEFAULT_COLOR_FMT,
                     bool depth = true,
                     int depthFormat = DEFAULT_DEPTH_FMT,
                     int numSamples = 0 )
    {
        std::vector<int> fmts;
        fmts.push_back(pixelFormat);
        return createMrt( w,h,fmts,depth,depthFormat,numSamples);
    }
    
    
    /// Returns true if render target has been initialized
    bool	isValid() const;
    
    /// Resizes render target
    bool	resize( int w, int h );
    
    /// Sets render target as current
    void	bind( bool viewPort = true );
    void	unbind( bool viewPort = true );
    
    Texture * getTexture( int ind = 0 );

    void updateTexture( int ind = 0 );
        
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    

protected:
    GLuint createAndAttachRenderbuffer(GLenum internalFormat, GLenum attachmentPoint);
    void createAndAttachTexture(int format, int attachmentPoint);
    bool bindDepth();
    
    bool valid;
    GLint savedFramebuffer;
    
    bool dirty[MAX_COLOR_ATTACHMENTS];
    Texture * textureAttachments[MAX_COLOR_ATTACHMENTS];
    GLuint colorBuffers[MAX_COLOR_ATTACHMENTS];
    int numColorAttachments;
    
    std::vector<int> formats;
    bool hasDepthStencil;
    int depthStencilFormat;
    int numSamples;

    int depthStencilId;
    GLuint fbo;
    GLuint fboTextures;
    
    int width;
    int height;

};


/// Wrapper around an OpenCV mat, 
/// handles conversion to OpenGL textures
class Image
{
public:
	enum 
	{
		// Actually just the number of channels
		GRAYSCALE = 1,
		BGR = 3,
		BGRA = 4,
        
        UNKNOWN = -1
	};

	
	Image()
	{
	}

	/// Creates an empty image
	Image( int w, int h, int format = Image::BGRA );

	/// Copy image
	Image( const Image & mom );

	/// Copy image with format conversion
	Image( const Image & mom, int format );

	/// Create from opencv Mat
	Image( const cv::Mat & m );

	/// Build from armadillo cube
	Image( const arma::uchar_cube &src );
	
	/// Convert to armadillo cube
	arma::uchar_cube toArma() const;

	~Image() { release(); }
	
	void release();

	/// Load image from file
	/// modes:
		/// CV_LOAD_IMAGE_UNCHANGED (<0) loads the image as is (including the alpha channel if present)
    /// CV_LOAD_IMAGE_GRAYSCALE ( 0) loads the image as an intensity one
	/// CV_LOAD_IMAGE_COLOR (>0) loads the image in the RGB format
	Image( const std::string & path, int loadMode = UNKNOWN );

	bool empty() const { return mat.empty(); }

	int width() const { return mat.cols; }
	int height() const { return mat.rows; }
	// Number of bytes for each row
	int step() const { return mat.step; }

	void bind( int sampler = 0 );
	void unbind();

	bool isBound() const { return boundSampler != -1; }

	void mirror( bool x, bool y );

	void updateTexture();

	/// Draw 2d image 
	void draw( float x = 0, float y = 0, float w = 0, float h = 0 );

	int getGlId() { return tex.getId(); }

	/// Copies current contents of framebuffer into the image
	void grabFrameBuffer();
	
	void save( const std::string& path ) const;

	/// threshold the image
	void threshold(float thresh, bool inv=false);

	/// Blur the image 
	void blur( float value, bool gaussian=false );

	/// Find contours in image
	/// approx simpifies the contour
	/// if minArea > 0, only contours with minArea<area<maxArea are kept
	Shape findContours( bool approx=true, float minArea=0., float maxArea=1e30 );
	

	cv::Mat mat;
	cv::Mat tmp;

    Texture tex;
	//cv::ogl::Texture2D tex;

	int boundSampler = -1;
	bool dirty = true;
};


/// Arcball, based on Shoemake 92 article
class ArcBall
{
public:
    ArcBall()
    {
        mat = arma::eye(4,4);
        rot = quat(mat);
    }
    
    static V3 projectToSphere( const V2 & p, const V2 & center, float radius, bool constrained=false, const V3& axis=V3(1,0,0) );
    void update( const V2 & mousePos, const V2 & mouseDelta, const V2 & center, float radius, bool contrained=false );
    
    V4 rot;
    M44 mat;
};

/////////////////////////////////////////////////////////////
// A few simple wrappers around GL functions 
// and graphics utilities for a processing like feel,
namespace gfx
{
    
    void setManualGLRelease( bool flag );
    bool isGLReleaseManual();
    void releaseGLObject( GLObj obj );
    void releaseGLObjects();
    
    // Low level funcs
    // error checking
    bool getGLError();
    bool checkFramebufferStatus();
    bool areNonPowerOfTwoTexturesSupported();
    void setPowerOfTwoTexturesSupport( bool flag );
    
    bool GLExtension( const std::string &extName );
    
    GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage);

/// OpenGL state defs

/// Fill modes
	enum FILLMODE
	{
	    FILL_SOLID		= 0,
	    FILL_WIRE		= 1,
	    FILL_POINT		= 2,
	    NUMFILLMODES 
	};

/// Cull modes
	enum CULLMODE
	{
	    CULL_NONE		= 0,
	    CULL_CW			,
	    CULL_CCW		,
	    NUMCULLMODES
	};

/// predefined blend modes
	enum BLENDMODE
	{
	    BLENDMODE_ALPHA = 0,
	    BLENDMODE_ALPHA_PREMULTIPLIED,
	    BLENDMODE_ADDITIVE,
	    BLENDMODE_MULTIPLY,
	    NUMBLENDMODES,
	    BLENDMODE_NONE
	};

/// Depth functions
	enum DEPTHFUNC
	{
	    DEPTH_GREATER = 0,
	    DEPTH_LESS,
	    DEPTH_GEQUAL,
	    DEPTH_LEQUAL,
	    DEPTH_EQUAL,
	    DEPTH_NOTEQUAL,
	    DEPTH_ALWAYS,
	    DEPTH_NEVER,
	    NUMDEPTHFUNCS
	};

/// Stencil functions
	enum STENCILFUNC
	{
	    STENCIL_GREATER = 0,
	    STENCIL_LESS,
	    STENCIL_GEQUAL,
	    STENCIL_LEQUAL,
	    STENCIL_EQUAL,
	    STENCIL_NOTEQUAL,
	    STENCIL_ALWAYS,
	    STENCIL_NEVER,
	    NUMSTENCILFUNCS
	};

/// Stencil operations
	enum STENCILOP
	{
	    STENCIL_INCR_WRAP = 0,
	    STENCIL_DECR_WRAP,
	    STENCIL_KEEP,
	    STENCIL_INCR,
	    STENCIL_DECR,
	    STENCIL_REPLACE,
	    STENCIL_ZERO,
	    STENCIL_INVERT,
	    NUMSTENCILOPS
	};

/// Primitive types
	enum PRIMITIVE
	{
	    TRIANGLELIST		= 0,
	    TRIANGLESTRIP,
	    TRIANGLEFAN,

	    QUADS,
	
	    LINELIST,
	    LINESTRIP,
	    LINELOOP,
	    POINTS,
	    NUMPRIMITIVES,
	    POLYGONS
	};

	GLenum getGLPRIMITIVETYPE( int in );

}

/// Mesh
struct Mesh
{
    Mesh()
    {
    }


    void triangle( int a, int b, int c )
	{
	    indices.push_back(a);
	    indices.push_back(b);
	    indices.push_back(c);
	}

   	void vertex( const arma::vec& v )
	{
	    for( int i = 0; i < v.size(); i++ )
            vertices.push_back((float)v[i]);
        if(v.size() < 3)
        {
            vertices.push_back(0.0);
        }
	}
    
    void normal( const arma::vec& n )
    {
    	for( int i = 0; i < 3; i++ )
            vertices.push_back((float)n[i]);
    }

    void color( const V4& clr )
    {
        colors.push_back(_float4(clr.x, clr.y, clr.z, clr.w));
    }
    
    void uv( const V2& coords )
    {
        uvs.push_back(_float2(coords.x, coords.y));
    }
    
    arma::mat getNormals() const 
    {
    	int size = normals.size()/3;
    	arma::mat N = arma::zeros(3, size);
    	for( int i=0; i < size; i++ )
    	{
    		int j = i*3;
    		N.col(i) = arma::vec({normals[j], normals[j+1], normals[j+2]});
    	}
    	return N;
    }

    arma::mat getVertices() const 
    {
    	int size = vertices.size()/3;
    	arma::mat V = arma::zeros(3, size);
    	for( int i=0; i < size; i++ )
    	{
    		int j = i*3;
    		V.col(i) = arma::vec({vertices[j], vertices[j+1], vertices[j+2]});
    	}
    	return V;
    }

    arma::mat getColors() const 
    {
    	int size = colors.size();
    	arma::mat V = arma::zeros(4, size);
    	for( int i=0; i < size; i++ )
    	{
    		V.col(i) = (arma::vec)V4(colors[i].x,
    							     colors[i].y,
    							     colors[i].z,
    							     colors[i].w );
    	}
    	return V;
    }

    arma::mat getUvs() const 
    {
    	int size = uvs.size();
    	arma::mat V = arma::zeros(2, size);
    	for( int i=0; i < size; i++ )
    	{
    		V.col(i) = (arma::vec)V2(uvs[i].x, uvs[i].y);
    	}
    	return V;
    }

    const std::vector<unsigned int> getIndices() const { return indices; }

    int numColors() const { return colors.size(); }
    int numUvs() const { return uvs.size(); }
    int numNormals() const { return normals.size()/3; }
    int numVertices() const { return vertices.size()/3; }
    int numIndices() const { return indices.size(); }
    
    std::vector <float> normals;
    std::vector <float> vertices;
    std::vector <float4> colors;
    std::vector <float2> uvs;
    
    std::vector <unsigned int> indices;
    int primitive = gfx::TRIANGLELIST;
};

#ifdef __APPLE__
#ifndef GLAPIENTRY
typedef GLvoid (*_GLUfuncptr)(void);
#endif
#endif

enum
{
WINDING_ODD = GLU_TESS_WINDING_ODD,
WINDING_NONZERO = GLU_TESS_WINDING_NONZERO
};

/// Contour and shape tessellator
struct Tessellator
{
    struct XYZ
    {
	XYZ( double x,
	     double y,
	     double z )
	: x(x), y(y), z(z) {}
	double x, y, z;
    };

    
    
    Tessellator() {}
    
    Tessellator( const Shape& shape, int winding=WINDING_ODD )
	{
	    tess = gluNewTess();

	    gluTessCallback( tess, GLU_TESS_VERTEX_DATA, (_GLUfuncptr) &Tessellator::vertex_cb );
	    gluTessCallback( tess, GLU_TESS_EDGE_FLAG_DATA, (_GLUfuncptr) &Tessellator::edge_cb );
	    gluTessCallback( tess, GLU_TESS_COMBINE_DATA,( _GLUfuncptr) &Tessellator::combine_cb );

	    gluTessBeginPolygon( tess, this );

	    gluTessProperty( tess, GLU_TESS_WINDING_RULE, (GLenum)winding );

	    count = 0;
	    
	    for( int i = 0; i < shape.contours.size(); i++ )
	    {
		const Contour& ctr = shape.contours[i];
		if(!ctr.size())
		    continue;
		
		gluTessBeginContour(tess);
		for( int j = 0; j < ctr.size(); j++ )
		{
		    arma::vec v = ctr[j];
		    mesh.vertex( v );
		    verts.push_back( XYZ(v[0], v[1], v[2]) );
		    gluTessVertex( tess, &verts.back().x, (void*)(long)count );
		    count++;
		}
		gluTessEndContour(tess);
	    }

	    gluTessEndPolygon( tess );
	    gluDeleteTess(tess);
	    
	    //printf("Mesh has %d inds and %d verts\n", mesh.numIndices(), mesh.numVertices() );
	    
	}

    ~Tessellator()
	{
	    
	}
    
    static void vertex_cb( GLvoid * v, void * data )
	{
	    Tessellator * inst = (Tessellator*)data;
	    inst->mesh.indices.push_back((long)v);
	    // printf("Vertex %d\n",(int)(long)v);
	}
    
    static void combine_cb( GLdouble coords[3],
			    void * vdata[4],
			    GLfloat weights[4],
			    void ** out,
			    void * data )
	{
	    Tessellator * inst = (Tessellator*)data;
	    
	    int n = inst->mesh.numVertices();
	    inst->mesh.vertex( arma::vec({(double)coords[0], (double)coords[1], (double)coords[2]}) );
	    *out = (void*)(long)n;
	    //printf("combine\n");
	}

    static void edge_cb( GLenum flag, void * data  )
	{
	}
    
    
    int count;
    Mesh mesh;
    GLUtesselator * tess;
    std::vector<XYZ> verts;
};

Mesh toMesh( const Shape& shape, int winding=WINDING_ODD );
Mesh toMesh( const Contour& shape, int winding=WINDING_ODD );

namespace gfx
{

bool init();
void exit();

// EPS rendering
void 	beginEps( const std::string & path, const Box & rect );
void 	endEps();
bool 	isRenderingToEps();

/// Clear frame and depth buffers
void clear( float r, float g, float b, float a, bool depth=true, float depthClear=1.0f );
/// Clear frame and depth buffers
void clear( const V4& clr, bool depth=true, float depthClear=1.0f );
/// Clear depth buffer
void clearDepth( float depthClear );
/// Clear stencil buffer
void clearStencil( int stencilClear );
/// Enable color write
void enableColorWrite( bool r, bool g, bool b, bool a );
/// Enable blending
void enableBlend( bool flag );
/// Set blending mode
/// One of BLENDMODE flags
void setBlendMode( int mode );
/// Set fill mode 
/// One of FILLMODE flags
void setFillMode( int mode );
/// Set culling mode, one of CULL flags
void setCullMode( int cull );
/// Toggle writing to depth buffer
void enableDepthWrite( bool flag );
/// Toggle reading to depth buffer
void enableDepthRead( bool flag );
/// Toggle depth buffer
void enableDepthBuffer( bool flag );
/// Set depth testing func
void setDepthFunc( int func );
/// Toggle stencil buffer write 
void enableStencilWrite( bool flag );
/// Toggle stencil buffer read
void enableStencilRead( bool flag );
/// Toggle stencil buffer 
void enableStencilBuffer( bool flag );
/// Toggle stencil buffer function 
void setStencilFunc( int func, int ref, unsigned int mask );

void setStencilOp( int fail, int zfail, int zpass );

void setStencilOp( int failFront, int zfailFront, int zpassFront,
			      int failBack, int zfailBack, int zpassBack );

void setPointSize( float s );
/// Enable point sprites
void enablePointSprites( bool flag, int textureIndex );
/// Toggle anti-aliasing
void enableAntiAliasing( bool aa );
/// Set line width
void lineWidth( float w );

/// Stiplled line
/// with factor = 0 line stipple is disabled.
void lineStipple( int factor, unsigned short pattern=0xAAAA );

/// Bind texture handle
void bindTexture( int id, int sampler=0 );
/// Unbind texture handle
void unbindTexture( int sampler=0 );
/// Set active texture unit
void activeTexture( int sampler );
    
////////////////////////////////////////////////// 
/// Projections/view/transformations
//@{
/// Push viewport on stack
void pushViewport();
/// Pop viewport from stack
void popViewport();
/// Set current viewport. Note: y0 is at bottom of screen.
void setViewport( int x, int y, int w, int h );

/// Sets identity to projection and model-view matrices
void setIdentityTransform();
/// Sets perspective projection (as in MESA standard)
void setPerspectiveProjection(float fovy, float aspect, float zNear, float zFar);
/// Set projection matrix by specifying view frustum 
void setFrustum( float left, float right, float bottom, float top, float near, float far );
/// Set orthographic projection
void setOrtho(float x,float y,float w, float h, float zNear = -1.0, float zFar = 1.0);
/// Set orthographic projection with origin at (0,0)
void setOrtho( float w, float h );

/// Look-at view matrix, Mesa 3d Implementation
void lookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
		   GLfloat centerx, GLfloat centery, GLfloat centerz,
		   GLfloat upx, GLfloat upy, GLfloat upz);

/// Sets view and projection matrix, imitating a orthographic projection on z=0
void set2DView( float w, float h, int rotate );
/// Set projection transform
void setProjectionMatrix( const M44& mat );
/// Set model view transform
void setModelViewMatrix( const arma::mat& mat );
/// Multiply current transform
void applyMatrix( const arma::mat& mat );
/// Get current projection matrix
M44 getProjectionMatrix();
/// Get current model-view matrix
M44 getModelViewMatrix();
/// Get current viewport
V4 getViewport();
    
/// Set current (mdoel-view) matrix to identity
void identity();

/// Translate model-view
void translate( const arma::vec& v );
/// Translate model-view
void translate( float x, float y , float z = 0.0 );
/// Rotate model-view
void rotate( float x, float y, float z  );
/// 2d rotate model-view (around z axis)
void rotate( float z );
/// scale model-view
void scale( float x, float y, float z = 1.0 );
/// uniform scale model-view
void scale( float s );

/// Push a matrix to the stack
void pushMatrix( const arma::mat& m );
/// Push new transformation to matrix stack 
void pushMatrix();
/// pop matrix stack
void popMatrix();
//@}

//////////////////////////////////////////////////
/// Drawing utilities
//@{
/// Specify a vertex, given a 2d or 3d vertex
void vertex( const arma::vec& v  );
    
/// Specify a 3d vertex
void vertex( float x, float y, float z  );
/// Specify a 2d vertex
void vertex( float x, float y );
/// Specify a normal
void normal( float x, float y, float z );
/// Specify a normal
void normal( const arma::vec& v );
    
/// Specify uv coordinates 
void uv( float u, float v, int texIndex=0  );
void uv( const V2& v, int texIndex=0 );

/// Specify color (r,g,b,a)
void color( const V4& c );

/// Specify grayscale color (l, alpha)
void color( float c, float a=1.0f );
/// Specify color (r,g,b,a) 
void color( float r, float g, float b, float a=1.0f );

/// Begin specifying vertices, equivalent to glBegin
/// @prim defines the primitive type with @PRIMITIVE
void beginVertices( int prim );
/// End sepcifying vertices
void endVertices();

/// Draw a 2d quad providing texture coordinates (u,v)
void drawUVQuad( float x  , float y  , float w , float h, float maxU=0.0f, float maxV=1.0f, bool flip=true );
/// Draw a 2d quad
void drawQuad( float x  , float y  , float w , float h );

/// Fill a circle
/// *** ADAPTED FROM LIBCINDER
void fillCircle( const V2& center, float radius, int numSegments = 0);
/// Draw a circle outline
/// *** ADAPTED FROM LIBCINDER
void drawCircle( const V2& center, float radius, int numSegments = 0 );

/// Draw triangle
void drawTriangle( const V2& a, const V2& b, const V2& c );
/// Fill a triangle
void fillTriangle( const V2& a, const V2& b, const V2& c );

/// Draw a rectangle
void drawRect( const Box& rect );
/// Fill a rectangle
void fillRect( const Box& rect );
/// Draw a rectangle
void drawRect( float x, float y, float w, float h );
/// Fill a rectangle
void fillRect( float x, float y, float w, float h );

/// Draw a line between a and b
void drawLine( const arma::vec& a, const arma::vec& b );
/// Draw a line
void drawLine( float x0, float y0, float x1, float y);
/// Draw an arrow
void drawArrow( const V2& a, const V2& b, float size );
/// Draw a 2d X marker at pos
void drawXMarker( const V2& pos, float size );

/// Draw a contour
void draw( const Contour& C );
/// Draw part of a contour
void draw( const Contour& C, int from, int to );
/// Draw a contour
void draw( const Shape& S );
/// Draw 2d/3d columns of a matrix
void draw( const arma::mat& P, bool closed, int from=-1, int to=-1 );
/// Draw a mesh
void draw( const Mesh& mesh );

/// Fill a contour
void fill( const Contour& shape, int winding=WINDING_ODD );
/// Fill a shape
void fill( const Shape& shape, int winding=WINDING_ODD );

/// Draw an image (note this is SLOW, but handy for opencv interop)
void image( Image& img, float x, float y, float w=0., float h=0. );
/// Draw an image (note this is SLOW, but handy for opencv interop)
void image( const arma::uchar_cube& img, float x, float y, float w=0., float h=0. );

/// draw a set of primitives (2d/3d), @prim specifies primitive type (PRIMITIVE)
void drawPrimitives( const arma::mat & P, int prim, int offset=0, int inc = 1 );
/// draw a set of primitives (2d/3d), @prim specifies primitive type (PRIMITIVE)
void drawPrimitives( const std::vector<V3> & P, int prim, int offset=0, int inc = 1 );
/// draw a set of primitives (2d/3d), @prim specifies primitive type (PRIMITIVE)
void drawPrimitives( const std::vector<V2> & P, int prim, int offset=0, int inc = 1 );
    
/// Draw a 3d box, given min and max coords
void drawWireBox( const arma::vec& min, const arma::vec& max );

/// Draw a cuboid 
void drawCube( const V3& center, const V3&size );
/// draw a wireframe cuboid
void drawWireCube( const V3& center, const V3&size );
/// Draw a sphere
void drawSphere( const V3& center, float radius, int segments=100);

/// Draw 3d axes defined by a 4x4 matrix
void drawAxis( const M44& mat, float scale  );

/// Draw frustrum given a projection matrix and the inverse of the view matrix
void drawFrustum( const M44& proj_, const M44& invView_ );

/// Draw 2d multivariate gaussian
void drawGauss2d( const arma::vec& mu, const arma::mat& Sigma, const arma::vec& clr=arma::vec({1.0,0.5,0.0,0.3}), float stdDevScale=3.,  int subd=30 );
/// Draw 3d multivariate gaussian
void drawGauss3d( const arma::vec& mu, const arma::mat& Sigma, const arma::vec& clr=arma::vec({1.0,0.5,0.0,0.3}), float stdDevScale=3., bool wireFrame=false, int subd=30 );

//@}


//////////////////////////////////////
/// Shader interface (needs further testing)
//@{
void removeShader( int id );
void deleteShaderProgram( int id );
void deleteAllShaders();
    
int loadShader( const std::string& vs, const std::string& ps );
int reloadShader( int id, const std::string& vs, const std::string& ps );
std::string shaderString( const std::string& path );
bool setTexture( const std::string& handle, int sampler );
void bindShader( int id );
void unbindShader();

bool setShaderInt( const std::string& handle, int v );
bool setShaderBool( const std::string& handle, bool v );
bool setShaderFloat( const std::string& handle, float v );
bool setShaderFloat2( const std::string& handle, const V2& v );
bool setShaderFloat3( const std::string& handle, const V3& v );
bool setShaderFloat4( const std::string& handle, const V4& v );
bool setShaderM44( const std::string& handle, const M44& v );
//bool setM33( const std::string& handle, const M33& v );
//bool setM44Array( const std::string& handle, const std::vector<M44>& v );
bool setShaderFloatArray( const std::string& handle, float*v, int n);

///
// Colors
//@}

} // end gfx

    
struct Color
{
public:
    static V4 black() { return V4(0.,0.,0.0,1.); }
    static V4 white() { return V4(1,1,1,1); }
   	static V4 red()  { return V4(1,0,0,1); }
    static V4 green() { return V4(0,1,0,1); }
    static V4 blue() { return V4(0,0,1,1); }
    static V4 turquoise() { return V4(0, 196. / 255., 200. / 255, 1.); }
    static V4 magenta() { return V4(1,0,1,1); }
    static V4 lemon() { return V4(195.0f/255,255.0f/255,64.0/255,1.); }
    static V4 grey() { return V4(0.5,0.5,0.5,1.); }
    static V4 darkGrey() { return V4{0.2, 0.2, 0.2, 1.}; }

	static V4 hsv(float h, float s, float v);
};


    

}

