///////////
/// RESAPI	
/// GL gfx utilities
///
/// © Daniel Berio 2010
/// http://www.enist.org
///
///
///////////

#include "gfx/CMGLInternal.h"
#include "core/CMUtils.h"
#include "gfx/CMGL.h"

static int glVersion = 1;

int cmGLVersion() { return glVersion; }
void cmSetGLVersion( int version ) { glVersion = version; }


/// check GL version
#ifndef CM_GLES
bool cmAreNonPowerOfTwoTexturesSupported() { return cmGLExtension("GL_ARB_texture_non_power_of_two"); }
#else
static bool bPowerOf2Tex = true;
 
bool cmAreNonPowerOfTwoTexturesSupported() { return bPowerOf2Tex; }
void cmSetPowerOfTwoTexturesSupport( bool flag )
{
	bPowerOf2Tex = flag;
}

#endif

/// From NVIDIA code
bool cmCheckFramebufferStatus()
{
	CM_GLCONTEXT

	GLenum status;
	status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status) 
	{
		
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			//debugPrint("GL_FRAMEBUFFER_COMPLETE!");
			return true;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			cm::debugPrint("GL_FRAMEBUFFER_UNSUPPORTED!");
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			cm::debugPrint("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT!");
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			cm::debugPrint("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT!");
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			cm::debugPrint("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT!");
			return false;
		/*
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			debugPrint("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT!");
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			debugPrint("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER!");
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			debugPrint("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER!");
			return false;	
		*/
		default:
			cm::debugPrint("Unknown GL_FRAMEBUFFER error!");
			cmssert(0);
			return false;
	}

	return false;
}

bool cmGetGLError() 
{
	CM_GLCONTEXT
	
	
	GLenum err = glGetError();
	//glClearError();
	if( err != GL_NO_ERROR )
	{
		#ifndef CM_GLES
		if(CM_GLVERBOSE)
			cm::debugPrint("GL ERROR %d %s",(int)err,gluErrorString(err));
		#endif
		
		return true;
	}
	
	return false;
	
}

// Got this snippet from LibCinder
bool cmGLExtension( const std::string &extName )
{
	CM_GLCONTEXT
	
	static const char *sExtStr = reinterpret_cast<const char*>( glGetString( GL_EXTENSIONS ) );
	static std::map<std::string,bool> sExtMap;
	
	std::map<std::string,bool>::const_iterator extIt = sExtMap.find( extName );
	if( extIt == sExtMap.end() ) 
	{
		bool found = false;
		int extNameLen = extName.size();
		const char *p = sExtStr;
		const char *end = sExtStr + strlen( sExtStr );
		while( p < end ) 
		{
			int n = strcspn( p, " " );
			if( (extNameLen == n) && ( strncmp(extName.c_str(), p, n) == 0)) 
			{
				found = true;
				break;
			}
			p += (n + 1);
		}
		sExtMap[extName] = found;
		return found;
	}
	else
		return extIt->second;
}

GLuint cmCreateVBO(const void* data, int dataSize, GLenum target, GLenum usage)
{
	CM_GLCONTEXT
	
	GLuint glid = 0;  // 0 is reserved, glGenBuffersARB() will return non-zero id if success
	
	glGenBuffers(1, &glid);                        // create a vbo
	glBindBuffer(target, glid);                    // activate vbo id to use
	glBufferData(target, dataSize, data, usage); // upload data to video card
	
	// check data size in VBO is same as input array, if not return 0 and delete VBO
	GLint bufferSize = 0;
	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
	if(dataSize != bufferSize)
	{
		glDeleteBuffers(1, &glid);
		glid = 0;
		cm::debugPrint("VBO creation error: data size mismatch\n");
	}
		
	glBindBuffer(target, 0); 
	
	return glid;  
}



namespace cm
{
	static std::vector<GLObj> objects;
	
	void releaseGLObject( const GLObj & obj )
	{
		objects.push_back(obj);
	}
	
	void releaseGLObjects()
	{
		for( int i = 0; i < objects.size(); i++ )
		{
			GLObj & o = objects[i];
			switch(o.type)
			{
				case GLObj::TEXTURE:
					glDeleteTextures(1,&o.glId);
					break;
					
				case GLObj::SHADER:
					glDeleteShader(o.glId);
					break;
					
				case GLObj::PROGRAM:
					glDeleteProgram(o.glId);
					break;
					
				case GLObj::VB:
					glDeleteBuffers(1,&o.glId);
					break;

				case GLObj::IB:
					glDeleteBuffers(1,&o.glId);
					break;

				case GLObj::FBO:
					glDeleteFramebuffersEXT(1,&o.glId);
					break;
					
				case GLObj::RENDERBUFFER:
					glDeleteRenderbuffersEXT(1,&o.glId);
					break;
					
				
			}
		}
		
		objects.clear();
	}
	


static bool bManual;

bool setManualGLRelease( bool flag )
{
	bManual = flag;
}

bool isGLReleaseManual()
{
	return bManual;
}

}