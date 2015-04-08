

#include "gfx/cmGLInternal.h"
#include "gfx/cmTexture.h"
#include "gfx/cmPixelFuncs.h"


#define ASSERTBOUND		assert( _boundSampler != -1 );

namespace cm
{

////////////////////////////////////////////////////////////


struct GLPixelFormatDesc
{
	GLint	glFormat;
	GLenum	glDataFormat;
	GLenum	glDataType;
	int		bytesPerPixel;
};

////////////////////////////////////////////////////////////


static GLPixelFormatDesc glFormatDescs[Texture::NUM_SUPPORTED_FORMATS] = 
{
#ifndef CM_GLES
	{ GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE,3 }, //R8G8B8 = 0,
	{ GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE,4 }, //A8R8G8B8, //GL_BGRA
	{ GL_INTENSITY8, GL_LUMINANCE, GL_UNSIGNED_BYTE,1 }, //L8,

	// FLOAT TEXTURE FORMATS

	// 16-BIT FLOATING POINT
	{ GL_INTENSITY_FLOAT16_ATI, GL_LUMINANCE, GL_UNSIGNED_SHORT,2 }, //R16F,
	// 32-BIT FLOATING POINT
	{ GL_LUMINANCE_ALPHA_FLOAT16_ATI, GL_LUMINANCE_ALPHA, GL_UNSIGNED_SHORT,4 }, //G16R16F,
	// 64-BIT FLOATING POINT
	{ GL_RGBA_FLOAT16_ATI, GL_RGBA, GL_UNSIGNED_SHORT,8 }, //A16B16G16R16F,
	// IEEE 32-BIT FLOATING POINT
	{ GL_R32F, GL_RED, GL_FLOAT,4 }, //R32F,
	// IEEE 64-BIT FLOATING POINT
	{ GL_LUMINANCE_ALPHA_FLOAT32_ATI, GL_LUMINANCE_ALPHA, GL_FLOAT,8 }, //G32R32F,
	// IEEE 128-BIT FLOATING POINT
	{ GL_RGBA_FLOAT32_ATI, GL_RGBA, GL_FLOAT,16 }, //A32B32G32R32F,
#else
	{ GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,3 }, //R8G8B8 = 0,
	{ GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,4 }, //A8R8G8B8,
	{ GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE,1 }, //L8,

			
	{ GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,3 }, //R5G6B5 = 0,
	{ GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4,4 }, //A4R4G4B4,
	{ GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1,4 }, //A1R5G5B5,
	
#endif

};

static GLenum glWrapTypes[Texture::NUM_WRAP_TYPES] = 
{
	GL_CLAMP_TO_EDGE,
	GL_CLAMP,
	GL_REPEAT
};

static GLenum glFilterTypes[Texture::NUM_FILTER_TYPES] = 
{
	GL_NEAREST,
	GL_LINEAR
};
////////////////////////////////////////////////////////////


static Texture	**currentSamplers = 0;
static GLint		maxTextureUnits = 0;
static int		textureCount = 0;



////////////////////////////////////////////////////////////

static void	initTexturing()
{
	CM_GLCONTEXT
	
	textureCount++;
	
	if( currentSamplers )
		return;
		
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);

	currentSamplers = new Texture* [maxTextureUnits];
	for( int i = 0; i < maxTextureUnits; i++ )
		currentSamplers[i] = 0;
}


////////////////////////////////////////////////////////////


static void deinitTexturing()
{
	if(textureCount)
		textureCount--;
	if( textureCount == 0 )
		SAFE_DELETE_ARRAY( currentSamplers );
}


////////////////////////////////////////////////////////////

void Texture::resetAllSamplers()
{
	CM_GLCONTEXT
	
	for( int i = 0; i < maxTextureUnits; i++ )
	{
		if(currentSamplers[i])
		{
			const TextureInfo & info = currentSamplers[i]->info; 
			glActiveTexture( GL_TEXTURE0+i );
			glBindTexture(info.glTarget,0);
			glDisable(info.glTarget); // TODO check me maybe it is not necessary
		}
		
		currentSamplers[i] = 0;
	}
	
	glActiveTexture( GL_TEXTURE0 );
}



