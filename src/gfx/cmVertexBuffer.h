

#pragma once

#include "gfx/cmGfxIncludes.h"
#include "gfx/cmHardwareBuffer.h"
#include "gfx/cmVertexFormat.h"

namespace cm
{

///	Vertex buffer object.
class	VertexBuffer : public HardwareBuffer
{
public:

	VertexBuffer();
	~VertexBuffer();
	
	void	release();

	bool	update( void * data, int count = 0, int start = 0 );
	
	/// TODO: needs work..
	int		updateDynamic( void * data, int count );
	
	bool	init( VertexFormat * fmt , int numVertices, USAGE usage = USAGE_DEFAULT	);
	bool	init( const char * format, int numVertices, USAGE usage = USAGE_DEFAULT );
	
	GLuint  getGLId() const { return _glId; }
	VertexFormat * getVertexFormat() { return _fmt; }
	
protected:
	VertexFormat * _fmt;
	bool	_ownFormat;
	
	GLenum _glUsage;
	GLuint _glId;
};

}