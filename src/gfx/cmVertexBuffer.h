///////////////////////////////////////////////////////////////////////////                                                     
//	 _________  __   ____  ___  __  _______  __________  ___			
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \			
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/			
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.		
//																		
//  Daniel Berio 2008-2011												
//	http://www.enist.org/												
//																	
//																		
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gfx/CMGfxIncludes.h"
#include "gfx/CMHardwareBuffer.h"
#include "gfx/CMVertexFormat.h"

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