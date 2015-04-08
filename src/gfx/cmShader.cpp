

#include "cmGLInternal.h"
#include "cmShader.h"


namespace cm
{



std::string ShaderProgram::defaultPath = "/data/shaders";

ShaderProgram::ShaderProgram()
:
_vsId(0),
_psId(0),
_progId(0)
{
}


//////


ShaderProgram::~ShaderProgram()
{
	release();
}


//////



void ShaderProgram::release()
{
	CM_GLCONTEXT
	
	
	if( _vsId )
	{
		if( isGLReleaseManual() )
		{
			releaseGLObject( GLObj(_vsId,GLObj::SHADER) );
		}
		else
			glDeleteShader(_vsId);
		_vsId = 0;
	}
	
	if( _psId )
	{
		if( isGLReleaseManual() )
		{
			releaseGLObject( GLObj(_psId,GLObj::SHADER) );
		}
		else
			glDeleteShader(_vsId);
		_psId = 0;
	}
	
	if( _progId )
	{
		if( isGLReleaseManual() )
		{
			releaseGLObject( GLObj(_progId,GLObj::PROGRAM) );
		}
		else
			glDeleteProgram(_progId);
		_progId = 0;
	}
}


//////


bool ShaderProgram::createShaderString( std::string & out, const char * path )
{
	FILE * f = fopen( path, "r" );
	
	if( !f )
	{
		std::string filename = getFilenameFromPath(path);
		std::string def = defaultPath + "/" + filename;
		f = fopen( def.c_str(), "r" );
		
		if( !f )
		{
			debugPrint("Could not open file %s\n",path);
			#ifdef _DEBUG
			//assert(0);
			#endif
			fclose(f);
			return false;
		}
	}
	
	int size = getFileSize(f);
	
	char * fstr = new char[size+1];
	fread(fstr,1,size,f);
	fstr[size] = '\0';
	
	std::string str = fstr;
	delete [] fstr;
	fclose(f);
	
	out = "";
	out = "/*added manually*/\n#version 120\n\n";
	
	// search for includes
	int  cur = 0;
	int ind = 0;
	
	while(true)
	{
		ind = str.find("#include",cur);
		if( ind == -1)
			break;
			
		// be sure line isnt commented... hack just checking for one /
		// TODO should have comment blocks too... search for  searchComment function...
		int newline = str.rfind('\n',ind); 
		int comm = str.rfind('/',ind);
		if( comm > newline )
		{
			debugPrint("found comment\n");

			continue;
		}
		
		// add stuff in the middle
		if( ind > cur )
			out+=str.substr(cur,ind-cur-1);
		
		int b1 = str.find('\"',ind);
		if( b1 == -1)
		{
			debugPrint("cmShader:: preprocessor error expected \" after #include 'n");
			return false;
		}
		
		int b2 = str.find('\"',b1+1);
		if( b2 == -1)
		{
			debugPrint("cmShader:: preprocessor error expected \" \n");
			return false;
		}
		
		cur = str.find('\n',b2);
		if( cur == -1 )
		{
			debugPrint("cmShader:: preprocessor error expected new line \n");
			return false;
		}
		
		std::string incPath = str.substr(b1+1,b2-b1-1);//,)
		
		std::string included;
		if( !createShaderString(included,incPath.c_str()) )
			return false;
			
		out+=included;
		
	}

	// append remaining stuff
	
	if( cur < (int)str.size() )
	{
		std::string sub =  str.substr(cur,str.size()-cur);
		
		out += sub;//str.substr(cur,str.size()-cur-1);
		//debugPrint("%s\n",out.c_str());
	}
	
	
	
	
	return true;
}


//////

bool ShaderProgram::compileShader( GLuint id, const char * shaderStr )
{
	CM_GLCONTEXT
	
	char errors[1024];
	
	glShaderSource(id, 1, (const GLchar**)&shaderStr,NULL);
	
	GLsizei len;
	GLint compiled;
	
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	
	if (!compiled)
	{
		glGetShaderInfoLog(	id, 1024, &len, errors );
		debugPrint("shader errors: %s\n",errors);
        //debugPrint("\n\n%s\n\n####################\n",shaderStr);
		//assert(0);
		return false;
	}     
	
	return true;
}

//////


bool ShaderProgram::load( const char * vsPath, const char * psPath )
{
	CM_GLCONTEXT
	
	// create vertex shader
	_vsId = glCreateShader(GL_VERTEX_SHADER);
	
	std::string vsStr;
	
	if(!createShaderString(vsStr,vsPath))
	{
		debugPrint("Could not create vertex shader string\n");
		//assert(0);
		return false;
	}
	
	vsSrc = vsStr;
	
	if( !compileShader(_vsId,vsStr.c_str()) )
	{
		debugPrint("Error compiling vertex shader %s\n",vsPath);
		//assert(0);
		return false;
	}
	
	
	// create pixel shader
	_psId = glCreateShader(GL_FRAGMENT_SHADER);
	
	std::string psStr;
	
	if(!createShaderString(psStr,psPath))
	{
		debugPrint("Could not create pixel shader string\n");
		//assert(0);
		return false;
	}
		
	psSrc = psStr;
	
	//debugPrint("%s\n",psStr.c_str());
	if( !compileShader(_psId,psStr.c_str()) )
	{	
		debugPrint("Error compiling pixel shader %s\n",psPath);
		//assert(0);
		return false;
	}
	
	return link();
}


bool	ShaderProgram::link() 
{	
	CM_GLCONTEXT
	
	_progId = glCreateProgram();
	
	glAttachShader(_progId, _vsId );
	glAttachShader(_progId, _psId );
	
	
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
	
	glLinkProgram(_progId);
	
	GLint linked;
	char errors[1024];
	GLsizei len;

	glGetProgramiv(_progId, GL_LINK_STATUS, &linked);
	if (!linked)
	{
	   glGetProgramInfoLog(	_progId, 1024, &len, errors );
	   debugPrint("GLSL Shader linker error:%s\n",errors);
	   assert(0);
	   return false;
	}      

	return true;
}


//////


bool ShaderProgram::loadString( const char * vsStr, const char * psStr, const char * header  )
{
	CM_GLCONTEXT
	
	std::string ps;
	std::string vs;
	
	if(header)
		stdPrintf(vs,"%s\n%s\n",header,vsStr);
	else
		vs = vsStr;
	
	if(header)
		stdPrintf(ps,"%s\n%s\n",header,psStr);
	else
		ps = psStr;
		
	vsSrc = vs;
	psSrc = ps;
	
	/*
	if(forceTexture2DRect)
	{
		replace(ps,"sampler2D","sampler2DRect");
		replace(ps,"texture2D","texture2DRect");
		
		printf("%s\n",ps.c_str());
	}*/
	
	// create vertex shader
	_vsId = glCreateShader(GL_VERTEX_SHADER);
			
	if( !compileShader(_vsId,vs.c_str()) )
		return false;
	
	
	// create pixel shader
	_psId = glCreateShader(GL_FRAGMENT_SHADER);
	
	if( !compileShader(_psId,ps.c_str()) )
		return false;
	
	return link();
	
}


int ShaderProgram::getNumTextures() const 
{
	return countSubstring(psSrc, "sampler2D");
	// todo add rectangle cube etc...
}

//////



bool ShaderProgram::setFloat( const char * handle,float val )
{
	CM_GLCONTEXT
	
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform1f(id,val);
	
	return true;
}


//////


bool ShaderProgram::setFloatArray( const char * handle,const float *val, int size )
{
	CM_GLCONTEXT
	
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform1fv(	id,size,val );
	
	return true;
}


//////


bool ShaderProgram::setMatrix4x4( const char * handle,const float *val )
{
	CM_GLCONTEXT
	
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniformMatrix4fv(id, 1, GL_FALSE, (const float*)val);
	
	return true;
}


//////


bool ShaderProgram::setVec4( const char * handle,const float *val )
{
	CM_GLCONTEXT
	
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform4f(id,val[0],val[1],val[2],val[3]);
	
	return true;
}


//////


bool ShaderProgram::setVec3( const char * handle,const float *val )
{
	CM_GLCONTEXT
	
	
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform3f(id,val[0],val[1],val[2]);
	
	return true;
}

//////

bool ShaderProgram::setVec2( const char * handle,const float *val )
{
	CM_GLCONTEXT
	
	
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
	glUniform2f(id,val[0],val[1]);
	
	return true;
}


//////

	
bool ShaderProgram::setInt( const char * handle, int val )
{
	CM_GLCONTEXT
	
	// TODO test me
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
	
	glUniform1i(id,val);
	
	return true;
}


//////


bool ShaderProgram::setBool( const char * handle, bool val )
{
	// TODO
	debugPrint("unimplemented!\n");
	return false;
}


//////


bool ShaderProgram::setValue( const char * handle, void * data, int size )
{
	// TODO
	debugPrint("unimplemented!\n");
	return false;
}


//////


bool ShaderProgram::setMatrix4x4Array( const char * handle, const  float * data, int size )
{
	CM_GLCONTEXT
	
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
	
	glUniformMatrix4fv(id,size,GL_FALSE,(const float*)data);
	
	return true;

}

bool ShaderProgram::setMatrix4x4Array( const char * handle, const std::vector<M44> &M )
{
	return setMatrix4x4Array(handle,(const float*)&M[0],M.size());
}
//////



bool ShaderProgram::setTexture( const char * handle, Texture * tex, int sampler  )
{
	CM_GLCONTEXT
	
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
		
	glUniform1i(id,sampler);
	
	tex->bind(sampler);
	_boundTextures.push_back(tex);
	
	if(CM_GLERROR)
		debugPrint("glerror\n");
	
	
	
	if(CM_GLERROR)
		debugPrint("glerror\n");
	
	return true;
}

bool ShaderProgram::setTextureGL( const char * handle, int texId, int sampler )
{
	CM_GLCONTEXT
	
	GLint id = getUniformLocation(handle);
	if(id == -1)
		return false;
		
	glUniform1i(id,sampler);
	
	glActiveTexture( GL_TEXTURE0+sampler );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texId);
	
