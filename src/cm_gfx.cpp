 #include "cm_gfx.h"

#define GFX_TO_EPS

#ifdef GFX_TO_EPS
#include "cm_epsfile.h"
#define EPS_ADDP(v) if( renderingToEps && drawingEpsPath ){currentEpsPath.addPoint(v);}
#else
#define EPS_ADDP(v)
#endif


namespace cm
{

#define NANOSVG_IMPLEMENTATION
#include "deps/nanosvg.h"
#include "deps/stb_image.h"
#include "deps/stb_image_write.h"

Mesh toMesh( const Shape& shape, int winding )
{
	Tessellator tess = Tessellator(shape, winding);
	return tess.mesh;
}

Mesh toMesh( const Contour& shape, int winding )
{
	Tessellator tess = Tessellator(shape, winding);
	return tess.mesh;
}

namespace gfx
{


bool init()
{
	#ifdef CM_LINUX
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  printf("Error: %s\n", glewGetErrorString(err));
	  return false;
	}
	#endif

	return true;
}

void deleteAllShaders();

void exit()
{
	deleteAllShaders();
	releaseGLObjects();
}

static int currentPrimitive = -1;

    
#ifdef GFX_TO_EPS
static EpsFile eps;
static bool renderingToEps = false;
static Contour currentEpsPath;
static bool drawingEpsPath = false;
static bool firstPointInPath = false;
static V4 currentColor=V4(0,0,0,1);
#define EPS_LINEWIDTHSCALE 0.5

void 	beginEps( const std::string & path, const Box & rect )
{
	eps.open(path);
	eps.header(rect);
	renderingToEps = true;
	eps.setlinewidth(EPS_LINEWIDTHSCALE);
}
void 	endEps()
{
	eps.close();
	renderingToEps = false;
}

bool 	isRenderingToEps() 
{
	return renderingToEps;
}

V4 	getCurrentColor()
{
	return currentColor;
}

EpsFile *getEps()
{
	return &eps;
}

#endif


static std::vector<GLObj> objects;
	
static bool bManual;

void setManualGLRelease( bool flag )
{
	bManual = flag;
}

bool isGLReleaseManual()
{
	return bManual;
}

    
static void delGLObject( GLObj o )
{
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


void releaseGLObject( GLObj obj )
{
	if(isGLReleaseManual())
	{
        delGLObject(obj);
	}
	else
	{
		objects.push_back(obj);	
	}
}

void releaseGLObjects()
{
	for( int i = 0; i < objects.size(); i++ )
	{
		GLObj o = objects[i];
		delGLObject(o);
	}
	
	objects.clear();
}





/// check GL version
#ifndef CM_GLES
bool areNonPowerOfTwoTexturesSupported() { return gfx::GLExtension("GL_ARB_texture_non_power_of_two"); }
#else
static bool bPowerOf2Tex = true;
 
bool areNonPowerOfTwoTexturesSupported() { return bPowerOf2Tex; }
    
void setPowerOfTwoTexturesSupport( bool flag )
{
	bPowerOf2Tex = flag;
}

#endif

/// From NVIDIA code
bool checkFramebufferStatus()
{

	GLenum status;
	status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status) 
	{
		
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			//debugPrint("GL_FRAMEBUFFER_COMPLETE!");
			return true;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			debugPrint("GL_FRAMEBUFFER_UNSUPPORTED!");
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			debugPrint("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT!");
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			debugPrint("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT!");
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			debugPrint("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT!");
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
			debugPrint("Unknown GL_FRAMEBUFFER error!");
			assert(0);
			return false;
	}

	return false;
}

bool getGLError() 
{
	GLenum err = glGetError();
	//glClearError();
	if( err != GL_NO_ERROR )
	{
		#ifndef CM_GLES
			debugPrint("GL ERROR %d %s",(int)err,gluErrorString(err));
        #endif
		return true;
	}
	
	return false;
	
}

// Got this snippet from LibCinder
bool GLExtension( const std::string &extName )
{
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

GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage)
{
	
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
		debugPrint("VBO creation error: data size mismatch\n");
	}
		
	glBindBuffer(target, 0); 
	
	return glid;  
}

//////////////////////////////////////

void	clear( float r, float g, float b, float a, bool depth, float depthClear )
{
	glClearColor(r,g,b,a);

	GLbitfield cbits = GL_COLOR_BUFFER_BIT;
	if(depth)
	{
		glClearDepth(depthClear);
		cbits |= GL_DEPTH_BUFFER_BIT;
	}
	
	glClear(cbits);
}

void clear( const V4& clr, bool depth, float depthClear )
{
    clear(clr.x, clr.y, clr.z, clr.w, depth, depthClear);
}
    
void	clearDepth( float depthClear )
{
#ifdef CM_GLES
	glClearDepthf(depthClear);
#else
	glClearDepth(depthClear);
#endif
	
	glClear(GL_DEPTH_BUFFER_BIT);
}

void	clearStencil( int stencilClear )
{
	glClearStencil( stencilClear );
	glClear(GL_STENCIL_BUFFER_BIT);
}

void enableColorWrite( bool r, bool g, bool b, bool a )
{
	glColorMask( r,g,b,a );
}

void pushViewport()
{
	glPushAttrib(GL_VIEWPORT_BIT);
}

void popViewport()
{
	glPopAttrib();
}

void	setViewport( int x, int y, int w, int h )
{
	    // Todo handle flip
	glViewport( x, y, w, h );
}

void	setIdentityTransform()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void	enableBlend( bool flag )
{
	if(flag)
		glEnable( GL_BLEND );
	else
		glDisable( GL_BLEND );
}

void	setBlendMode( int mode )
{
	if( mode != BLENDMODE_NONE )
		enableBlend(true);
	else
	{
		enableBlend(false);
		return;
	}
	
	GLenum blendtable[NUMBLENDMODES][2] =
	{
	    { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA }, // straight alpha
	    { GL_ONE, GL_ONE_MINUS_SRC_ALPHA }, // premultiplied alpha
	    { GL_SRC_ALPHA, GL_ONE }, // additive
	    { GL_ZERO, GL_SRC_COLOR }, // multiply
	};

	glBlendFunc( blendtable[mode][0], blendtable[mode][1] );
}

void	setFillMode( int mode )
{
	GLenum	list[]=
	{
		GL_FILL,
		GL_LINE,
		GL_POINT
	};


	glPolygonMode( GL_FRONT_AND_BACK, list[mode] );
}

void	setCullMode( int cull )
{
	if( cull == CULL_NONE )
	{
		glDisable( GL_CULL_FACE );
		return;
	}

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );

	GLenum tab[2] = { GL_CW, GL_CCW };

	int frontFace = (int)cull-1;
	frontFace = !frontFace;

	glFrontFace( tab[frontFace] );
}

void	enableDepthWrite( bool flag )
{

	glDepthMask((GLboolean)flag);
}

void	enableDepthRead( bool flag )
{

	if( flag )
	{
		glEnable( GL_DEPTH_TEST );
	}
	else
	{
		glDisable( GL_DEPTH_TEST );
	}
}

void	enableDepthBuffer( bool flag )
{
	enableDepthWrite(flag);
	enableDepthRead(flag);
}


void	setDepthFunc( int func )
{
	GLenum funcs[] =
	{
		GL_GREATER,
		GL_LESS,
		GL_GEQUAL,
		GL_LEQUAL,
		GL_EQUAL,
		GL_NOTEQUAL,
		GL_ALWAYS,
		GL_NEVER
	};

	glDepthFunc( funcs[func] );
}

void	enableStencilWrite( bool flag )
{

	if( flag )
		glStencilMask(~0);
	else
		glStencilMask(0);
}

void	enableStencilRead( bool flag )
{

	if( flag )
	{
		glEnable( GL_STENCIL_TEST );
	}
	else
	{
		glDisable( GL_STENCIL_TEST );
	}
}

void	enableStencilBuffer( bool flag )
{
	enableStencilWrite(flag);
	enableStencilRead(flag);
}


void	setStencilFunc( int func, int ref, unsigned int mask )
{
	GLenum funcs[] =
	{
		GL_GREATER,
		GL_LESS,
		GL_GEQUAL,
		GL_LEQUAL,
		GL_EQUAL,
		GL_NOTEQUAL,
		GL_ALWAYS,
		GL_NEVER
	};

	glStencilFunc( funcs[func],ref,mask );
}


void	setStencilOp( int fail, int zfail, int zpass )
{

	GLenum stencilOps[] = 
	{
		GL_INCR_WRAP,
		GL_DECR_WRAP,
		GL_KEEP,
		GL_INCR,
		GL_DECR,
		GL_REPLACE,
		GL_ZERO,
		GL_INVERT
	};

	glStencilOp( stencilOps[fail], stencilOps[zfail], stencilOps[zpass] );
}

void	setStencilOp( int failFront, int zfailFront, int zpassFront,
	int failBack, int zfailBack, int zpassBack )
{
	GLenum stencilOps[] = 
	{
		GL_INCR_WRAP,
		GL_DECR_WRAP,
		GL_KEEP,
		GL_INCR,
		GL_DECR,
		GL_REPLACE,
		GL_ZERO,
		GL_INVERT
	};
	glStencilOpSeparate( GL_FRONT, stencilOps[failFront], stencilOps[zfailFront], stencilOps[zpassFront] );
	glStencilOpSeparate( GL_BACK, stencilOps[failBack], stencilOps[zfailBack], stencilOps[zpassBack] );
}


void	setPointSize( float s )
{

	glPointSize(s);
}

void	enablePointSprites( bool flag, int textureIndex )
{
	glActiveTexture(GL_TEXTURE0+textureIndex);

	if(flag)
	{
		glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
		glEnable( GL_POINT_SPRITE_ARB );
	}
	else
	{
		glDisable( GL_POINT_SPRITE_ARB );
	}
}

void setPerspectiveProjection(float fovy, float aspect, float zNear, float zFar)
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

    // This code is based off the MESA source for gluPerspective
    // *NOTE* This assumes GL_PROJECTION is the current matrix   
	float xmin, xmax, ymin, ymax;
	float m[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define M(x,y) m[((y)<<2)+(x)]

	ymax = zNear * tan(fovy * PI / 360.0);
	ymin = -ymax;

	xmin = ymin * aspect;
	xmax = ymax * aspect;

    // Set up the projection matrix
    M(0,0) = (2.0 * zNear) / (xmax - xmin); //proj00 = 
    M(1,1) = (2.0 * zNear) / (ymax - ymin); //proj11 = 
    M(2,2) = -(zFar + zNear) / (zFar - zNear);

    M(0,2) = (xmax + xmin) / (xmax - xmin);
    M(1,2) = (ymax + ymin) / (ymax - ymin);
    M(3,2) = -1.0;

    M(2,3) = -(2.0 * zFar * zNear) / (zFar - zNear);

#undef M

    // Add to current matrix
    glMultMatrixf(m);

    glMatrixMode( GL_MODELVIEW );
}