////////////////////////////////////////////////////////////



Texture::Texture()
:
_boundSampler(-1),
_format(FORMAT_UNKNOWN),
flipX(false),
flipY(false)

{
	memset( &info, 0, sizeof( TextureInfo ) );
	memset( matrix, 0, sizeof( matrix ) );
	matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
}

Texture::Texture( const char * path, int miplevels )
	:
	_boundSampler(-1),
	_format(FORMAT_UNKNOWN),
	flipX(false),
	flipY(false)
{
	memset( &info, 0, sizeof( TextureInfo ) );
	memset( matrix, 0, sizeof( matrix ) );
	matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
	load(path,miplevels);
}

Texture::Texture( int w, int h, int fmt, int miplevels  )
{
	memset( &info, 0, sizeof( TextureInfo ) );
	memset( matrix, 0, sizeof( matrix ) );
	matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
	create(w,h,fmt,miplevels);
}
	

////////////////////////////////////////////////////////////


Texture::~Texture()
{
	release();
	
}


////////////////////////////////////////////////////////////


void	Texture::release()
{
	CM_GLCONTEXT
	
	unbind();
	
	if( info.glId )
	{
		deinitTexturing();
		if( isGLReleaseManual() )
		{
			releaseGLObject( GLObj(info.glId,GLObj::TEXTURE) );
		}
		else
			glDeleteTextures(1,&info.glId);
	}
	
	memset( &info, 0, sizeof( TextureInfo ) );
	
	
}



////////////////////////////////////////////////////////////



void	Texture::bind( int sampler )
{
	CM_GLCONTEXT
	
	if(!isValid())
		return;
		
	glActiveTexture( GL_TEXTURE0+sampler );
	glEnable(info.glTarget);
	glBindTexture(info.glTarget,info.glId);
	
	currentSamplers[sampler] = this;
	_boundSampler = sampler;
	
	if(CM_GLERROR)
		debugPrint("glerror\n");
}


////////////////////////////////////////////////////////////



void	Texture::unbind()
{
	CM_GLCONTEXT
	
	if( isBound() )
	{
		glActiveTexture( GL_TEXTURE0+_boundSampler );
		glDisable(info.glTarget);
		glBindTexture(info.glTarget,0);
		currentSamplers[_boundSampler] = 0;
	}
	
	
	
	_boundSampler = -1;
	
	if(CM_GLERROR)
		debugPrint("glerror\n");
	
	glActiveTexture( GL_TEXTURE0 );
}
		

void	Texture::setWrap( int  wrap )
{
	CM_GLCONTEXT
	// texture must be bound!
	ASSERTBOUND
	glTexParameteri( info.glTarget, GL_TEXTURE_WRAP_S, glWrapTypes[wrap] ); 
	glTexParameteri( info.glTarget, GL_TEXTURE_WRAP_T, glWrapTypes[wrap] );
}

////////////////////////////////////////////////////////////

void	Texture::setWrapS( int  wrap )
{
	CM_GLCONTEXT
	// texture must be bound!
	ASSERTBOUND
	glTexParameteri( info.glTarget, GL_TEXTURE_WRAP_S, glWrapTypes[wrap] ); 
}

////////////////////////////////////////////////////////////

void	Texture::setWrapT( int  wrap )
{ 
	CM_GLCONTEXT
	// texture must be bound!
	ASSERTBOUND
	glTexParameteri( info.glTarget, GL_TEXTURE_WRAP_T, glWrapTypes[wrap] );
}

////////////////////////////////////////////////////////////

void	Texture::setMinFilter( int  filter )
{
	CM_GLCONTEXT
	// texture must be bound!
	ASSERTBOUND
	glTexParameteri( info.glTarget, GL_TEXTURE_MIN_FILTER, glFilterTypes[filter] ); 
}

////////////////////////////////////////////////////////////

void	Texture::setMagFilter( int  filter )
{
	CM_GLCONTEXT
	// texture must be bound!
	ASSERTBOUND
	glTexParameteri( info.glTarget, GL_TEXTURE_MAG_FILTER, glFilterTypes[filter] ); 
}


////////////////////////////////////////////////////////////