	if(CM_GLERROR)
		debugPrint("glerror\n");
	
	
	return true;
}

//////

 
GLint ShaderProgram::getUniformLocation( const char * handle )
{
	CM_GLCONTEXT
	
	// TODO use map probably faster
	return glGetUniformLocation( _progId, handle );
}


//////


bool ShaderProgram::bind()
{		
	CM_GLCONTEXT
	
	glUseProgram(_progId);
	_boundTextures.clear();
	
	if(CM_GLERROR)
		return false;
		
	return true;
}

void ShaderProgram::unbind()
{
	CM_GLCONTEXT
	for( int i = 0; i < (int)_boundTextures.size(); i++ )
		_boundTextures[i]->unbind();
	_boundTextures.clear();
	
	glUseProgram(0);
	
}

bool	ShaderProgram::createHeader( const char * headerFile, const char * vsFile,const char * psFile,bool addFragVertExt )
{
	std::string vsStr;
	std::string psStr;
	
	std::string vsName,psName;
			
	
	FILE * f = fopen(headerFile,"w+");
	if(!f)
		return  false;

	std::string curline;
	
	if( vsFile )
	{
		if( !createShaderString(vsStr,vsFile) )
			return false;
			
		vsName = getFilenameWithoutExt( getFilenameFromPath(vsFile) );
		if(addFragVertExt)
			vsName+="Vert";
			
		FILE * vf = fopen( vsFile, "r" );
		if( !vf )
			return false;
			
		fprintf(f,"#pragma once\n\n\n");
	
		fprintf(f,"const char * %s = ", vsName.c_str() );
		
		
		int cur = 0;
		
		while( (cur = readLine(curline,vsStr,cur) ) != -1 )
		{
			fprintf(f,"\"%s\\n\"\n",curline.c_str());
		}
		
		fprintf(f,"\"//end\";\n\n\n");
		
		fclose(vf);
	}
	
	if( psFile )
	{
		if( !createShaderString(psStr,psFile) )
			return false;
			
		psName = getFilenameWithoutExt( getFilenameFromPath(psFile) );
		
		if(addFragVertExt)
			psName+="Frag";
		
		
		FILE * pf = fopen( psFile, "r" );
		if( !pf )
			return  false;
			
		fprintf(f,"const char * %s = ", psName.c_str() );
		
		int cur = 0;
		
		while( (cur = readLine(curline,psStr,cur) ) != -1 )
		{
			fprintf(f,"\"%s\\n\"\n",curline.c_str());
		}

		fprintf(f,"\"//end\";\n\n\n");
			
		fclose(f);
		
		fclose(pf);
	}
	
	
	fclose(f);
	
	return true;
}

}




