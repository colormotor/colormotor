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
 

#include "gfx/cmGLInternal.h"
#include "gfx/cmQuad.h"

namespace cm
{

Quad::Quad()
{	
	numTexCoords = 0;
}

Quad::Quad(float maxU,float maxV)
{
	numTexCoords = 1;
	setMinCoords(0,0,0);
	setMaxCoords(0,maxU,maxV);
}
	
void Quad::setNumTexCoords( int n )
{
	numTexCoords = n;
	for( int i = 0; i < numTexCoords; i++ )
	{
		setMinCoords(i,0,0);
		setMaxCoords(i,1,1);
	}
}

void Quad::setMaxCoords(int coordIndex, float maxU, float maxV)
{
	maxCoords[coordIndex][0] = maxU;
	maxCoords[coordIndex][1] = maxV;	
}

void Quad::setMinCoords(int coordIndex, float minU, float minV)
{
	minCoords[coordIndex][0] = minU;
	minCoords[coordIndex][1] = minV;
}

void Quad::draw( float x, float y, float w, float h, bool flip )
{	
	CM_GLCONTEXT
	
	
	for( int i = 0; i < numTexCoords; i++ )
	{
	
		if(flip)
		{
			texCoords[i][0] = minCoords[i][0]; texCoords[i][1] = maxCoords[i][1]; 
			texCoords[i][2] = minCoords[i][0]; texCoords[i][3] = minCoords[i][1]; 
			texCoords[i][4] = maxCoords[i][0]; texCoords[i][5] = minCoords[i][1];
			texCoords[i][6] = maxCoords[i][0]; texCoords[i][7] = maxCoords[i][1]; 
		}
		else 
		{
			texCoords[i][0] = minCoords[i][0]; texCoords[i][1] = minCoords[i][1]; 
			texCoords[i][2] = minCoords[i][0]; texCoords[i][3] = maxCoords[i][1]; 
			texCoords[i][4] = maxCoords[i][0]; texCoords[i][5] = maxCoords[i][1]; 
			texCoords[i][6] = maxCoords[i][0]; texCoords[i][7] = minCoords[i][1]; 
		}
	
	}
			
		
	GLfloat verts[] = {
		x,y,
		x,y+h,
		x+w,y+h,
		x+w,y
	};


	
	for( int i = 0; i < numTexCoords; i++ )
	{
		glClientActiveTexture( GL_TEXTURE0 + i );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords[i] );
	}
	
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT, 0, verts );
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	
	for( int i = 0; i < numTexCoords; i++ )
	{
		glClientActiveTexture( GL_TEXTURE0 + i );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}
	
	glClientActiveTexture( GL_TEXTURE0 );
}


}