void setFrustum( float left, float right, float bottom, float top, float near, float far )
{

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum(left,right,bottom,top,near,far);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void setOrtho(float x,float y,float w, float h, float zNear, float zFar)
{

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( x, x+w, y+h, y, zNear, zFar);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void setOrtho( float w, float h )
{
	setOrtho(0,0,w,h);
}

/// Mesa 3d Implementation
void lookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
	GLfloat centerx, GLfloat centery, GLfloat centerz,
	GLfloat upx, GLfloat upy, GLfloat upz)
{


	GLfloat m[16];
	GLfloat x[3], y[3], z[3];
	GLfloat mag;

    /* Make rotation matrix */

    /* Z vector */
	z[0] = eyex - centerx;
	z[1] = eyey - centery;
	z[2] = eyez - centerz;
	mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) 
    {          /* mpichler, 19950515 */
	z[0] /= mag;
	z[1] /= mag;
	z[2] /= mag;
	}

	/* Y vector */
	y[0] = upx;
	y[1] = upy;
	y[2] = upz;

	/* X vector = Y cross Z */
	x[0] = y[1] * z[2] - y[2] * z[1];
	x[1] = -y[0] * z[2] + y[2] * z[0];
	x[2] = y[0] * z[1] - y[1] * z[0];

	/* Recompute Y = Z cross X */
	y[0] = z[1] * x[2] - z[2] * x[1];
	y[1] = -z[0] * x[2] + z[2] * x[0];
	y[2] = z[0] * x[1] - z[1] * x[0];

    /* mpichler, 19950515 */
    /* cross product gives area of parallelogram, which is < 1.0 for
     * non-perpendicular unit-length vectors; so normalize x, y here
     */

     mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
     if (mag) {
     	x[0] /= mag;
     	x[1] /= mag;
     	x[2] /= mag;
     }

     mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
     if (mag) {
     	y[0] /= mag;
     	y[1] /= mag;
     	y[2] /= mag;
     }

#define M(row,col)  m[col*4+row]
     M(0, 0) = x[0];
     M(0, 1) = x[1];
     M(0, 2) = x[2];
     M(0, 3) = 0.0;
     M(1, 0) = y[0];
     M(1, 1) = y[1];
     M(1, 2) = y[2];
     M(1, 3) = 0.0;
     M(2, 0) = z[0];
     M(2, 1) = z[1];
     M(2, 2) = z[2];
     M(2, 3) = 0.0;
     M(3, 0) = 0.0;
     M(3, 1) = 0.0;
     M(3, 2) = 0.0;
     M(3, 3) = 1.0;
#undef M
     glMultMatrixf(m);

    /* Translate Eye to Origin */
     glTranslatef(-eyex, -eyey, -eyez);

 }


/// Sets OF like 2d/3d view
 void set2DView( float w, float h, int rotate )
 {

 	float halfFov, theTan, screenFov, aspect;
 	screenFov 		= 60.0f;

 	if( rotate )
 	{
 		int tmp = w;
 		w = h;
 		h = tmp;
 	}

 	float eyeX 		= (float)w / 2.0;
 	float eyeY 		= (float)h / 2.0;
 	halfFov 		= PI * screenFov / 360.0;
 	theTan 			= tanf(halfFov);
 	float dist 		= eyeY / theTan;
    float nearDist 	= dist / 10.0;	// near / far clip plane
    float farDist 	= dist * 10.0;
    aspect 			= (float)w/(float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspectiveProjection(screenFov, aspect, nearDist, farDist);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    lookAt(eyeX, eyeY, dist, eyeX, eyeY, 0.0, 0.0, 1.0, 0.0);
    glRotatef(rotate*90,0,0,1);
    glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
    glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.

}


void setProjectionMatrix( const M44& m )
{	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixd(m.memptr());
}

void setModelViewMatrix( const arma::mat& m )
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	applyMatrix(m);
}

void applyMatrix( const arma::mat& m_ )
{
	glMatrixMode(GL_MODELVIEW);
	arma::mat m = m_;
	if(m.n_rows==3)
	{
		m.insert_cols(2, 1);
		m.insert_rows(2, 1);		
	}
	glMultMatrixd(m.memptr());
    
#ifdef GFX_TO_EPS
    if( renderingToEps )
    {
        eps.setMatrix(m_);
    }
#endif
}


M44 getProjectionMatrix()
{
	float buffer[16];
	glGetFloatv( GL_PROJECTION_MATRIX, buffer);
    return m44(buffer);
}

M44 getModelViewMatrix()
{
  	float buffer[16];
  	glGetFloatv( GL_MODELVIEW_MATRIX, buffer);
    return m44(buffer);
}

V4 getViewport()
{
    GLint viewpt[4];
    glGetIntegerv(GL_VIEWPORT, (GLint*)viewpt);
    return V4(viewpt[0], viewpt[1], viewpt[2], viewpt[3]);
}
    
GLenum getGLPRIMITIVETYPE( int in )
{

	GLenum	list[]=
	{
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN,

#ifndef CM_GLES
		GL_QUADS,
#endif

		GL_LINES,
		GL_LINE_STRIP,
		GL_LINE_LOOP,
		GL_POINTS
	    /// \todo fix eventual bug with polygons
	};

	return list[in];
}

void	drawUVQuad( float x  , float y  , float w , float h, float maxU, float maxV, bool flip )
{	
	GLfloat tex_coords[8];

	if(flip)
	{
		tex_coords[0] = 0.0f; tex_coords[1] = maxV; 
		tex_coords[2] = 0.0f; tex_coords[3] = 0.0f; 
		tex_coords[4] = maxU; tex_coords[5] =  0.0f; 
		tex_coords[6] = maxU; tex_coords[7] = maxV; 
	}
	else 
	{
		tex_coords[0] = 0.0f; tex_coords[1] = 0.0f; 
		tex_coords[2] = 0.0f; tex_coords[3] = maxV; 
		tex_coords[4] = maxU; tex_coords[5] = maxV; 
		tex_coords[6] = maxU; tex_coords[7] = 0.0f; 
	}

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
}

void	drawQuad( float x  , float y  , float w , float h )
{
	GLfloat verts[] = {
		x+w,y,
		x+w,y+h,
		x,y+h,
		x,y
	};

	glBegin(GL_TRIANGLE_FAN);
	for( int i = 0; i < 8; i+=2 )
	{
		glVertex2f(verts[i],verts[i+1]);
	}
	glEnd();
}


void	drawWireBox( const arma::vec& min, const arma::vec& max )
{
	float verts[8][3] = 
	{	
		{ min[0],  max[1],  max[2] },
		{ min[0],  min[1],  max[2] },
		{ max[0],  min[1],  max[2] },
		{ max[0],  max[1],  max[2] },

		{ min[0],  max[1],  min[2] },
		{ min[0],  min[1],  min[2] },
		{ max[0],  min[1],  min[2] },
		{ max[0],  max[1],  min[2] },
	};

	unsigned short inds[] = 
	{
		0,1, 1,2, 2,3, 3,0,

		4,5, 5,6, 6,7, 7,4,

		0,4, 1,5, 2,6, 3,7
	};

	glVertexPointer(	3,
		GL_FLOAT,
		0,
		verts );
	glEnableClientState(GL_VERTEX_ARRAY); 
	glDrawElements(getGLPRIMITIVETYPE(LINELIST),24,GL_UNSIGNED_SHORT,inds);
	glDisableClientState(GL_VERTEX_ARRAY); 
}


///////////////////////////////////////////////////////////////

void enableAntiAliasing( bool aa )
{	

	if ( aa )	
	{		
		glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );		
		glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );	
		glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );	
		glEnable( GL_POINT_SMOOTH );	
		glEnable( GL_LINE_SMOOTH );	
		glEnable( GL_POLYGON_SMOOTH );	
		glLineWidth( 0.5 );	
	}	else	{		
		glHint( GL_POINT_SMOOTH_HINT, GL_FASTEST );		
		glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST );		
		glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST );				
		glDisable( GL_POINT_SMOOTH );		
		glDisable( GL_LINE_SMOOTH );		
		glDisable( GL_POLYGON_SMOOTH );	
	}
}


void color( float c, float a )
{    
	glColor4f(c,c,c,a);
    currentColor = V4(c,c,c,a);
}

void color( float r, float g, float b, float a )
{
	glColor4f(r,g,b,a);
    currentColor = V4(r,g,b,a);
}

void color( const V4& c )
{
	glColor4f(c.x, c.y, c.z, c.w);
    currentColor = c;
}

void vertex( const arma::vec& v  )
{
	switch(v.size())
	{
		case 2:
		glVertex2f(v[0], v[1]);
		break;
		case 4:
		glVertex4f(v[0], v[1], v[2], v[3]);
		break;
		default:
		glVertex3f(v[0], v[1], v[2]);
		break;
	}

	EPS_ADDP(v)
}

void vertex( const V2& v  )
{
	glVertex3f(v.x,v.y,0);

	EPS_ADDP(v)
}

void vertex( const V3& v  )
{
	glVertex3f(v.x,v.y,v.z);

	EPS_ADDP(v)
}

void vertex( float x, float y, float z  )
{
	glVertex3f(x,y,z);

	EPS_ADDP(V2(x,y))
}

void vertex( float x, float y )
{
	glVertex2f(x,y);

	EPS_ADDP(V2(x,y))
}

void normal( float x, float y, float z )
{
	glNormal3f(x,y,z);
}

void normal( const arma::vec& v )
{
	glNormal3f(v[0], v[1], v[2]);
}

void normal( const V3& v )
{
	glNormal3f(v[0], v[1], v[2]);
}

void uv( float u, float v, int texIndex  )
{
	glMultiTexCoord2f(GL_TEXTURE0+texIndex, u, v);
}

void uv( const V2&v, int texIndex  )
{
	glMultiTexCoord2f(GL_TEXTURE0+texIndex, v.x, v.y);
}

void	beginVertices( int prim )
{
	glBegin(getGLPRIMITIVETYPE(prim));

	#ifdef GFX_TO_EPS
	if( renderingToEps )
	{
        if( prim == gfx::LINESTRIP ||
            prim == gfx::LINELOOP )
        {
            currentEpsPath.clear();
            drawingEpsPath = true;
        }
	}
	#endif
}


void	endVertices()
{
	glEnd();
    glActiveTexture(GL_TEXTURE0+0);

    #ifdef GFX_TO_EPS
	if( renderingToEps && drawingEpsPath )
	{
		if( currentPrimitive == gfx::LINELOOP )
			currentEpsPath.close();

		eps.strokeShape(currentEpsPath, currentColor);
		currentEpsPath.clear();
		drawingEpsPath = false;
	}
	#endif
}

void draw( const Contour& C )
{
	draw(C.points, C.closed);
}

void draw( const Contour& C, int from, int to )
{
    draw(C.points, C.closed, from, to);
}

void draw( const Shape& S )
{
	for( int i = 0; i < S.size(); i++ )
		draw(S[i]);
}