void Texture::init()
{
	release();
	initTexturing();
}

bool	Texture::load( const char * path, int miplevels )
{
	return textureFromImage(this,path,0,miplevels);
}

bool	Texture::create( void * idata, int w, int h, int fmt, int mipLevels )
{
	init();
	
	if(fmt == Texture::FORMAT_UNKNOWN)
		return false;
		
	CM_GLCONTEXT
	
	// reset all samplers
	resetAllSamplers();
	
	glDisable( GL_TEXTURE_RECTANGLE );
	glEnable( GL_TEXTURE_2D );

	GLint prevAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	void * data = idata;
	
	_format = fmt;
	
	info.width =  w;
	info.height =  h;
	
	if( cmAreNonPowerOfTwoTexturesSupported() )
	{
		info.hwWidth = w;
		info.hwHeight = h;
	}
	else 
	{
		
		info.hwWidth = NPOT(w);
		info.hwHeight = NPOT(h);
		
		// realign data
		int hww = info.hwWidth;
		int hwh = info.hwHeight;
		
		int sz = glFormatDescs[fmt].bytesPerPixel;
		
		unsigned char * src = (unsigned char*)data;
		unsigned char * dst = new unsigned char[ hww*hwh*sz ];
		data = dst;
		
		memset( dst, 0, hww*hwh*sz );
		for( int y = 0; y < h; y++ )
		{
			memcpy(&dst[y*hww*sz],&src[y*w*sz],w*sz);
		}
	}

	info.maxU = ((float)info.width) / info.hwWidth;
	info.maxV = ((float)info.height) / info.hwHeight;
	
	const GLPixelFormatDesc & desc = glFormatDescs[fmt];
	
	info.glTarget = GL_TEXTURE_2D;
	
	info.glFormat = desc.glFormat;
	info.glDataType = desc.glDataType;
	info.glDataFormat = desc.glDataFormat;
	info.bytesPerPixel = desc.bytesPerPixel;
	
	info.mipLevels = mipLevels;
	
	// this could be handy
	// setPixelFunc();

	if(CM_GLERROR)
		return false;

	glGenTextures(1, &info.glId);					// Create 1 Texture
	glBindTexture(GL_TEXTURE_2D, info.glId);			// Bind The Texture

	/// hack to enable texturing ? TODO check...
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if(mipLevels == 0 || mipLevels > 1 )
	{
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );//GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );//GL_CLAMP_TO_EDGE);

	if(CM_GLERROR)
		return false;
	
	#ifndef CM_GLES
		
		if( mipLevels == 0 && cmGLExtension("GL_SGIS_generate_mipmap") )
		{
			glTexImage2D( GL_TEXTURE_2D,
						 0,
						 info.glFormat,
						 info.hwWidth,
						 info.hwHeight,
						 0,
						 info.glDataFormat,
						 info.glDataType,
						 data );

			if(CM_GLERROR)
				return false;
			
			// auto gen mip maps... /// ?? testme
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
		else if( mipLevels == 1)
		{
			glTexImage2D( GL_TEXTURE_2D,
						  0,
						  info.glFormat,
						  info.hwWidth,
						  info.hwHeight,
						  0,
						  info.glDataFormat,
						  info.glDataType,
						  data );
		}
		else
		{
			// we might be here because glGeneratetMipmapEXT is not supported
			if( mipLevels == 0 )
				mipLevels = 4; // HACK make sense out of this.
				
			 gluBuild2DMipmaps( GL_TEXTURE_2D,
								mipLevels,
								info.hwWidth,
								info.hwHeight,
								info.glDataFormat,
								info.glDataType,
								data );
								
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		} 
	#else
		// HACK see how to handle mipmaps in opengl ES
		glTexImage2D( GL_TEXTURE_2D,
						  0,
						  info.glFormat,
						  info.hwWidth,
						  info.hwHeight,
						  0,
						  info.glDataFormat,
						  info.glDataType,
						  data );
		mipLevels = 1;
		info.mipLevels = 1;
	#endif
	
	if(CM_GLERROR)
		return false;
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);

	if(CM_GLERROR)
		return false;

	glDisable(GL_TEXTURE_2D);
	
	// set pixel callback
	setPixelFunc();
	
	
	return true;

}



