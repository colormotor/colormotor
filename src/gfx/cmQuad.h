

#pragma once 

#include "gfx/cmGfxIncludes.h"

namespace cm
{



class Quad
{
public:
	enum {
		MAX_TEXCOORDS = 4
	};
	
	Quad();
	Quad(float maxU,float maxV);
	
	/// Set number of texture coordinates 
	/// resets min and max coords to defaults (0 1)
	void setNumTexCoords( int n );
	
	void setMaxCoords(int coordIndex, float maxU, float maxV);
	void setMinCoords(int coordIndex, float minU, float minV);
	
	void draw( float x=-1.0f, float y=-1.0f, float w=2.0f, float h=2.0f, bool flip = false );
	
	float maxCoords[MAX_TEXCOORDS][2];
	float minCoords[MAX_TEXCOORDS][2];
	
private:
	int	  numTexCoords;
	float texCoords[MAX_TEXCOORDS][8];
};

}