void draw( const arma::mat& P, bool closed, int from, int to )
{
	if(!P.size())
		return;

    if(from==-1)
        from = 0;
    if(to==-1)
        to = P.n_cols;
    
    if(from == to)
        return;
    
	if(closed)
		gfx::beginVertices(LINELOOP);
	else
		gfx::beginVertices(LINESTRIP);
    switch(P.n_rows) //P[0].size())
	{
		case 2:
		{
			for( int i = from; i < to; i++ )
			{
			    glVertex2f(P.at(0,i), P.at(1,i)); //  )P[i][0], P[i][1] );
 				EPS_ADDP(V2(P.at(0,i), P.at(1,i)));
			}
			break;
		}
		default:
		{
			for( int i = from; i < to; i++ )
			{
				glVertex3f(P.at(0, i), P.at(1, i), P.at(2, i));//P[i][0], P[i][1], P[i][2] );
 				EPS_ADDP(V2(P.at(0, i), P.at(1, i)));
			}
		break;
		}
	}

	gfx::endVertices();
    
#ifdef GFX_TO_EPS
    if(renderingToEps)
    {
        eps.strokeShape(Contour(P, closed), currentColor);
    }
#endif
    
}

void drawPrimitives( const std::vector<V3> & P, int prim, int offset, int inc )
{
	if(!P.size())
		return;

	gfx::beginVertices(prim);

	for( int i = offset; i < P.size(); i+=inc )
	{
		glVertex3f(P[i].x, P[i].y, P[i].z );
	}

	gfx::endVertices();
}


void drawPrimitives( const std::vector<V2> & P, int prim, int offset, int inc )
{
	if(!P.size())
		return;

	gfx::beginVertices(prim);

	for( int i = offset; i < P.size(); i+=inc )
	{
		glVertex2f(P[i].x, P[i].y );
	}

	gfx::endVertices();
}


void drawPrimitives( const arma::mat & P, int prim, int offset, int inc )
{
	if(!P.size())
		return;

	gfx::beginVertices(prim);
	switch(P.n_rows)
	{
		case 2:
		{
			for( int i = offset; i < P.n_cols; i+=inc )
			{
				glVertex2f(P(0,i), P(1,i) );
			}
			break;
		}
		default:
		{
			for( int i = offset; i < P.n_cols; i+=inc )
			{
				glVertex3f(P(0,i), P(1,i), P(2,i) );
			}
			break;
		}
	}

	gfx::endVertices();
}

void draw( const Mesh& mesh )
{
	glVertexPointer( 3, GL_FLOAT, 0,  &(mesh.vertices[0]) );
    
    if(mesh.normals.size())
    {
        glNormalPointer( GL_FLOAT, 0, &(mesh.normals[0]) );
        glEnableClientState( GL_NORMAL_ARRAY );
    }

    if(mesh.colors.size())
    {
        glColorPointer( 4, GL_FLOAT, sizeof(float4), &(mesh.colors[0]) );
        glEnableClientState( GL_COLOR_ARRAY );
    }

    if(mesh.uvs.size())
    {
        glTexCoordPointer( 2, GL_FLOAT, sizeof(float2), &(mesh.uvs[0]) );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    }

	glEnableClientState( GL_VERTEX_ARRAY );
	if(mesh.numIndices())
	{

		glDrawElements( getGLPRIMITIVETYPE( mesh.primitive ),
			mesh.numIndices(), GL_UNSIGNED_INT, &(mesh.indices[0]) );
	}
	else
	{
		glDrawArrays( getGLPRIMITIVETYPE( mesh.primitive ),
			0,
			mesh.numVertices() );
	}
    
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}


void fill( const Contour& shape, int winding )
{
	Tessellator tess = Tessellator(shape, winding);
	gfx::draw( tess.mesh );
#ifdef GFX_TO_EPS
    if(renderingToEps)
    {
        eps.fillShape(shape, currentColor);
    }
#endif
    
}

	
void fill( const Shape& shape, int winding )
{
	Tessellator tess = Tessellator(shape, winding);
	gfx::draw( tess.mesh );
#ifdef GFX_TO_EPS
    if(renderingToEps)
    {
        eps.fillShape(shape, currentColor);
    }
#endif
}

/// Draw an image
void image( Image& img, float x, float y, float w, float h )
{
	img.draw(x, y, w, h);
}

void image( const arma::uchar_cube& img, float x, float y, float w, float h )
{
	Image im(img);
	im.draw(x,y,w,h);
}

void identity()
{
	glLoadIdentity();
}

void translate( const V2& v )
{
	translate(v.x, v.y, 0);
}

void translate( const V3& v )
{
	translate(v.x, v.y, v.z);
}

void translate( const arma::vec& v )
{
	if(v.size()==2)
		translate(v[0], v[1]);
	else
		translate(v[0], v[1], v[2]);
}

void	translate( float x, float y , float z  )
{
	glTranslatef(x,y,z);
	#ifdef GFX_TO_EPS
	if( renderingToEps )
	{
		eps.translate(x, y);
	}
	#endif
}

void	rotate( float x, float y, float z  )
{
	glRotatef( z,0,0,1 );
	glRotatef( x,1,0,0 );
	glRotatef( y,0,1,0 );
}

void	rotate( float z )
{
	glRotatef(z,0,0,1);

	#ifdef GFX_TO_EPS
	if( renderingToEps )
		eps.rotate(z);
	#endif
}

void	scale( const V3& s  )
{
	glScalef(s.x, s.y, s.z);

	#ifdef GFX_TO_EPS
	if ( renderingToEps )
		eps.scale(s.x, s.y);
	#endif
}

void	scale( const V2& s  )
{
	glScalef(s.x, s.y, 1);

	#ifdef GFX_TO_EPS
	if ( renderingToEps )
		eps.scale(s.x, s.y);
	#endif
}

void	scale( float x, float y, float z  )
{
	glScalef(x,y,z);

	#ifdef GFX_TO_EPS
	if ( renderingToEps )
		eps.scale(x, y);
	#endif
}

void	scale( float s )
{
	glScalef(s,s,s);

	#ifdef GFX_TO_EPS
	if ( renderingToEps )
		eps.scale(s, s);
	#endif
}

void pushMatrix( const M44& m )
{
	pushMatrix();
	applyMatrix(m);
}

void pushMatrix( const M33& m )
{
	pushMatrix();
	applyMatrix(m);
}

void pushMatrix( const arma::mat& m )
{
	pushMatrix();
	applyMatrix(m);
}

void	pushMatrix()
{
	glPushMatrix();

	#ifdef GFX_TO_EPS
	if ( renderingToEps )
	{
		//eps.gsave();
        eps.pushMatrix();
	}
	#endif
}

void	popMatrix()
{
	glPopMatrix();

	#ifdef GFX_TO_EPS
	if ( renderingToEps )
	{
		//eps.grestore();
        eps.popMatrix();
	}
	#endif
}

void drawLine(  const arma::vec&  a,  const arma::vec&  b )
{
	beginVertices(LINELIST);
	vertex(a);
	vertex(b);
	endVertices();
}

void drawLine( float x0, float y0, float x1, float y1)
{
	beginVertices(LINELIST);
	vertex(x0,y0);
	vertex(x1,y1);
	endVertices();
}


void drawArrow( const V2&a, const V2&b, float size )
{
	#ifdef GFX_TO_EPS
		bool tmpEps = renderingToEps;
		renderingToEps = false;
	#endif
	gfx::drawLine(a,b);
	arma::vec d = b-a;
	d = normalise(d);

	arma::vec perp({-d[1], d[0]});
	d*=size;
	perp*=size;
	gfx::drawLine(b-d-perp,b);
	gfx::drawLine(b-d+perp,b);

	#ifdef GFX_TO_EPS
	renderingToEps = tmpEps;
	
	if(renderingToEps)
    	eps.drawArrow(a, b, size, currentColor);
#endif
}

/////////////////////////////////////// *** ADAPTED FROM LIBCINDER

void fillCircle( const V2& center, float radius, int numSegments )
{
	    // automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)floor( radius * PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new float[(numSegments+2)*2];
	verts[0] = center[0];
	verts[1] = center[1];
	for( int s = 0; s <= numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[(s+1)*2+0] = center[0] + cos( t ) * radius;
		verts[(s+1)*2+1] = center[1] + sin( t ) * radius;
	}
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
	glDisableClientState( GL_VERTEX_ARRAY );
	delete [] verts;

#ifdef GFX_TO_EPS
	if(renderingToEps)
		eps.fillCircle(center, radius, currentColor);
#endif
}


void drawCircle( const V2&center, float radius, int numSegments )
{
	    // automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)floorf( radius * PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new float[numSegments*2];
	for( int s = 0; s < numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[s*2+0] = center[0] + cos( t ) * radius;
		verts[s*2+1] = center[1] + sin( t ) * radius;
	}
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glDrawArrays( GL_LINE_LOOP, 0, numSegments );
	glDisableClientState( GL_VERTEX_ARRAY );
	delete [] verts;

#ifdef GFX_TO_EPS
	if(renderingToEps)
		eps.strokeCircle(center, radius, currentColor);
#endif

}


void drawTriangle( const V2& a, const V2& b, const V2& c )
{
	beginVertices(LINELOOP);
	vertex(a);
	vertex(b);
	vertex(c);
	endVertices();
}


void fillTriangle( const V2& a, const V2& b, const V2& c )
{
	beginVertices(TRIANGLELIST);
	vertex(a);
	vertex(b);
	vertex(c);
	endVertices();
}
	