////////////////////////////////////////////////////////////




bool	Texture::create( int w, int h, int  fmt, int mipLevels )
{
	const GLPixelFormatDesc & desc = glFormatDescs[fmt];
	
	int bufW = w;
	int bufH = h;
	if( !cmAreNonPowerOfTwoTexturesSupported() )
	{
		bufW = NPOT(w);
		bufH = NPOT(h);
	}
	
	int pxSize = desc.bytesPerPixel;
	char *data = new char[bufW*bufH*pxSize];
	memset(data,0,bufW*bufH*pxSize);	// Clear Storage Memory

	bool res = create(data,w,h,fmt,mipLevels);
	
	delete [] data;
	
	return res;
}


////////////////////////////////////////////////////////////

void	Texture::setGLData(	int w, int h,
								int hww, int hwh,
							  GLuint	glTarget,
							  GLuint	glId,
							  GLint		glFormat )
{
	info.width = w;
	info.height = h;
	info.hwWidth = w;
	info.hwHeight = h;
	info.glTarget = glTarget;
	info.glFormat = glFormat;
	info.glId = glId;
	
	switch( glTarget )
	{
		case GL_TEXTURE_2D:
		{
			info.maxU = ((float)info.width) / info.hwWidth;
			info.maxV = ((float)info.height) / info.hwHeight;
		}
			break;
		
		#ifndef CM_GLES
		case GL_TEXTURE_RECTANGLE:
		{
			info.maxU = w;
			info.maxV = h;
		}
			break;
		#endif
		
			
	}
	
}


void	Texture::createFromGL( int w, int h,
					  GLuint	glTarget,
					  GLuint	glId,
					  GLint		glFormat )
{
	init();

	info.width = w;
	info.height = h;
	info.glTarget = glTarget;
	info.glFormat = glFormat;
	info.glId = glId;
	
	switch( glTarget )
	{
		case GL_TEXTURE_2D:
		{
			if( cmAreNonPowerOfTwoTexturesSupported() )
			{
				info.hwWidth = w;
				info.hwHeight = h;
			}
			else 
			{
				info.hwWidth = NPOT(w);
				info.hwHeight = NPOT(h);
			}

			info.maxU = ((float)info.width) / info.hwWidth;
			info.maxV = ((float)info.height) / info.hwHeight;
		}
			break;
		
		#ifndef CM_GLES
		case GL_TEXTURE_RECTANGLE:
		{
			info.hwWidth = w;
			info.hwHeight = h;
			info.maxU = w;
			info.maxV = h;
		}
			break;
		#endif
		
			
	}
	
}

////////////////////////////////////////////////////////////

bool Texture::isValid()
{
	return info.glId != 0;
}


bool Texture::update( void * data, int w, int h)
{
	CM_GLCONTEXT
	
	bind();
	
	GLint prevAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexSubImage2D(info.glTarget,
				 0, //level
				 0, //xoffset
				 0, //yoffset
				 w,
				 h,
				 info.glDataFormat,
				 info.glDataType,
				 data);
	
	if(CM_GLERROR)
		return false;
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);
	
	unbind();
	
	return true;
}

bool Texture::update( void * data, int x, int y, int w, int h)
{
	CM_GLCONTEXT
	
	bind();
	
	GLint prevAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexSubImage2D(info.glTarget,
				 0, //level
				 x,
				 y,
				 w,
				 h,
				 info.glDataFormat,
				 info.glDataType,
				 data);
	
	if(CM_GLERROR)
		return false;
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);
	
	unbind();
	
	return true;	
	
}
					  

void 	Texture::setSize( int width, int height )
{
	info.width = width;
	info.height = height;
	info.maxU = ((float)info.width) / info.hwWidth;
	info.maxV = ((float)info.height) / info.hwHeight;
}


////////////////////////////////////////////////////////////


bool Texture::grabFrameBuffer()
{
	CM_GLCONTEXT
	bind();
	glCopyTexSubImage2D(info.glTarget, //target
						0, //level
						0, //xoffset
						0, //yoffset
						0, //x
						0, //y
						info.width, //width
						info.height //height
	);	
	
	unbind();
	return true;
}

