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

#pragma once 

#include "gfx/cmGfxIncludes.h"

#ifdef CM_OFX
#include "ofImage.h"
#endif

#include "../libs/stb/cmStbImage.h"

namespace cm
{

enum CUBE_FACE
{
	CUBE_POS_X = 0,
	CUBE_NEG_X = 1,
	CUBE_POS_Y = 2,
	CUBE_NEG_Y = 3,
	CUBE_POS_Z = 4,
	CUBE_NEG_Z = 5,
	CUBE_FORCE_DWORD = 0xffffffff,
};



struct TextureInfo
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
struct 	TextureLockInfo
{
	void * buf;
	int	pitch;

	// volume texture specific
	int rowPitch;
    int slicePitch;
	/// mip level that has been locked
	int	level;
	/// this is trash if texture isnt a cube map
	CUBE_FACE face;
};



// Use App is an app running  structure?
// CMMath   CMApp		
// TODO add texture transformation matrix?
// texture specific states, linear nearest etc

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

	public:
		Texture();
		Texture( const char * path, int miplevels = 1 );
		Texture( int w, int h, int  fmt, int miplevels = 1 );
	
		~Texture();
		
		void	release();
		
		void	bind( int sampler = 0 );
		void	unbind();
		
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
		
		/// draw a quad with texture information but don't bind
		static void drawQuad( Texture * tex, float x = -1.0f , float y = -1.0f , float w = 2.0f , float h = 2.0f  );
		static void drawQuad2( Texture * tex, Texture * tex2, float x = -1.0f , float y = -1.0f , float w = 2.0f , float h = 2.0f  );
		
		
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
		TextureInfo info;
	protected:
		/// sampler that this texture has been bound to, -1 is no sampler
		int		_boundSampler;
		
		int		_format;
		
		bool	setPixelFunc();
		/// pixel manipulation callback
		/// gets set when format is specified.
		void	(*_pixelfunc) ( void * out, int x, int y, int z, int pitch, int slicepitch, float r, float g, float b, float a );
	
		/// information on locked texture
		TextureLockInfo			_lockInfo;
		int		_lockFlags;
		
		bool	_initialized;
		
	
};

bool grabFrameBuffer( void * data, int width, int height, Texture::FORMAT fmt );

}