// Adapted from LibCinder
void drawCubeImpl( const V3&c, const V3&size, bool drawColors )
{
	GLfloat sx = size[0] * 0.5f;
	GLfloat sy = size[1] * 0.5f;
	GLfloat sz = size[2] * 0.5f;
    GLfloat vertices[24*3]={c[0]+1.0f*sx,c[1]+1.0f*sy,c[2]+1.0f*sz,	c[0]+1.0f*sx,c[1]+-1.0f*sy,c[2]+1.0f*sz,	c[0]+1.0f*sx,c[1]+-1.0f*sy,c[2]+-1.0f*sz,	c[0]+1.0f*sx,c[1]+1.0f*sy,c[2]+-1.0f*sz,		// +X
	    c[0]+1.0f*sx,c[1]+1.0f*sy,c[2]+1.0f*sz,	c[0]+1.0f*sx,c[1]+1.0f*sy,c[2]+-1.0f*sz,	c[0]+-1.0f*sx,c[1]+1.0f*sy,c[2]+-1.0f*sz,	c[0]+-1.0f*sx,c[1]+1.0f*sy,c[2]+1.0f*sz,		// +Y
	    c[0]+1.0f*sx,c[1]+1.0f*sy,c[2]+1.0f*sz,	c[0]+-1.0f*sx,c[1]+1.0f*sy,c[2]+1.0f*sz,	c[0]+-1.0f*sx,c[1]+-1.0f*sy,c[2]+1.0f*sz,	c[0]+1.0f*sx,c[1]+-1.0f*sy,c[2]+1.0f*sz,		// +Z
	    c[0]+-1.0f*sx,c[1]+1.0f*sy,c[2]+1.0f*sz,	c[0]+-1.0f*sx,c[1]+1.0f*sy,c[2]+-1.0f*sz,	c[0]+-1.0f*sx,c[1]+-1.0f*sy,c[2]+-1.0f*sz,	c[0]+-1.0f*sx,c[1]+-1.0f*sy,c[2]+1.0f*sz,	// -X
	    c[0]+-1.0f*sx,c[1]+-1.0f*sy,c[2]+-1.0f*sz,	c[0]+1.0f*sx,c[1]+-1.0f*sy,c[2]+-1.0f*sz,	c[0]+1.0f*sx,c[1]+-1.0f*sy,c[2]+1.0f*sz,	c[0]+-1.0f*sx,c[1]+-1.0f*sy,c[2]+1.0f*sz,	// -Y
	    c[0]+1.0f*sx,c[1]+-1.0f*sy,c[2]+-1.0f*sz,	c[0]+-1.0f*sx,c[1]+-1.0f*sy,c[2]+-1.0f*sz,	c[0]+-1.0f*sx,c[1]+1.0f*sy,c[2]+-1.0f*sz,	c[0]+1.0f*sx,c[1]+1.0f*sy,c[2]+-1.0f*sz};	// -Z


	GLfloat normals[24*3]={ 1,0,0,	1,0,0,	1,0,0,	1,0,0,
		0,1,0,	0,1,0,	0,1,0,	0,1,0,
		0,0,1,	0,0,1,	0,0,1,	0,0,1,
		-1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
		0,-1,0,	0,-1,0,  0,-1,0,0,-1,0,
		0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1};

	GLubyte colors[24*4]={	255,0,0,255,	255,0,0,255,	255,0,0,255,	255,0,0,255,	// +X = red
		0,255,0,255,	0,255,0,255,	0,255,0,255,	0,255,0,255,	// +Y = green
		0,0,255,255,	0,0,255,255,	0,0,255,255,	0,0,255,255,	// +Z = blue
		0,255,255,255,	0,255,255,255,	0,255,255,255,	0,255,255,255,	// -X = cyan
		255,0,255,255,	255,0,255,255,	255,0,255,255,	255,0,255,255,	// -Y = purple
		255,255,0,255,	255,255,0,255,	255,255,0,255,	255,255,0,255 };// -Z = yellow

	GLfloat texs[24*2]={	0,1,	1,1,	1,0,	0,0,
		1,1,	1,0,	0,0,	0,1,
		0,1,	1,1,	1,0,	0,0,							
		1,1,	1,0,	0,0,	0,1,
		1,0,	0,0,	0,1,	1,1,
		1,0,	0,0,	0,1,	1,1 };

		GLubyte elements[6*6] ={	0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9,10, 8, 10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23 };

	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer( GL_FLOAT, 0, normals );

	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, texs );

	if( drawColors ) {
		glEnableClientState( GL_COLOR_ARRAY );	
		glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colors );		
	}

	glEnableClientState( GL_VERTEX_ARRAY );	 
	glVertexPointer( 3, GL_FLOAT, 0, vertices );

	glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, elements );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	 
	glDisableClientState( GL_NORMAL_ARRAY );
	if( drawColors )
		glDisableClientState( GL_COLOR_ARRAY );

}


void drawCube( const V3& center, const V3& size )
{
	drawCubeImpl( center, size, false );
}


void drawColorCube( const V3& center, const V3& size )
{
	drawCubeImpl( center, size, true );
}


void drawWireCube( const V3& center, const V3& size )
{
	V3 min = center - size * 0.5f;
	V3 max = center + size * 0.5f;

	gfx::drawLine(  V3(min[0], min[1], min[2]),  V3(max[0], min[1], min[2]) );
	gfx::drawLine(  V3(max[0], min[1], min[2]),  V3(max[0], max[1], min[2]) );
	gfx::drawLine(  V3(max[0], max[1], min[2]),  V3(min[0], max[1], min[2]) );
	gfx::drawLine(  V3(min[0], max[1], min[2]),  V3(min[0], min[1], min[2]) );
	gfx::drawLine(  V3(min[0], min[1], max[2]),  V3(max[0], min[1], max[2]) );
	gfx::drawLine(  V3(max[0], min[1], max[2]),  V3(max[0], max[1], max[2]) );
	gfx::drawLine(  V3(max[0], max[1], max[2]),  V3(min[0], max[1], max[2]) );
	gfx::drawLine(  V3(min[0], max[1], max[2]),  V3(min[0], min[1], max[2]) );
	gfx::drawLine(  V3(min[0], min[1], min[2]),  V3(min[0], min[1], max[2]) );
	gfx::drawLine(  V3(min[0], max[1], min[2]),  V3(min[0], max[1], max[2]) );
	gfx::drawLine(  V3(max[0], max[1], min[2]),  V3(max[0], max[1], max[2]) );
	gfx::drawLine(  V3(max[0], min[1], min[2]),  V3(max[0], min[1], max[2]) );
}

// http://local.wasp.uwa.edu.au/~pbourke/texture_colour/spheremap/  Paul Bourke's sphere code
// We should weigh an alternative that reduces the batch count by using GL_TRIANGLES instead

void drawSphere( const V3& center, float radius, int segments )
{
	if( segments < 0 )
		return;

	float *verts = new float[(segments+1)*2*3];
	float *normals = new float[(segments+1)*2*3];
	float *texCoords = new float[(segments+1)*2*2];

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, verts );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer( GL_FLOAT, 0, normals );

	for( int j = 0; j < segments / 2; j++ ) {
		float theta1 = j * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
		float theta2 = (j + 1) * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );

		for( int i = 0; i <= segments; i++ ) {
            arma::vec e(3);
			arma::vec p(3);

			float theta3 = i * 2 * 3.14159f / segments;

			e[0] = cos( theta1 ) * cos( theta3 );
			e[1] = sin( theta1 );
			e[2] = cos( theta1 ) * sin( theta3 );
			p = e * radius + (arma::vec)center;
			normals[i*3*2+0] = e[0]; normals[i*3*2+1] = e[1]; normals[i*3*2+2] = e[2];
			texCoords[i*2*2+0] = 0.999f - i / (float)segments; texCoords[i*2*2+1] = 0.999f - 2 * j / (float)segments;
			verts[i*3*2+0] = p[0]; verts[i*3*2+1] = p[1]; verts[i*3*2+2] = p[2];

			e[0] = cos( theta2 ) * cos( theta3 );
			e[1] = sin( theta2 );
			e[2] = cos( theta2 ) * sin( theta3 );
			p = e * radius + (arma::vec)center;
			normals[i*3*2+3] = e[0]; normals[i*3*2+4] = e[1]; normals[i*3*2+5] = e[2];
			texCoords[i*2*2+2] = 0.999f - i / (float)segments; texCoords[i*2*2+3] = 0.999f - 2 * ( j + 1 ) / (float)segments;
			verts[i*3*2+3] = p[0]; verts[i*3*2+4] = p[1]; verts[i*3*2+5] = p[2];
		}
		glDrawArrays( GL_TRIANGLE_STRIP, 0, (segments + 1)*2 );
	}

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );

	delete [] verts;
	delete [] normals;
	delete [] texCoords;
}

void lineWidth( float w )
{
	glLineWidth(w);
}

void lineStipple( int factor, unsigned short pattern )
{
	if(factor == 0)
	{
		glDisable(GL_LINE_STIPPLE);
	}
	else
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(factor, pattern);
	}
}


void drawXMarker( const V2& pos, float size )
{
	V2 d1(-size,-size);
	V2 d2(-size,size);
	gfx::drawLine( pos+d1, pos-d1 );
	gfx::drawLine( pos+d2, pos-d2 );
}


void drawCone( double r, double h, int n )
{
	double x,y,z;

	arma::vec T = arma::linspace(0 + PI/4,TWOPI + PI/4,n+1);
	gfx::beginVertices(gfx::TRIANGLEFAN);
	gfx::vertex(0,0,0);
	for( int i = 0; i < T.size(); i++ )
	{
		double t = T[i];
		x = cos(t)*r;
		y = 0;
		z = sin(t)*r;
		gfx::vertex(x,y,z);
	}
	gfx::endVertices();

	beginVertices(gfx::TRIANGLEFAN);
	gfx::vertex(0,h,0);
	for( int i = 0; i < T.size(); i++ )
	{
		double t = T[i];
		x = cos(t)*r;
		y = 0;
		z = sin(t)*r;
		gfx::vertex(x,y,z);
	}
	gfx::endVertices();
}


///////////////////////////////////////////////////////////////
void drawAxis( const M44& m, float scale  )
{
    V3 pos = trans44(m);
    V3 x = pos + x44(m)*scale;
	V3 y = pos + y44(m)*scale;
	V3 z = pos + z44(m)*scale;

	beginVertices(LINELIST);
	color(1,0,0,1);
	vertex(pos);
	vertex(x);
	color(0,1,0,1);
	vertex(pos);
	vertex(y);	
	color(0,0,1,1);
	vertex(pos);
	vertex(z);	
	color(1,1,1,1);
	endVertices();

// draw arrows..
	double coner = scale*0.07;
	double coneh = scale*0.3;
	double conen = 9;

	gfx::pushMatrix(m);

	gfx::pushMatrix();
	glTranslatef(scale,0,0);
	glRotatef(-90,0,0,1);
	gfx::color(1,0,0,1);
	gfx::drawCone(coner, coneh, conen);
	gfx::popMatrix();

	gfx::pushMatrix();
	glTranslatef(0,scale,0);
	gfx::color(0,1,0);
	gfx::drawCone(coner, coneh, conen);
	gfx::popMatrix();

	gfx::pushMatrix();
	glTranslatef(0,0,scale);
	glRotatef(90,1,0,0);
	gfx::color(0,0,1);
	gfx::drawCone(coner, coneh, conen);
	gfx::popMatrix();

	gfx::popMatrix();

}

///////////////////////////////////////////////////////////////
void drawBox( const Box& box )
{
	if(box.dimension() == 2)
		drawRect(box);
	else
		drawWireBox(box.min(), box.max());
}

void drawRect( const Box& rect )
{
	drawRect(rect.l(), rect.t(), rect.width(), rect.height() );
}

void fillRect( const Box& rect )
{
	fillRect(rect.l(), rect.t(), rect.width(), rect.height() );
}

void drawRect( float x, float y, float w, float h )
{
	float pts[8] = 
	{
		x,y,
		x,y+h,
		x+w,y+h,
		x+w,y
	};

	glEnableClientState(GL_VERTEX_ARRAY);		
	glVertexPointer(2, GL_FLOAT, 0,  &pts[0] );
	glDrawArrays( GL_LINE_LOOP, 0, 4 );
	glDisableClientState( GL_VERTEX_ARRAY );
    
    
#ifdef GFX_TO_EPS
    if(renderingToEps)
    {
        eps.strokeShape(Contour(Box(x,y,w,h).corners(),true), currentColor);
    }
#endif
    
}