bool Texture::grabFrameBuffer(int x, int y, int width, int height)
{
	CM_GLCONTEXT
	bind();
	glCopyTexSubImage2D(info.glTarget, //target
						0, //level
						0, //xoffset
						0, //yoffset
						x, //x
						y, //y
						width, //width
						height //height
						);	
	
	unbind();
	return true;
}


////////////////////////////////////////////////////////////


void	Texture::draw( float x  , float y  , float w , float h, bool yUp, float uscale, float vscale )
{
	CM_GLCONTEXT
	
	bool shouldBind = !isBound();
	
	// TODO: same for Geometry!!!
	if(shouldBind)
		bind();
		
	GLfloat tex_coords[8];
	
	float u0 = 0;
	float v0 = 0;
	float u1 = info.maxU*uscale;
	float v1 = info.maxV*vscale;
	
	if(flipX)
		std::swap(u0,u1);
	if(flipY)
		std::swap(v0,v1);
	if(yUp)
		std::swap(v0,v1);
	
	tex_coords[0] = u0; tex_coords[1] = v0; 
	tex_coords[2] = u0; tex_coords[3] = v1; 
	tex_coords[4] = u1; tex_coords[5] = v1; 
	tex_coords[6] = u1; tex_coords[7] = v0; 
	
	/*
	GLfloat verts[] = {
		x,y,0,
		x,y+h,0,
		x+w,y+h,0,
		x+w,h,0
	};*/
	
	GLfloat verts[] = {
		x,y,
		x,y+h,
		x+w,y+h,
		x+w,y
	};


	
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, tex_coords );
	
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT, 0, verts );
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	
	if(shouldBind)
		unbind();

}


/// draw a part of the texture
void	Texture::drawPart( float x, float y, float w, float h, float tx, float ty, float tw, float th )
{
	CM_GLCONTEXT
	
	bool shouldBind = !isBound();
	
	// TODO: same for Geometry!!!
	if(shouldBind)
		bind();

	GLfloat tex_coords[8];
	
	float u0 = tx;///info.width;
	float v0 = ty;///info.height;
	float u1 = (tx+tw);///info.width;
	float v1 = (ty+th);///info.height;
	u1*=info.maxU;
	v1*=info.maxV;
	
	if(flipX)
		std::swap(u0,u1);
	if(flipY)
		std::swap(v0,v1);
		
	tex_coords[0] = u0; tex_coords[1] = v0; 
	tex_coords[2] = u0; tex_coords[3] = v1; 
	tex_coords[4] = u1; tex_coords[5] = v1; 
	tex_coords[6] = u1; tex_coords[7] = v0; 
	
			
	/*
	GLfloat verts[] = {
		x,y,0,
		x,y+h,0,
		x+w,y+h,0,
		x+w,h,0
	};*/
	
	GLfloat verts[] = {
		x,y,
		x,y+h,
		x+w,y+h,
		x+w,y
	};


	
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, tex_coords );
	
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT, 0, verts );
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	
	if(shouldBind)
		unbind();
}

////////////////////////////////////////////////////////////


void	Texture::drawQuad( Texture * tex, float x  , float y  , float w , float h )
{
	CM_GLCONTEXT
	
	const TextureInfo & info = tex->info;
	
	GLfloat tex_coords[8];
	
	float u0 = 0;
	float v0 = 0;
	float u1 = info.maxU;
	float v1 = info.maxV;
	
	if(tex->flipX)
		std::swap(u0,u1);
	if(tex->flipY)
		std::swap(v0,v1);
	
	tex_coords[0] = u0; tex_coords[1] = v0; 
	tex_coords[2] = u0; tex_coords[3] = v1; 
	tex_coords[4] = u1; tex_coords[5] = v1; 
	tex_coords[6] = u1; tex_coords[7] = v0; 
	
	GLfloat verts[] = {
		x,y,
		x,y+h,
		x+w,y+h,
		x+w,y		
	};
	
	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, tex_coords );
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT, 0, verts );
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		
}

