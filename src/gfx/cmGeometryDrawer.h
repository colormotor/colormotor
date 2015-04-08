

#pragma once 

#include "gfx/cmGfxIncludes.h"
#include "gfx/cmGfxUtils.h"

namespace cm
{

class GeometryDrawer
{
public:
	enum 
	{
		MAX_VERTEX_ELEMENTS = 14
	};
	
	static void reset();
	static void setColorPointer( unsigned int * buf, int stride = 0 );
	static void setVertexPointer( void * buf, int stride = 0 );
	static void setTexCoordPointer( void * buf, int stride = 0 );
	static void setNormalPointer( void * buf, int stride = 0 );
	static void drawArrays( PRIMITIVE prim, int numElements, int startElement );
	static void drawElements( PRIMITIVE prim, unsigned short * inds, int numElements );
	static void drawElements( PRIMITIVE prim, unsigned int * inds, int numElements );
};

}