void fillRect( float x, float y, float w, float h )
{
	float pts[8] = 
	{
		x,y,
		x,y+h,
		x+w,y+h,
		x+w,y
	};

	glEnableClientState(GL_VERTEX_ARRAY);		
	glVertexPointer(2, GL_FLOAT, 0,  &pts[0] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glDisableClientState( GL_VERTEX_ARRAY );
    
#ifdef GFX_TO_EPS
    if(renderingToEps)
    {
        eps.fillShape(Contour(Box(x,y,w,h).corners(),true), currentColor);
    }
#endif

}

void drawFrustum( const M44& proj_, const M44& invView_ )
{
	const double * proj = proj_.memptr();
	const double * invView = invView_.memptr();

	// Get near and far from the Projection matrix.
	const float near = proj[14] / (proj[10] - 1.0);
	const float far = proj[14] / (1.0 + proj[10]);
 
	// Get the sides of the near plane.
	const float nLeft = near * (proj[8] - 1.0) / proj[0];
	const float nRight = near * (1.0 + proj[8]) / proj[0];
	const float nTop = near * (1.0 + proj[9]) / proj[5];
	const float nBottom = near * (proj[9] - 1.0) / proj[5];
 
	// Get the sides of the far plane.
	const float fLeft = far * (proj[8] - 1.0) / proj[0];
	const float fRight = far * (1.0 + proj[8]) / proj[0];
	const float fTop = far * (1.0 + proj[9]) / proj[5];
	const float fBottom = far * (proj[9] - 1.0) / proj[5];
 
	pushMatrix();
	glMultMatrixd( invView );

	glBegin(GL_LINES);
	
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(fLeft, fBottom, -far);
 
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(fRight, fBottom, -far);
 
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(fRight, fTop, -far);
 
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(fLeft, fTop, -far);
 
	//far
	glVertex3f(fLeft, fBottom, -far);
	glVertex3f(fRight, fBottom, -far);
 
	glVertex3f(fRight, fTop, -far);
	glVertex3f(fLeft, fTop, -far);
 
	glVertex3f(fRight, fTop, -far);
	glVertex3f(fRight, fBottom, -far);
 
	glVertex3f(fLeft, fTop, -far);
	glVertex3f(fLeft, fBottom, -far);
 
	//near
	glVertex3f(nLeft, nBottom, -near);
	glVertex3f(nRight, nBottom, -near);
 
	glVertex3f(nRight, nTop, -near);
	glVertex3f(nLeft, nTop, -near);
 
	glVertex3f(nLeft, nTop, -near);
	glVertex3f(nLeft, nBottom, -near);
 
	glVertex3f(nRight, nTop, -near);
	glVertex3f(nRight, nBottom, -near);
 
	glEnd();
	
	popMatrix();
}


void bindTexture( int id, int sampler )
{
	activeTexture(sampler);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id);
}

void unbindTexture( int sampler )
{
	activeTexture(sampler);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void activeTexture( int sampler )
{
	//assert(0);
	// not working...
//glActiveTexture( GL_TEXTURE0+sampler );
}


void drawGauss2d( const arma::vec& mu, const arma::mat& Sigma, const arma::vec& clr, float stdDevScale,  int subd)
{
    arma::mat V;
    arma::vec d;
    
    arma::eig_sym(d, V, Sigma);
    
    arma::mat D = arma::diagmat(sqrt(d)*stdDevScale);
    arma::mat U = V*D;
    
    arma::mat tm = arma::eye(3,3);
    
    tm.submat(0,0,1,1) = U;
    trans33(tm,mu);
    
    // draw axes
    gfx::color(1,0,0);
    gfx::drawLine(mu, mu+U.col(0));
    gfx::color(0,1,0);
    gfx::drawLine(mu, mu+U.col(1));
    
    gfx::pushMatrix(tm);
    gfx::color(clr[0], clr[1], clr[2], clr[3]);
    gfx::fillCircle(arma::vec({0,0}), 1, subd);
    gfx::color(clr[0], clr[1], clr[2], 1.);
    gfx::drawCircle(arma::vec({0,0}), 1, subd);
    gfx::popMatrix();
}

void drawGauss3d( const arma::vec& mu, const arma::mat& Sigma, const arma::vec& clr, float stdDevScale, bool wireFrame, int subd)
{
    arma::mat V;
    arma::vec d;
    
    arma::eig_sym(d, V, Sigma);
    
    arma::mat D = arma::diagmat(sqrt(d)*stdDevScale);
    arma::mat U = V*D;
    
    arma::mat tm = arma::eye(4,4);
    tm.submat(0,0,2,2) = U;
    trans44(tm,mu);
    
    // draw axes
    gfx::color(1,0,0);
    gfx::drawLine(mu, mu+U.col(0));
    gfx::color(0,1,0);
    gfx::drawLine(mu, mu+U.col(1));
    gfx::color(0,0,1);
    gfx::drawLine(mu, mu+U.col(2));
    
    if(wireFrame)
        gfx::setFillMode(gfx::FILL_WIRE);
    
    gfx::pushMatrix(tm);
    gfx::color(clr[0], clr[1], clr[2], clr[3]);
    gfx::drawSphere(arma::vec({0,0,0}), 1, subd);
    gfx::popMatrix();
    
    if(wireFrame)
        gfx::setFillMode(gfx::FILL_SOLID);
}


}

////////////////////////////////////////////////////////////////
// Rect impl

void Box::setCenter( const arma::vec& cenp )
{
    arma::vec o = cenp - center();
    minmax.col(0) += o; minmax.col(1) += o;
}
    
void Box::includeAt( int index, const arma::vec& p )
{
	if(index == 0)
	{
		minmax.col(0) = p;
		minmax.col(1) = p;
	}
	else
	{
		minmax.col(0) = arma::min(minmax.col(0), p);
		minmax.col(1) = arma::max(minmax.col(1), p);
	}
}

arma::mat Box::corners() const
{
	arma::mat C = arma::zeros(2,4);

	C.col(0) = minmax.col(0);
	C.col(1) = arma::vec( { r(), t() } );
	C.col(2) = minmax.col(1);
	C.col(3) = arma::vec( { l(), b() } );

	return C;
}

////////////////////////////////////////////////////////////////
// Contour Impl

void Contour::addPoint( const V2& pt )
{
	addPoint((arma::vec)pt);
}

void Contour::addPoint( const V3& pt )
{
	addPoint((arma::vec)pt);
}

void Contour::addPoint( const arma::vec& pt )
{
	if(empty())
	{
		points = arma::zeros(pt.size(), 1);
		points.col(0) = pt;
	}
	else
	{
		points.insert_cols( points.n_cols, pt );
	}
}

void Contour::addPoint( double x, double y )
{
	addPoint( arma::vec({x, y}) );
}

void Contour::save( const std::string & path, arma::file_type type, bool savePointsAsRows )
{
	arma::mat pts = points;
	if(savePointsAsRows)
		inplace_trans(pts);
	pts.save(path, type);
}


void Contour::load( const std::string & path, arma::file_type type, bool savePointsAsRows )
{	
	points.load(path, type);
	if(savePointsAsRows)
		inplace_trans(points);
}	

// adapted from LibCinder (http://libcinder.org/) code.

arma::vec Contour::interpolate( double t ) const
{
	if(!size())
		return arma::zeros(2,1);

	if( t <= 0 )
		return points.col(0);
	else if( t >= 1 )
		return last();

	int totalSegments =  size()-1;
	float segParamLength = 1.0f / totalSegments;
	size_t seg = t * totalSegments;

	float t1 = 1 - t;
	return points.col(seg)*t1 + points.col(seg+1)*t;
}


double Contour::length( int a, int b ) const
{
	double l = 0.0;
	if( b == -1 )
		b = size();
	for( int i = a; i < b-1; i++ )
	{
		l += norm( points.col(i+1) - points.col(i) );
	}
	return l;
}


Box Contour::boundingBox() const
{
	Box box;
	for( int i = 0; i < points.n_cols; i++ )
		box.includeAt( i, points.col(i) );
	return box;
}


void Contour::addDimension()
{
	arma::Row<double> r = arma::zeros<arma::Row<double>>( points.n_cols );
	points.insert_rows(points.n_rows, r);
}


void Contour::transform( const arma::mat& m )
{
	bool is3d = m.n_rows == 4;

	arma::mat ph = points;

// convert to 3d if necessary
	if( dimension() == 2 && is3d )		
		ph.insert_rows( ph.n_rows, arma::zeros(1, ph.n_cols) );

// make coords homogeneous
	ph.insert_rows( ph.n_rows, arma::ones(1, ph.n_cols) );

// transform
	ph = m * ph;

// back to cartesian
	points = ph.rows(0, ph.n_rows-2);
}


Contour Contour::transformed( const arma::mat& m ) const
{
	Contour res = *this;
	res.transform(m);
	return res;
}


////////////////////////////////////////////////////////////////
// Shape impl


Box Shape::boundingBox() const
{
	Box r(0,0,0,0);
	int c = 0;
	for( int i = 0; i < size(); i++ )
	{
		const Contour& ctr = contours[i];
		for( int j = 0; j < ctr.size(); j++ )
		{
			r.includeAt(c++, ctr[j] );
		}
	}

	return r;
}



arma::vec  Shape::centroid() const
{
	arma::vec p({0,0});

	int c = 0;
	for( int i = 0; i < size(); i++ )
	{
		const Contour& ctr = contours[i];
		for( int j = 0; j < ctr.size(); j++ )
		{
			p += ctr[j];
			c++;
		}
	}

	p /= c;

	return p;
}


void Shape::transform( const arma::mat& m )
{
	for( int i = 0; i < size(); i++ )
		contours[i].transform(m);
}


Shape Shape::transformed( const arma::mat& m ) const
{
	Shape res = *this;
	res.transform(m);
	return res;
}

void Shape::loadSvg( const std::string & f, int subd )
{
	struct NSVGimage* image;
	image = nsvgParseFromFile(f.c_str(), "px", 72);
	
	clear();

	// Use...
	for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next) 
	{
	    for (NSVGpath* path = shape->paths; path != NULL; path = path->next) 
	    {
	    	Contour ctr;
	        for (int i = 0; i < path->npts; i += 3) 
	        {
	            float* p = &path->pts[i*2];
	            ctr.addPoint(p[0],p[1]);
	        }

	        add(ctr);
	    }
	}
	// Delete
	nsvgDelete(image);
}
    
    
#define MAKERGBA(r,g,b,a) ((unsigned int)((((unsigned char)(a*255.0f)&0xff)<<24)|(((unsigned char)(r*255.0f)&0xff)<<16)|(((unsigned char)(g*255.0f)&0xff)<<8)|((unsigned char)(b*255.0f)&0xff)))


/// 32 bit color (RGBA)
void	pixelA8R8G8B8 ( void * out, int x, int y, int z, int pitch, int slicepitch, float r, float g, float b, float a )
{
	unsigned int * dst = (unsigned int  *)((unsigned char*)out + z*slicepitch + y*pitch + x * 4);
	*dst = MAKERGBA(r,g,b,a);//(int32)clr;
}

/// 8 bit color (grayscale) just reads r component
void	pixelL8 ( void * out, int x, int y,int z, int pitch, int slicepitch, float r, float g, float b, float a )
{
	unsigned char *dst = (unsigned char *)out + z*slicepitch + y*pitch + x;
	*dst = r*255;
}

