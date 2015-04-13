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

#ifdef CM_OFX
#include "ofMain.h"
#endif

/// Some ifdef hell
#ifdef CM_WIN32

	#include <windows.h>
	#include <GL/glu.h>
		
	#ifndef GL_CLAMP_TO_EDGE
	#define GL_CLAMP_TO_EDGE 0x812F
	#endif
#else 

#ifdef CM_LINUX
#define GL_GLEXT_PROTOTYPES
	 #include <GL/glew.h>
     #include <GL/gl.h>
     #include <GL/glx.h>

	#define CM_GLCONTEXT
	#define CM_GETGLCONTEXT

#else
	#ifndef CM_GLES
		#include <OpenGL/glu.h>
		#include <OpenGL/OpenGL.h>
		#include <OpenGL/gl.h>
		#include <OpenGL/glext.h>
		
	#else // OPENGL ES
		#include <OpenGLES/ES1/gl.h>
		#include <OpenGLES/ES1/glext.h>
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
	#endif
		

	// On mac only define context macros
	#ifdef CGLCONTEXTS
	#include <OpenGL/CGLMacro.h>
	#define CM_GLCONTEXT	CGLContextObj cgl_ctx = CGLGetCurrentContext();	
	#define CM_GETGLCONTEXT	cgl_ctx = CGLGetCurrentContext();	
	#else
	#define CM_GLCONTEXT
	#define CM_GETGLCONTEXT
	#endif

#endif
#endif


#define CM_GLERROR		cmGetGLError()
#define CM_GLVERBOSE	1	// if 1 gl errors are printed out


int cmGLVersion();
void cmSetGLVersion( int version );

// error checking
bool cmGetGLError();
bool cmCheckFramebufferStatus();
bool cmAreNonPowerOfTwoTexturesSupported();
void cmSetPowerOfTwoTexturesSupport( bool flag );

bool cmGLExtension( const std::string &extName );

GLuint cmCreateVBO(const void* data, int dataSize, GLenum target, GLenum usage);

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


// GLES definitions to make life easier with slight name changes
#ifdef CM_GLES
#define GL_FRAMEBUFFER_EXT GL_FRAMEBUFFER
#define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE	
#define glCheckFramebufferStatusEXT glCheckFramebufferStatus

#define GL_FRAMEBUFFER_UNSUPPORTED_EXT GL_FRAMEBUFFER_UNSUPPORTED
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS

#define GL_COLOR_ATTACHMENT0_EXT GL_COLOR_ATTACHMENT0
#define glGenFramebuffersEXT glGenFramebuffers
#define glBindFramebufferEXT glBindFramebuffer
#define glDeleteFramebuffersEXT glDeleteFramebuffers

#define GL_RENDERBUFFER_EXT GL_RENDERBUFFER
#define glGenRenderbuffersEXT glGenRenderbuffers
#define glBindRenderbufferEXT glBindRenderbuffer
#define glDeleteRenderbuffersEXT glDeleteRenderbuffers	
#define glRenderbufferStorageEXT glRenderbufferStorage	

#define glFramebufferTexture2DEXT glFramebufferTexture2D	
#define glFramebufferRenderbufferEXT glFramebufferRenderbuffer 
#define GL_DEPTH_ATTACHMENT_EXT GL_DEPTH_ATTACHMENT
#define GL_CLAMP GL_CLAMP_TO_EDGE //?
#endif

namespace cm
{
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

bool setManualGLRelease( bool flag );
bool isGLReleaseManual();
void releaseGLObject( const GLObj & obj );
void releaseGLObjects();
	
}