void	Texture::drawQuad2( Texture * tex1, Texture * tex2, float x  , float y  , float w , float h )
{
	CM_GLCONTEXT
	
	const TextureInfo & info1 = tex1->info;
	
	GLfloat tex_coords1[8];
	
	tex_coords1[0] = 0.0f; tex_coords1[1] = 0.0f; 
	tex_coords1[2] = 0.0f; tex_coords1[3] = info1.maxV; 
	tex_coords1[4] = info1.maxU; tex_coords1[5] =  info1.maxV; 
	tex_coords1[6] = info1.maxU; tex_coords1[7] = 0.0f; 
	
	const TextureInfo & info2 = tex2->info;
	
	GLfloat tex_coords2[8];
	
	tex_coords2[0] = 0.0f; tex_coords2[1] = 0.0f; 
	tex_coords2[2] = 0.0f; tex_coords2[3] = info2.maxV; 
	tex_coords2[4] = info2.maxU; tex_coords2[5] =  info2.maxV; 
	tex_coords2[6] = info2.maxU; tex_coords2[7] = 0.0f; 
	
	GLfloat verts[] = {
		x,y,
		x,y+h,
		x+w,y+h,
		x+w,y		
	};
	
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_coords1 ); 
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	//glClientActiveTexture(GL_TEXTURE1);
	//glTexCoordPointer(2, GL_FLOAT, 0, tex_coords2 ); 
	//glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glEnableClientState(GL_VERTEX_ARRAY);		
	glVertexPointer(2, GL_FLOAT, 0, verts );
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//	glClientActiveTexture(GL_TEXTURE1);
//	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
}
///////////////////////////////////////////////////////////////////////////

void * Texture::lock(int flags , int mipLevel )
{
	CM_GLCONTEXT
	
	resetAllSamplers();
	
	glEnable( info.glTarget );

	GLint prevAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	_lockInfo.buf = new char[info.hwWidth*info.hwHeight*info.bytesPerPixel];
	_lockFlags = flags;
	
	bind();
	
	
	if(_lockFlags & LOCK_READ )
	{
		#ifndef CM_GLES
		
		glGetTexImage(	info.glTarget,
						mipLevel,
						info.glDataFormat,
						info.glDataType,
						_lockInfo.buf );
						
		#else
			debugPrint("glGetTexImage not supported on OpenGL ES\n");
		#endif
	}
	
	_lockInfo.pitch = info.hwWidth * info.bytesPerPixel;
	_lockInfo.level = mipLevel;


	glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);
	
	return _lockInfo.buf;
}


///////////////////////////////////////////////////////////////////////////

bool Texture::unlock()
{
	CM_GLCONTEXT
	
	GLint prevAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if( _lockFlags & LOCK_WRITE )
	{
		glTexSubImage2D( GL_TEXTURE_2D, _lockInfo.level, 0,0, info.hwWidth, info.hwHeight , info.glDataFormat, info.glDataType, _lockInfo.buf );
	}
	
	if(_lockInfo.buf)
	{
		delete [] (char*)_lockInfo.buf;
	}
	_lockInfo.buf = 0;


	glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);

	unbind();
	
	return true;
}


///////////////////////////////////////////////////////////////////////////

			
bool Texture::setPixelFunc()
{
#define OOPS debugPrint("Texture:Can't set pixel callback\n");
	switch(getFormat())
	{

	case FORMAT_UNKNOWN: 
		_pixelfunc = 0;
		OOPS;
		break;
		// SUPPORTED BY IL AND D3DX IMAGE MODULE
	case R8G8B8:// return 24;
		_pixelfunc = &pixelR8G8B8;
		break;

	case A8R8G8B8:// return 32;
		_pixelfunc = &pixelA8R8G8B8;
		break;

		//case PF_B8G8R8: return 24;
		//case PF_A8B8G8R8: return 32;
	case L8:// return 8;
		_pixelfunc = &pixelL8;
		break;

#ifndef CM_GLES
	case R16F: /// \todo!!
		_pixelfunc = 0;
		OOPS;
		break;
	case G16R16F: /// \todo!!
		_pixelfunc = 0;
		OOPS;
		break;

	case A16B16G16R16F:// return 64;
		_pixelfunc = &pixelA16B16G16R16F;
		break;

	case R32F:// return 32;
		_pixelfunc = &pixelR32F;
		break;

	case G32R32F: 
		/// \todo!!
		_pixelfunc = 0;
		OOPS;
		break;

	case A32B32G32R32F:// return 128;
		_pixelfunc = &pixelA32B32G32R32F;
		break;
#endif
	default: 
		_pixelfunc = 0;
		OOPS;
		break;
	}

	if(_pixelfunc == 0)
		return false;

	return true;
}