/// 4 float component color
void	pixelA32B32G32R32F( void * out, int x, int y,int z,  int pitch, int slicepitch, float r, float g, float b, float a )
{
	float *dst = (float *)((unsigned char*)out + z*slicepitch + y*pitch + x * (sizeof(float)*4));
	*dst++ = r;
	*dst++ = g;
	*dst++ = b;
	*dst++ = a;
}

/// 4 half float component color
void	pixelA16B16G16R16F( void * out, int x, int y, int z, int pitch, int slicepitch, float r, float g, float b, float a )
{
	//slow!
	// \todo PLATFORM SPECIFIC!!!!!
/*
	D3DXFLOAT16 r = D3DXFLOAT16(clr.r);
	D3DXFLOAT16 g = D3DXFLOAT16(clr.g);
	D3DXFLOAT16 b = D3DXFLOAT16(clr.b);
	D3DXFLOAT16 a = D3DXFLOAT16(clr.a);

	D3DXFLOAT16 *dst = (D3DXFLOAT16 *)((uchar*)out  + z*slicepitch+ y*pitch + x * (sizeof(D3DXFLOAT16)*4));
	*dst++ = r;
	*dst++ = g;
	*dst++ = b;
	*dst++ = a;*/
	
	/// \todo IMPLEMENT float16 !!!!
}


/// set float pixel, only r component
void	pixelR32F ( void * out, int x, int y, int z, int pitch, int slicepitch, float r, float g, float b, float a )
{
	float	* dst = (float *)((unsigned char*)out + z*slicepitch + y*pitch + x * sizeof(float));
	*dst = r;
}

/// set 24 bit pixel (RGB)
void	pixelR8G8B8 ( void * out, int x, int y,int z,  int pitch, int slicepitch, float r, float g, float b, float a )

{
	unsigned char *dst = (unsigned char *)out + z*slicepitch + y*pitch + x * 3;
	*dst++ = b*255;
	*dst++ = g*255;
	*dst++ = r*255;
}

#define ASSERTBOUND		assert( _boundSampler != -1 );
    
struct GLPixelFormatDesc
{
    GLint	glFormat;
    GLenum	glDataFormat;
    GLenum	glDataType;
    int		bytesPerPixel;
};

///////////////////////////////////////////////////////////////////////

