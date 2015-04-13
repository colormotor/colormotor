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

#include "gfx/cmGfxIncludes.h"
#include "gfx/cmHardwareBuffer.h"

namespace cm
{

class	IndexBuffer : public HardwareBuffer
{
public:
	enum TYPE
	{
		INDEX_16 = 2,
		INDEX_32 = 4
	};
	
	IndexBuffer();
	~IndexBuffer();

	virtual void release ();
	
	bool	update( void * data,  int count = 0, int start = 0);
	bool	init(	int size, TYPE type = INDEX_16 ,USAGE usage = HardwareBuffer::USAGE_DEFAULT	);

	GLuint  getGLId() const { return _glId; }
	GLenum  getGLIndexFormat() const { return _glIndexFmt; }
	
protected:

	GLenum _glIndexFmt;
	GLenum _glUsage;
	GLuint _glId;
};


}