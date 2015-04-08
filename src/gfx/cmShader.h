

#pragma once 

#include "gfx/cmGfxIncludes.h"
#include "gfx/cmTexture.h"
#include "cmMath.h"

namespace cm
{

/// \todo add shader object for linking multiple shaders
/// It's allready done in old CM!
struct Shader
{
	enum TYPE
	{
		VERTEX_SHADER = 0,
		PIXEL_SHADER,
		GEOMETRY_SHADER // TODO! 
	};
	
	TYPE type;
	GLuint glId;
	char * code;
};

/// \class ShaderProgram
class ShaderProgram : public GfxObject
{
	public:
		ShaderProgram();
		~ShaderProgram();
		
		void release();
		
		bool load( const char * vsPath, const char * psPath );
		bool loadString( const char * vsString, const char * psString , const char * header= 0);
		
		bool setFloat( const char * handle,float val );
		bool setFloatArray( const char * handle,const float *val, int size );
		bool setMatrix4x4( const char * handle,const float *val );
		bool setVec4( const char * handle,const float *val );
		bool setVec3( const char * handle,const float *val );	
		bool setVec2( const char * handle,const float *val );	
		bool setInt( const char * handle, int val );
		bool setBool( const char * handle, bool val );
		bool setValue( const char * handle, void * data, int size );
		bool setMatrix4x4Array( const char * handle, const  float * data, int size );
		bool setMatrix4x4Array( const char * handle, const std::vector<M44> &M );

		bool setTexture( const char * handle, Texture * tex, int sampler = 0 ); 
		bool setTextureGL( const char * handle, int id, int sampler = 0 );
		bool bind();
		void unbind();
		
		int getNumTextures() const;
		
		/// Create a  C header file from a vertex shader and a pixel shader.
		/// strings are named as the filenames without file extension, 
		/// Also must eventually handle geometry shaders and multiple shaders.
		///
		/// If addFragVertExt flag is set to true vertex shader string name is appended with "Vert"
		/// and fragment shader string is appended with "Frag"
		static bool createHeader( const char * headerFile, const char * vsFile,const char * psFile,bool addFragVertExt = false);
		
		static std::string defaultPath;
		
	protected:
		GLint getUniformLocation( const char * handle );
		bool compileShader( GLuint id, const char * shaderStr );
		static bool createShaderString(  std::string & out, const char * path );
		bool link();
		
		std::vector <Texture*> _boundTextures;
		
		// vertex and pixel shader id's
		GLuint _vsId;
		GLuint _psId;
		
		std::string vsSrc;
		std::string psSrc;
		
		// shader program id
		GLuint _progId;
		
		// maps uniform names to GLint location TODO...
		//std::map <std::string,GLint>	_uniformMap;
};

}