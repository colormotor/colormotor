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
#include "gfx/CMGeometryDrawer.h"
#include "gfx/CMVertexFormat.h"

namespace cm
{

static int texCount = 0;
static int genericCount = 0;


static VertexFormat::ELEMENT_TYPE currentElementTypes[GeometryDrawer::MAX_VERTEX_ELEMENTS];


void GeometryDrawer::reset()
{
	CM_GLCONTEXT
	
	int curTexCoord = 0;
	for (int i = 0; i < genericCount; i++ )
	{
		VertexFormat::ELEMENT_TYPE e = currentElementTypes[i];
		
		switch (e) 
		{
			case VertexFormat::POS3:
			case VertexFormat::POS4:
				glDisableClientState(GL_VERTEX_ARRAY); 
				break;
				
			case VertexFormat::UVW:
			case VertexFormat::UV:
				glClientActiveTexture(GL_TEXTURE0+curTexCoord);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
				curTexCoord++;
				break;
			
			case VertexFormat::COLOR:
				glDisableClientState(GL_COLOR_ARRAY); 
				break;
				
			case VertexFormat::NORMAL:
				glDisableClientState(GL_NORMAL_ARRAY); 
				break;

			default:
				break;
		}
	}
	
	genericCount = 0;
	texCount = 0;
	
}

	
//////////////////////////////////////////////////////////

void GeometryDrawer::setColorPointer( unsigned int * buf, int stride )
{
	CM_GLCONTEXT
	
	glEnableClientState(GL_COLOR_ARRAY); 
	glColorPointer(4,GL_UNSIGNED_BYTE,stride,buf );
	currentElementTypes[genericCount] = VertexFormat::COLOR;
	++genericCount;
}

void GeometryDrawer::setVertexPointer( void * buf, int stride  )
{
	CM_GLCONTEXT
	
	glVertexPointer(	3,
						GL_FLOAT,
						stride,
						buf );
	glEnableClientState(GL_VERTEX_ARRAY); 
	currentElementTypes[genericCount] = VertexFormat::POS3;
	++genericCount;
}

void GeometryDrawer::setTexCoordPointer( void * buf, int stride  )
{
	CM_GLCONTEXT
	
	glClientActiveTexture(GL_TEXTURE0+texCount);
	glTexCoordPointer(2, GL_FLOAT, stride, buf ); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
	currentElementTypes[genericCount] = VertexFormat::UV;
	++texCount;
	++genericCount;
}

void GeometryDrawer::setNormalPointer( void * buf, int stride )
{
	CM_GLCONTEXT
	
	glNormalPointer(GL_FLOAT, stride, buf ); 
	glEnableClientState(GL_NORMAL_ARRAY); 
	currentElementTypes[genericCount] = VertexFormat::NORMAL;
	++genericCount;
}

void GeometryDrawer::drawArrays( PRIMITIVE prim, int numElements, int startElement )
{
	CM_GLCONTEXT
	
	glDrawArrays(getGLPRIMITIVETYPE(prim), startElement, numElements); 
	
	if( CM_GLERROR )
		debugPrint("Error in GeometryDrawer::drawArrays\n");
}

void GeometryDrawer::drawElements( PRIMITIVE prim, unsigned short * inds, int numElements )
{
	CM_GLCONTEXT
	
	glDrawElements(getGLPRIMITIVETYPE(prim),numElements,GL_UNSIGNED_SHORT,inds);
	
	if( CM_GLERROR )
		debugPrint("Error in GeometryDrawer::drawArrays\n");
}

void GeometryDrawer::drawElements( PRIMITIVE prim, unsigned int * inds, int numElements )
{
	CM_GLCONTEXT
	
	glDrawElements(getGLPRIMITIVETYPE(prim),numElements,GL_UNSIGNED_INT,inds);
	
	if( CM_GLERROR )
		debugPrint("Error in GeometryDrawer::drawArrays\n");
}


}