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
#include "gfx/CMGfxUtils.h"

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