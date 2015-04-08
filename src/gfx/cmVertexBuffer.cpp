

#include "gfx/cmGLInternal.h"
#include "gfx/cmVertexBuffer.h"
#include "gfx/cmGfxUtils.h"

namespace cm
{





VertexBuffer::VertexBuffer()
{
	_glId = 0;
	_fmt = 0;
	_ownFormat = false;
}

VertexBuffer::~VertexBuffer()
{
	release();
}

void VertexBuffer::release()
{
	CM_GLCONTEXT
	
	if( _ownFormat && _fmt )
		delete _fmt;
	_fmt = 0;
	
	if( _glId )
	{
		if( isGLReleaseManual() )
		{
			releaseGLObject( GLObj(_glId,GLObj::VB) );
		}
		else
			glDeleteBuffers(1, &_glId);
		_glId = 0;
	}
	
	HardwareBuffer::release();
}

bool	VertexBuffer::init( VertexFormat * fmt , int numVertices, HardwareBuffer::USAGE usage 	)
{
	_fmt = fmt;
	
	HardwareBuffer::init( numVertices, fmt->getSize(), usage );
	
	_glUsage = GL_STATIC_DRAW;
	
	// TODO test these
	if( usage & USAGE_DYNAMIC )
	{
		if( usage & USAGE_WRITEONLY )
			_glUsage = GL_STREAM_DRAW;
		else
			_glUsage = GL_DYNAMIC_DRAW;
	}
	
	
	_glId = cmCreateVBO(0,numVertices*getStride(),GL_ARRAY_BUFFER, _glUsage);
  
	if(_glId == 0)
		return false;
	
	return true;

}

bool	VertexBuffer::init( const char * format, int numVertices, HardwareBuffer::USAGE usage  )
{
		if(! init( new VertexFormat(format), numVertices, usage ) )
			return false;
			
		_ownFormat = true;
		
		return true;
}
	
int	VertexBuffer::updateDynamic( void * data, int count )
{
	CM_GLCONTEXT
	
	if( _nextFreeIndex > _size - count )
	{
		_nextFreeIndex = 0;
		// DISCARD?
		//glBufferDataARB(GL_ARRAY_BUFFER_ARB, _size*_stride, 0, _glUsage); 
	}
	
	int ind = _nextFreeIndex;
	
	glBindBuffer(GL_ARRAY_BUFFER, _glId);
	glBufferSubData(GL_ARRAY_BUFFER, _nextFreeIndex*_stride, count*_stride, data);
	
	if( CM_GLERROR )
	{
		debugPrint("GLVertexBuffer::update error\n");
		return -1;
	}
	
	_nextFreeIndex += count;
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	return ind;
}

bool	VertexBuffer::update( void * data, int count, int start  )
{
	CM_GLCONTEXT
	
	if( count == 0 )
		count = getSize();
	
	glBindBuffer(GL_ARRAY_BUFFER, _glId);
	glBufferSubData(GL_ARRAY_BUFFER, start*_stride, count*_stride, data);
	
	if( CM_GLERROR )
	{
		debugPrint("GLVertexBuffer::update error\n");
		return false;
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return true;
}


}


