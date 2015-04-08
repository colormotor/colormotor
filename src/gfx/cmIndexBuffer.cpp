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

#include "gfx/CMGLInternal.h"
#include "gfx/CMIndexBuffer.h"

namespace cm
{


IndexBuffer::IndexBuffer()
{
	_glId = 0;
}

IndexBuffer::~IndexBuffer()
{
	release();
}

void IndexBuffer::release ()
{
	CM_GLCONTEXT
	
	if( _glId )
	{
		if( isGLReleaseManual() )
		{
			releaseGLObject( GLObj(_glId,GLObj::IB) );
		}
		else
			glDeleteBuffers(1, &_glId);
		_glId = 0;
	}
	
	HardwareBuffer::release();
}

bool	IndexBuffer::init(	int size, IndexBuffer::TYPE type ,USAGE usage )
{
	int stride = (int)type;
	
	HardwareBuffer::init(size,stride,usage);
	
	if( type == IndexBuffer::INDEX_32 )
	{
		_glIndexFmt = GL_UNSIGNED_INT;
	}
	else
	{
		_glIndexFmt = GL_UNSIGNED_SHORT;
	}
	
	_glUsage = GL_STATIC_DRAW;
	// TODO questi li ho messi un po a caso....
	if( usage & USAGE_DYNAMIC )
	{
		if( usage & USAGE_WRITEONLY )
			_glUsage = GL_STREAM_DRAW;
		else
			_glUsage = GL_DYNAMIC_DRAW;
	}
	
	
	_glId = cmCreateVBO(0,size*stride,GL_ELEMENT_ARRAY_BUFFER, _glUsage);
	
	if(_glId == 0)
		return false;
	

	return true;
}

bool	IndexBuffer::update( void * data, int count, int start  )
{
	CM_GLCONTEXT
	
	if( count == 0 )
		count = getSize();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glId);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, start*_stride, count*_stride, data);
	
	if( CM_GLERROR )
	{
		debugPrint("GLIndexBuffer::update error\n");
		return false;
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	return true;
	
}

}