static GLPixelFormatDesc glFormatDescs[Texture::NUM_SUPPORTED_FORMATS] =
{
#ifndef CM_GLES
    { GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE,3 }, //R8G8B8 = 0,
    { GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE,4 }, //A8R8G8B8, //GL_BGRA
    { GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE,1 }, //L8,
    
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
    { GL_RGB, GL_BGR, GL_UNSIGNED_BYTE,3 }, //R8G8B8 = 0,
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

static Texture	**currentSamplers = 0;
static GLint		maxTextureUnits = 0;
static int		textureCount = 0;


	
static void swizzleRGB( unsigned char * data, int w, int h, int numComponents )
{
	if( numComponents < 3 )
		return;
		
	unsigned char * c = data;
	
	for( int y = 0; y < h; y++ )
	{
		c = &data[ y*w*numComponents ];
		for( int x = 0; x < w; x++ )
		{
			int r = c[0];
			int b = c[2];
			c[0] = b;
			c[2] = r;
			c += numComponents;
		}
	}
}

static void flip( unsigned char * data, int w, int h, int numComponents )
{
	int pitch = w*numComponents;
	int sz = pitch*h;
	
	unsigned char * tmp = new unsigned char[sz];
	memcpy( tmp, data, sz );
	
	for( int y = 0; y < h; y++ )
	{
		unsigned char * src = &tmp[(h-1-y)*pitch];
		unsigned char * dst = &data[y*pitch];
		memcpy( dst,src,pitch );
	}
	
	delete [] tmp;
}

static unsigned char * loadImage( const char * filename, int * width, int * height, int * numComponents, int requestedNumComponents )
{
	FILE * f = fopen(filename,"rb");
	if( !f ) 
	{
		debugPrint("Could not open file %s\n",filename);
		return 0;
	}
	
	fseek(f, 0L, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0L, SEEK_SET);

	unsigned char * buf = new unsigned char[len];
	fread( buf, 1, len, f );
	
	unsigned char * data = stbi_load_from_memory( buf,len,width,height,numComponents,requestedNumComponents );
	
	bool res = true;
	
	if(data==0)
	{
		debugPrint("STB error: %s\n",stbi_failure_reason());
		res = true;
	}
	
	delete [] buf;
	fclose(f);
	
	if( res )
	{
		int n = (requestedNumComponents)?requestedNumComponents:*numComponents;
		
		if( n==4 )
			swizzleRGB(data,*width,*height,n);
		//flip(data,*width,*height,n);
	}
	
	return data;
}

//////////////////////////////////////////////////////////////////////

/// Load a texture from an image
static bool textureFromImage( cm::Texture * tex, const char * filename, int requestedNumComponents , int mipLevels )
{
	int imw,imh;
	int numComp = 0;
	
	unsigned char * data = loadImage( filename, &imw,&imh,&numComp,requestedNumComponents );
	
	if( data )
	{
		int n = requestedNumComponents?requestedNumComponents:numComp;
		Texture::FORMAT fmt;
		switch( n )
		{
			case 1:
				fmt = Texture::L8;
				//printf("L8 %s\n",hexDump(data,imw*imh*1).c_str());
				break;
			case 3:
				fmt = Texture::R8G8B8;
				//printf("R8G8B8 %s\n",hexDump(data,imw*imh*3).c_str());
				break;
				
			case 4:
				fmt = Texture::A8R8G8B8;
				//printf("A8R8G8B8 %s\n",hexDump(data,imw*imh*4).c_str());
				break;
				
			default:
				debugPrint("textureFromImage: Unknown format!\n");
				free(data);
				return 0;
		} 
		
		
		if( !tex->create(data,imw,imh,fmt, mipLevels ) )
		{
			debugPrint("textureFromImage: could not create texture!\n");
			free(data);
			return false;
		}
		
		free(data);
		
		return true;
	}
	else
		return false;
}

static Texture * textureFromImage( const char * filename, int requestedNumComponents, int mipLevels  )
{
	int imw,imh;
	int numComp = 0;
	
	unsigned char * data = loadImage( filename, &imw,&imh,&numComp,requestedNumComponents );
		
	if( data )
	{
		int n = requestedNumComponents?requestedNumComponents:numComp;
		Texture::FORMAT fmt;
		switch( n )
		{
			case 1:
				fmt = Texture::L8;
				break;
			case 3:
				fmt = Texture::R8G8B8;
				break;
				
			case 4:
				fmt = Texture::A8R8G8B8;
				break;
				
			default:
				debugPrint("textureFromImage: Unknown format!\n");
				free(data);
				return 0;
		} 
		
		Texture * t = new Texture();
		
		if( !t->create(data,imw,imh,fmt, mipLevels ) )
		{
			debugPrint("textureFromImage: could not create texture!\n");
			free(data);
			delete t;
			return 0;
		}
		
		free(data);
		
		return t;
	}
	else
		return 0;
}


//////////

static bool writeToPng( const char * filenmame, int w, int h, int numComp, const void * data, int strideInBytes )
{
	if(strideInBytes == 0)
		strideInBytes = w*numComp;
	return stbi_write_png( filenmame, w,h,numComp,data,strideInBytes );
}

//////////

static bool writeTextureToPng( const char * filenmame, Texture * tex, int startx , int starty )
{
	int numComp = 0;
	
	switch( tex->getFormat() )
	{
		case Texture::L8:
			numComp = 1;
			break;
			
		case Texture::R8G8B8:
			numComp = 3;
			break;
			
		case Texture::A8R8G8B8:
			numComp = 4;
			break;
			
		default:
			debugPrint("Unsupported texture format!\n");
			return false;
	}
	
	int stride = tex->getHardwareWidth()*numComp;
	
	char * data = (char*)tex->lock(Texture::LOCK_READ);
	
	data+=(starty*stride)+startx*numComp;
	bool res = stbi_write_png( filenmame, tex->getHardwareWidth(),tex->getHardwareHeight(),numComp,data,stride );
	tex->unlock();
	
	return res;
}

//////////

static bool writeToTga( const char * filenmame, int w, int h, int numComp, const void * data )
{
	return (bool)stbi_write_tga( filenmame, w,h,numComp,data );
}

//////////

static bool writeTextureToTga( const char * filenmame, Texture * tex )
{
	int numComp = 0;
	
	switch( tex->getFormat() )
	{
		case Texture::L8:
			numComp = 1;
			break;
			
		case Texture::R8G8B8:
			numComp = 3;
			break;
			
		case Texture::A8R8G8B8:
			numComp = 4;
			break;
			
		default:
			debugPrint("Unsupported texture format!\n");
			return false;
	}
	
	int stride = tex->getHardwareWidth()*numComp;
	
	char * data = (char*)tex->lock(Texture::LOCK_READ);
	bool res = stbi_write_tga( filenmame, tex->getHardwareWidth(),tex->getHardwareHeight(),numComp,data );
	tex->unlock();
	
	return res;
}

static void	initTexturing()
{
    textureCount++;
    
    if( currentSamplers )
        return;
    
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
    
    currentSamplers = new Texture* [maxTextureUnits];
    for( int i = 0; i < maxTextureUnits; i++ )
        currentSamplers[i] = 0;
}


static void deinitTexturing()
{
    if(textureCount > 0)
        textureCount--;
    
    if( textureCount == 0 && currentSamplers)
    {
        delete [] currentSamplers;
        currentSamplers = 0;
    }
}

void Texture::resetAllSamplers()
{
    for( int i = 0; i < maxTextureUnits; i++ )
    {
        if(currentSamplers[i])
        {
            const Texture::Info & info = currentSamplers[i]->info;
            glActiveTexture( GL_TEXTURE0+i );
            glBindTexture(info.glTarget,0);
            glDisable(info.glTarget); // TODO check me maybe it is not necessary
        }
        
        currentSamplers[i] = 0;
    }
    
    glActiveTexture( GL_TEXTURE0 );
}

Texture::Texture()
:
_boundSampler(-1),
_format(FORMAT_UNKNOWN),
flipX(false),
flipY(false)

{
    memset( &info, 0, sizeof( Texture::Info ) );
    info.glId = -1;
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
    memset( &info, 0, sizeof( Texture::Info ) );
    info.glId = -1;
    memset( matrix, 0, sizeof( matrix ) );
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
    load(path,miplevels);
}

Texture::Texture( int w, int h, int fmt, int miplevels  )
{
    
    memset( &info, 0, sizeof( Texture::Info ) );
    info.glId = -1;
    memset( matrix, 0, sizeof( matrix ) );
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
    create(w,h,fmt,miplevels);
}

Texture::~Texture()
{
    release();
}

void	Texture::release()
{
    unbind();
    
    if( info.glId != -1 )
    {
        deinitTexturing();
        gfx::releaseGLObject( GLObj(info.glId,GLObj::TEXTURE) );
    }
    
    memset( &info, 0, sizeof( Texture::Info ) );
    
    info.glId = -1;
}

void	Texture::bind( int sampler )
{
    
    
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

void	Texture::unbind()
{
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
    
    // texture must be bound!
    ASSERTBOUND
    glTexParameteri( info.glTarget, GL_TEXTURE_WRAP_S, glWrapTypes[wrap] );
    glTexParameteri( info.glTarget, GL_TEXTURE_WRAP_T, glWrapTypes[wrap] );
}

void	Texture::setWrapS( int  wrap )
{
    
    // texture must be bound!
    ASSERTBOUND
    glTexParameteri( info.glTarget, GL_TEXTURE_WRAP_S, glWrapTypes[wrap] );
}

void	Texture::setWrapT( int  wrap )
{
    
    // texture must be bound!
    ASSERTBOUND
    glTexParameteri( info.glTarget, GL_TEXTURE_WRAP_T, glWrapTypes[wrap] );
}

void	Texture::setMinFilter( int  filter )
{
    
    // texture must be bound!
    ASSERTBOUND
    glTexParameteri( info.glTarget, GL_TEXTURE_MIN_FILTER, glFilterTypes[filter] );
}

void	Texture::setMagFilter( int  filter )
{
    
    // texture must be bound!
    ASSERTBOUND
    glTexParameteri( info.glTarget, GL_TEXTURE_MAG_FILTER, glFilterTypes[filter] );
}

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
    
    if( gfx::areNonPowerOfTwoTexturesSupported() )
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
    
    if( mipLevels == 0 && gfx::GLExtension("GL_SGIS_generate_mipmap") )
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

bool	Texture::create( int w, int h, int  fmt, int mipLevels )
{
    const GLPixelFormatDesc & desc = glFormatDescs[fmt];
    
    int bufW = w;
    int bufH = h;
    if( !gfx::areNonPowerOfTwoTexturesSupported() )
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

void	Texture::setGLData(	int w, int h,
                           int hww, int hwh,
                           GLuint	glTarget,
                           GLuint	glId,
                           GLint		glFormat )
{
    init();
    
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
            if( gfx::areNonPowerOfTwoTexturesSupported() )
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

bool Texture::isValid()
{
    return info.glId != 0;
}

bool Texture::update( void * data, int w, int h)
{
    
    
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

bool Texture::grabFrameBuffer()
{
    
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

void	Texture::draw( float x  , float y  , float w , float h, bool yUp, float uscale, float vscale )
{
    
    
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


void * Texture::lock(int flags , int mipLevel )
{
    
    
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

bool Texture::unlock()
{
    
    
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


////////////////////////////////////////////////////////////////
// Image impl

void convertColor( const cv::Mat& src, cv::Mat& dst, int format )
{
	if(src.channels() == 1)
	{
		switch(format)
		{
			case Image::GRAYSCALE:
				dst = src.clone();
				break;
			case Image::BGR:
				cv::cvtColor(src, dst, CV_GRAY2BGR);
				break;
			case Image::BGRA:
				cv::cvtColor(src, dst, CV_GRAY2BGRA);
				break;
		}
	}
	else if(src.channels() == 3)
	{
		switch(format)
		{
			case Image::GRAYSCALE:
				cv::cvtColor(src, dst, CV_BGR2GRAY);
				
				break;
			case Image::BGR:
				dst = src.clone();
				break;
			case Image::BGRA:
				cv::cvtColor(src, dst, CV_BGR2BGRA);
				break;
		}
	}
	else if(src.channels() == 4)
	{
		switch(format)
		{
			case Image::GRAYSCALE:
				cv::cvtColor(src, dst, CV_BGRA2GRAY);
				
				break;
			case Image::BGR:
				cv::cvtColor(src, dst, CV_BGRA2BGR);
				break;
			case Image::BGRA:
				dst = src.clone();
				break;
		}
	}
	else
	{
		assert(false);
	}
}

/// Creates an empty image
Image::Image( int w, int h, int format )
{
	switch(format)
	{
		case Image::GRAYSCALE:
			mat = cv::Mat(h, w, CV_8UC1);
			break;
		case Image::BGR:
			mat = cv::Mat(h, w, CV_8UC3);
			break;
		case Image::BGRA:
			mat = cv::Mat(h, w, CV_8UC4);
			break;
		default:
			assert(0);
	}		

}

/// Copy image
Image::Image( const Image & mom )
{
	mat = mom.mat;
}

/// Copy image
Image::Image( const Image & mom, int format )
{
	convertColor(mom.mat, mat, format);
}

/// Create from opencv Mat
Image::Image( const cv::Mat & m )
{
	mat = m;
}

Image::Image( const std::string & path, int fmt )
{
    /// CV_LOAD_IMAGE_UNCHANGED (<0) loads the image as is (including the alpha channel if present)
    /// CV_LOAD_IMAGE_GRAYSCALE ( 0) loads the image as an intensity one
    /// CV_LOAD_IMAGE_COLOR (>0) loads the image in the RGB format

    int loadMode = CV_LOAD_IMAGE_UNCHANGED;
    if(fmt == Image::BGR)
    {
        loadMode = CV_LOAD_IMAGE_COLOR;
    }
    else if(fmt == Image::GRAYSCALE)
    {
        loadMode = CV_LOAD_IMAGE_GRAYSCALE;
    }
    
	mat = cv::imread(path.c_str(), loadMode);
}

// Code adapted from
// https://gist.github.com/psycharo/5b724fbae5f07008e7de
Image::Image( const arma::uchar_cube &src )
{
	std::vector<cv::Mat_<unsigned char>> channels;
    for (size_t c = 0; c < src.n_slices; ++c)
    {
    	arma::Mat<unsigned char> slice = src.slice(c); 
    	slice = slice.t();
        auto *data = const_cast<unsigned char*>(slice.memptr()); //src.slice(c).memptr());
        channels.push_back({int(src.n_rows), int(src.n_cols), data});
    }
 
    cv::merge(channels, mat);
}
	
/// Convert to armadillo cube
arma::uchar_cube Image::toArma() const
{
	// Type conv if necessary
	cv::Mat transposed;
	cv::transpose(mat, transposed);
    const cv::Mat * addr = &transposed;

    cv::Mat tmp;
    if((mat.type()&CV_MAT_DEPTH_MASK) != CV_8U)
    {
        double min, max;
        cv::minMaxLoc(transposed, &min, &max);
        tmp = transposed;
        tmp -= min;
        tmp.convertTo(tmp,CV_8U, 255.0/(max-min),-255.0/min);
        addr = &tmp;
    }
    
    const cv::Mat& src = *addr;
    std::vector<cv::Mat_<unsigned char>> channels;
    arma::uchar_cube dst(mat.rows, mat.cols, src.channels());
    for (int c = 0; c < src.channels(); ++c)
        channels.push_back({src.rows, src.cols, dst.slice(c).memptr()});
    cv::split(src, channels);
    return dst;
}



void Image::release()
{
	tex.release();
}

void Image::bind( int sampler )
{
	updateTexture();
    tex.bind(sampler);
	//gfx::bindTexture( tex.texId(), sampler );
	boundSampler = sampler;
}

void Image::unbind()
{
    tex.unbind();
	//gfx::bindTexture( boundSampler );
	boundSampler = -1;
}

void Image::mirror( bool x, bool y )
{
	int flipMode = 0;

	if( y && !x ) 
		flipMode = 0;
	else if( !y && x ) 
		flipMode = 1;
	else if( y && x ) 
		flipMode = -1;
	else return;

	cv::flip( mat, tmp, flipMode );
	mat = tmp;

	dirty = true;
}

void Image::updateTexture()
{
    
    if(dirty || tex.empty())
    {
        dirty = false;
        Texture::FORMAT tfmt;
        switch (mat.channels()) {
                
            case Image::BGR:
                if( !tex.create(mat.ptr(), mat.step/3, height(), Texture::R8G8B8) )
                {
                    debugPrint("Failed to create texture!");
                }
                break;
                
            case Image::BGRA:
                if( !tex.create(mat.ptr(), mat.step/4, height(), Texture::A8R8G8B8) )
                {
                    debugPrint("Failed to create texture!");
                }
                break;
                
            case Image::GRAYSCALE:
                if( !tex.create(mat.ptr(), mat.step, height(), Texture::L8) )
                {
                    debugPrint("Failed to create texture!");
                }
                break;
                
            
            default:
            	debugPrint("Cannot create texture for image format\n");
                break;
        }

        tex.setSize(width(), height());
    }
}


void Image::draw( float x, float y, float w, float h )
{
	if(empty())
		return;

	if( w == 0 )
		w = width();
	if( h == 0 )
		h = height();

	updateTexture();

	bool shouldBind = !isBound();

	// TODO: same for Geometry!!!
	if(shouldBind)
		bind();
	
	GLfloat tex_coords[8];
	
	// Might need to take into consideration non multiples of 16 here.		
	float u0 = 0;
	float v0 = 0;
	float u1 = 1.0; 
	float v1 = 1.0; 
	
	/*if(flipX)
		std::swap(u0,u1);
	if(flipY)
		std::swap(v0,v1);*/
	//if(yUp)
	//	std::swap(v0,v1);
	
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


	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, tex_coords );
	
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT, 0, verts );
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	//cv::ogl::render(tex,cv::Rect_<double>(x,y,w,h));

	if(shouldBind)
		unbind();
}

void Image::grabFrameBuffer()
{
	if(empty())
	{
		printf("Initialize image before grabbing frame buffer\n");
		assert(false);
	}

	GLint vp[4];

	glGetIntegerv( GL_VIEWPORT, vp );
	int vw = vp[2];
	int vh = vp[3];

	// from http://stackoverflow.com/questions/9097756/converting-data-from-glreadpixels-to-opencvmat/9098883#9098883
	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
	glPixelStorei(GL_PACK_ALIGNMENT, (mat.step & 3) ? 1 : 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, mat.step/mat.elemSize());

	int glFormat;
    if(mat.channels() == Image::GRAYSCALE)
		glFormat = GL_LUMINANCE;
    else if(mat.channels() == Image::BGR)
		glFormat = GL_BGR;
    else if(mat.channels() == Image::BGRA)
		glFormat = GL_BGRA;
	else
	{
		printf("Non recognized format for Image::grabFrameBuffer");
		assert(false);
	}
	
	int y = vh-height();

	glReadPixels(0, y, width(), height(), glFormat, GL_UNSIGNED_BYTE, mat.data);
	mirror(false, true);

	glPopClientAttrib();
}


void Image::save( const std::string& path ) const
{
	cv::imwrite(path, mat);
}


void Image::threshold(float thresh, bool inv)
{
	convertColor(mat, tmp, Image::GRAYSCALE);
    cv::threshold( tmp, mat, thresh, 255, inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY );
    dirty = true;
}


void Image::blur( float value, bool gaussian )
{
    int v = value;
    if(value==0)
        return;
    
    if( v % 2 == 0 )
        v++;
    
    if(gaussian)
        cv::GaussianBlur(mat, tmp, cv::Size(v,v),0);
    else
        cv::blur(mat, tmp, cv::Size(v,v));
    
    mat = tmp.clone();
    dirty = true;
}

Shape Image::findContours( bool approx, float minArea, float maxArea )
{
    convertColor(mat, tmp, Image::GRAYSCALE);

    /// Find contours
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours( tmp, contours, hierarchy, CV_RETR_TREE, approx?CV_CHAIN_APPROX_SIMPLE:CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );
    
    bool prune = false;
    if(minArea > 0.0)
        prune = true;
    
    Shape shape;
    for( int i = 0; i < contours.size(); i++ )
    {
        Contour ctr;
        const std::vector<cv::Point>& c=contours[i];
        for( int j = 0; j < c.size(); j++ )
            ctr.addPoint(c[j].x, c[j].y);
        
        if(prune)
        {
            Box box = ctr.boundingBox();
            float area = box.width() * box.height();
            if(area > minArea && area < maxArea)
                shape.add(ctr);
        }
        else
        {
            shape.add(ctr);
        }
    }
    
    return shape;
}


	

V3 ArcBall::projectToSphere( const V2 & p, const V2 & center, float radius, bool constrained, const V3& axis )
{
    V3 pt(0,0,0);
    pt.x = (p.x - center.x)/radius;
    pt.y = -(p.y - center.y)/radius;
    
    float r = dot(pt.xy(), pt.xy());
    if( r > 1. )
    {
        float s = 1. / sqrt(r);
        pt.x *= s;
        pt.y *= s;
        pt.z = 0.0;
    }
    else
    {
        pt.z = sqrt(1 - r);
    }
    
    // constrain
    if( constrained )
    {
        V3 proj = pt - axis*dot(pt, axis);
        float norm = arma::norm(proj);
        
        if( norm > 0. )
        {
            float s = 1. / norm;
            if( proj.z < 0 )
                s = -s;
            pt = proj*s;
        }
        else
        {
            if( axis.z == 1. )
                pt = V3(1, 0, 0);
            else
                pt = normalize( V3(-axis.y, axis.x, 0) );
        }
    }
    
    return pt;
}

void ArcBall::update( const V2 & mousePos, const V2 & mouseDelta, const V2 & center, float radius, bool contrained )
{
    V3 axis(0,1,0);
    if(fabs(mouseDelta.y) > fabs(mouseDelta.x))
    {
        axis = V3(1,0,0);
    }
    
    V3 p1 = projectToSphere( mousePos, center, radius, contrained, axis );
    V3 p2 = projectToSphere( mousePos + mouseDelta, center, radius, contrained, axis );
    
    V4 q;
    V3 xyz = cross(p1, p2);
    q.x = xyz.x; q.y = xyz.y; q.z = xyz.z;
    q.w = dot(p1, p2);
    
    rot = quatMul(q, rot);
    mat = m44(rot);
}

/////////////////////////////////////////
// Shader interface
namespace gfx
{

struct Shader : public GLObj
{
	Shader( int id=-1 )
	:
	GLObj(id, GLObj::SHADER)
	{

	}

	int refcount=0;
};

struct ShaderProgram : public GLObj
{
	ShaderProgram( int id=-1, int vs=-1, int ps=-1 )
	:
	GLObj(id, GLObj::PROGRAM ),
	ps(ps),
	vs(vs)
	{

	}
    
    int ps=-1;
    int vs=-1;
};


typedef std::map<int, Shader> ShaderMap; 
typedef std::map<int, ShaderProgram> ShaderProgramMap; 

static ShaderProgramMap shaderProgramMap;
static ShaderMap shaderMap;

static int curProgram=-1;
static std::vector<Texture*> shaderTextures;

static void incShaderRefCount(int id)
{
    auto it = shaderMap.find(id);
    if(it != shaderMap.end())
        it->second.refcount++;
}
    
    
void removeShader( int id )
{
    auto it = shaderMap.find(id);
    if(it == shaderMap.end())
        return;
    it->second.refcount--;
    if( it->second.refcount <= 0 )
    {
        releaseGLObject(it->second);
        shaderMap.erase(it);
    }
}

void deleteShaderProgram( int id )
{
    auto it = shaderProgramMap.find(id);
    if(it == shaderProgramMap.end())
        return;
    releaseGLObject(it->second);
    removeShader(it->second.vs);
    removeShader(it->second.ps);
    shaderProgramMap.erase(it);
}

void deleteAllShaders()
{
    {
        auto it = shaderMap.begin();
        while( it != shaderMap.end() )
        {
            releaseGLObject(it->second);
            it++;
        }
    }
    
    {
        auto it = shaderProgramMap.begin();
        while( it != shaderProgramMap.end() )
        {
            releaseGLObject(it->second);
            it++;
        }
    }
}

static bool compileShader( GLuint id, const char* shaderStr )
{	
	char errors[1024];
	
	glShaderSource(id, 1, (const GLchar**)&shaderStr,NULL);
	
	GLsizei len;
	GLint compiled;
	
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	
	if (!compiled)
	{
		glGetShaderInfoLog(	id, 1024, &len, errors );
		printf("shader errors: %s\n",errors);
        return false;
	}     
	
	return true;
}

static std::string addVersion( const std::string& str )
{
	return std::string("\n#version 120\n\n") + str;
}

int loadVertexShader( const std::string& vs_ )
{
	int id = glCreateShader(GL_VERTEX_SHADER);
	std::string vs = addVersion(vs_);
	if(!compileShader(id, vs.c_str()))
	{
		return -1;
	}
    
    shaderMap[id] = Shader(id);

	return id;
}

int loadPixelShader( const std::string& ps_ )
{
	int id = glCreateShader(GL_FRAGMENT_SHADER);
	std::string ps = addVersion(ps_);
	if(!compileShader(id, ps.c_str()))
	{
		return -1;
	}
    
    shaderMap[id] = Shader(id);
    
	return id;
}

int	linkShader( int vs, int ps ) 
{
	int id = glCreateProgram();
	
	glAttachShader(id, vs );
	glAttachShader(id, ps );
	
	
	// bind attributes
	/*
	for( u32 i = 0; i < _vertexAttributes.size() ; i++ )
	{
		VertexAttribute * a = _vertexAttributes[i];

		glBindAttribLocation(this->_glId, GENERIC_ATTRIB(a->index), a->name.str);
		
		if(getGLGfx()->getGLError())
		{
			debugPrint("In GLShaderProgram::link");
			return false;
		}

		delete _vertexAttributes[i];
	}

	_vertexAttributes.clear();
	*/
	
	glLinkProgram(id);
	
	GLint linked;
	char errors[1024];
	GLsizei len;

	glGetProgramiv(id, GL_LINK_STATUS, &linked);
	if (!linked)
	{
	   glGetProgramInfoLog(id, 1024, &len, errors );
	   printf("GLSL Shader linker error:%s\n",errors);
	   assert(0);
	   return -1;
	}      

    shaderProgramMap[id] = ShaderProgram(id, vs, ps);
    incShaderRefCount(vs);
    incShaderRefCount(ps);
    
	return id;
}

int loadShader( const std::string& vs, const std::string& ps )
{
	int vsid = loadVertexShader(vs);
	if(vsid < 0)
		return -1;

	int psid = loadPixelShader(ps);
	if(psid < 0)
		return -1;
	
	return linkShader( vsid, psid );
}

int reloadShader( int id, const std::string& vs, const std::string& ps )
{
	int newid = loadShader(vs, ps);
	if(newid==-1)
		return id;

	deleteShaderProgram(id);
	return newid;
}

static int getUniformLocation( const std::string& handle )
{
	return glGetUniformLocation( curProgram, handle.c_str() );
}

bool setTexture( const std::string& handle, Texture& tex, int sampler )
{
	int id = getUniformLocation(handle);
	if(id == -1)
		return false;
		
	glUniform1i(id,sampler);
	
	tex.bind(sampler);
	shaderTextures.push_back(&tex);
    
    return true;
}

void bindShader( int id )
{
	glUseProgram(id);
	shaderTextures.clear();
	curProgram = id;
}

void unbindShader()
{
	for( int i = 0; i < (int)shaderTextures.size(); i++ )
		shaderTextures[i]->unbind();
	shaderTextures.clear();
	glUseProgram(0);
	curProgram = -1;
}

bool setInt( const std::string& handle, int v )
{
	int id = getUniformLocation(handle);
	if(id == -1)
		return false;
	
	glUniform1i(id, v);
	return true;
}

bool setFloat( const std::string& handle, float v )
{
	int id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform1f(id, v);
	
	return true;
}

bool setFloat2( const std::string& handle, const V2& v )
{
	int id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform2f(id, v.x, v.y);
	return true;
}

bool setFloat3( const std::string& handle, const V3& v )
{
	int id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform3f(id, v.x, v.y, v.z);
	return true;
}

bool setFloat4( const std::string& handle, const V4& v )
{
	int id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform4f(id, v.x, v.y, v.z, v.w);
	return true;
}

bool setM44( const std::string& handle, const M44& v )
{
	int id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniformMatrix4fv(id, 1, GL_FALSE, (const float*)&v[0]);
	return true;
}

// TODO fixme
// bool setM44Array( const std::string& handle, const float* data, int n )
// {
// 	int id = getUniformLocation(handle);
// 	if(id == -1)
// 		return false;
	
// 	glUniformMatrix4fv(id, n, GL_FALSE, data);
// 	return true;
// }

// bool setM44Array( const std::string& handle, const std::vector<M44>& v )
// {
// 	return setM44Array(handle, (const float*)&v[0], v.size());
// }

// bool setM44Array( const std::string& handle, const std::vector<M44>& v )
// {
// 	return setM44Array(handle, (const float*)&v[0], v.size());
// }

bool setFloatArray( const std::string& handle, float*v, int n)
{
	int id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform1fv(id, n, v);
	return true;
}

}


/// Hsv
V4 Color::hsv(float h, float s, float v)
{
    V4 clr;
    clr.w = 1.;
    
    if (s == 0.0f){
        clr.x = clr.y = clr.z = v;
    }else{
        float f,p,q,t;
        float hue=h;
        
        int i;
        
        if( h == 360.0f)
            hue = 0.0f;
        
        hue /= 60.0f;
        i = (int)floor(hue);
        
        f = hue - i;
        p = v * (1.0f - s);
        q = v * (1.0f - ( s * f ));
        t = v * (1.0f - ( s * (1.0f - f)));
        
        switch(i){
            case 0:
                clr.x = v; clr.y = t; clr.z = p;
                break;
            case 1:
                clr.x = q; clr.y = v; clr.z = p;
                break;
                
            case 2:
                clr.x = p; clr.y = v; clr.z = t;
                break;
                
            case 3:
                clr.x = p; clr.y = q; clr.z = v;
                break;
                
            case 4:
                clr.x = t; clr.y = p; clr.z = v;
                break;
                
            case 5: 
                clr.x = v; clr.y = p; clr.z = q;
                break;
                
        }
        
    }

    return clr;
}


} // end cm