#ifdef CM_OFX
bool	Texture::fromOfImage( ofImage & img, int mipLevels )
{
	unsigned char * buf = img.getPixels();
	FORMAT fmt;
	switch(img.bpp)
	{
		case 8:
			fmt = Texture::L8;
			break;
		case 24:
			fmt = Texture::R8G8B8;
			break;
		case 32:
			fmt = Texture::A8R8G8B8;
			break;
		
		default:
			return false;
	}
	
	return create( buf, img.getWidth(),img.getHeight(),fmt,mipLevels );
}

void	Texture::fromOfTexture( ofTexture & tex )
{
	
	
	createFromGL( tex.getTextureData().width, tex.texData.height,
				  tex.getTextureData().textureTarget,
				  tex.getTextureData().textureID,
				  tex.getTextureData().glTypeInternal ); // glTypeInternal?
	flipY = tex.texData.bFlipTexture;
}

#endif


bool	Texture::readPixels( void * data, int width, int height, FORMAT fmt )
{
	CM_GLCONTEXT
	
	const  GLPixelFormatDesc & desc = glFormatDescs[fmt];
	
	glPushAttrib( GL_VIEWPORT_BIT );
	
		//first render texture to screen in ortho mode
	glViewport(0, 0, width, height);

	glClearColor(0.0f, 0.0f , 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glScalef(1,1,1);//0.5,0.5,0.5);
	
	draw();
		
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glPopAttrib();
	
	//Read OpenGL context pixels directly.
#ifdef _WIN32
	//read the pixels from the screen
	GLint dBuffer, rBuffer;
	//glGetIntegerv(GL_READ_BUFFER,&rBuffer);
	//glGetIntegerv(GL_DRAW_BUFFER,&dBuffer);
	//glReadBuffer(dBuffer);
	//glRasterPos2i(0,0);

	glReadPixels(0, 0, width, height, desc.glDataFormat, desc.glDataType, data);
	
	//glReadBuffer(rBuffer);
	
#else
    
    // For extra safety, save & restore OpenGL states that are changed
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
	GLint dBuffer, rBuffer;
	//glGetIntegerv(GL_READ_BUFFER,&rBuffer);
	//glGetIntegerv(GL_DRAW_BUFFER,&dBuffer);
	//glReadBuffer(dBuffer);
	//glRasterPos2i(0,0);

    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    
	GLenum dataType = desc.glDataType;
	if(fmt == A8R8G8B8)
		dataType = GL_UNSIGNED_INT_8_8_8_8_REV;
    //Read a block of pixels from the frame buffer
    glReadPixels(0, 0, width, height, desc.glDataFormat, dataType, data);

	//glReadBuffer(rBuffer);

    glPopClientAttrib();
	
#endif

	return true;
}



bool	grabFrameBuffer( void * data, int width, int height, Texture::FORMAT fmt )
{
	CM_GLCONTEXT
	
	const  GLPixelFormatDesc & desc = glFormatDescs[fmt];
	
	//Read OpenGL context pixels directly.
#ifdef _WIN32
	glReadPixels(0, 0, width, height, desc.glDataFormat, desc.glDataType, data);
	
#else
    
    // For extra safety, save & restore OpenGL states that are changed
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    
	GLenum dataType = desc.glDataType;
	if(fmt == Texture::A8R8G8B8)
		dataType = GL_UNSIGNED_INT_8_8_8_8_REV;
    //Read a block of pixels from the frame buffer
    glReadPixels(0, 0, width, height, desc.glDataFormat, dataType, data);

    glPopClientAttrib();
	
#endif

	return true;
}


}