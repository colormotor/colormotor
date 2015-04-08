

#include "gfx/cmGLInternal.h"
#include "gfx/cmVertexFormat.h"

namespace cm
{


static const char *elementNames[VertexFormat::NUMVERTEXELEMENTS] = 
{
	"POS3",
	"POS4",
	"UV",
	"UVW",
	"COLOR",
	"NORMAL",
	
	"FLOAT",
	"FLOAT2", 
	"FLOAT3",
	"FLOAT4",
	
	"BYTE",
	"BYTE2", 
	"BYTE3",
	"BYTE4",
	
};

int	VertexFormat::elementSizes[VertexFormat::NUMVERTEXELEMENTS] = 
{	
	12, // POS3
	16, // POS4
	8,  // UV
	12, // UVW
	4,  // COLOR
	12, // NORMAL
	
	4, // FLOAT 
	8, // FLOAT2
	12,  // FLOAT3
	16, // FLOAT4
	
	1, // BYTE
	2, // BYTE2
	3, // BYTE3
	4, // BYTE4

};


GLint	VertexFormat::elementNumComponents[VertexFormat::NUMVERTEXELEMENTS] = 
{	
	3, // POS3
	4, // POS4
	2,  // UV
	3, // UVW
	4,  // COLOR
	3, // NORMAL
	
	1, // FLOAT 
	2, // FLOAT2
	3,  // FLOAT3
	4, // FLOAT4
	
	1, // BYTE
	2, // BYTE2
	3, // BYTE3
	4, // BYTE4
};

GLenum	VertexFormat::glElementTypes[VertexFormat::NUMVERTEXELEMENTS] = 
{	
	GL_FLOAT, // POS3
	GL_FLOAT, // POS4
	GL_FLOAT,  // UV
	GL_FLOAT, // UVW
	GL_UNSIGNED_BYTE,  // COLOR
	GL_FLOAT, // NORMAL
	
	GL_FLOAT, // FLOAT 
	GL_FLOAT, // FLOAT2
	GL_FLOAT,  // FLOAT3
	GL_FLOAT, // FLOAT4
	
	GL_UNSIGNED_BYTE, // BYTE
	GL_UNSIGNED_BYTE, // BYTE2
	GL_UNSIGNED_BYTE, // BYTE3
	GL_UNSIGNED_BYTE, // BYTE4
};



static VertexFormat::ELEMENT_TYPE elementTypeFromString( const char * str )
{

	for( int i = 0; i < VertexFormat::NUMVERTEXELEMENTS; i++ )
		if( strcmp(elementNames[i],str) == 0)
		{
			return (VertexFormat::ELEMENT_TYPE)i;
		}

	return VertexFormat::UNKNOWN_ELEMENT;
}

//////////////

VertexFormat::VertexFormat()// u16 streamIndex /*= 0 */ )
{
	release();
	_format = "";
}

VertexFormat::VertexFormat( const char * str )// u16 streamIndex /*= 0 */ )
{
	release();
	_format = "";
	create(str);
}



VertexFormat::VertexFormat( const VertexFormat & fmt )
{
	release();
	for( int i = 0; i < fmt.getNumElements(); i++ )
	{
		addElement(fmt.getElement(i));
	}
}


VertexFormat::~VertexFormat()
{
	release();
}


void VertexFormat::release()
{
	_elements.clear();
	for( int i = 0; i < NUMVERTEXELEMENTS; i++ )
		_elementCount[i] = 0;
	_size = 0;
}


bool VertexFormat::create( const char * str )
{
	release();
	
	char * c = (char*)str;
	char token[128];

	_format = "";

	while( *c != '\0' ){
		int ind = 0;	

		//skip whitespace or other
		while( !isalnum(*c) )
			c++;

		while( isalnum(*c) )
		{
			token[ind++] = *c++;
		}
		token[ind] = '\0';

		ELEMENT_TYPE elem = elementTypeFromString(token);

		if(elem == UNKNOWN_ELEMENT)
			return false;

		addElement(elem);

		while( !isalnum(*c) && *c != '\0')
			c++; //very object oriented

	}

	return true;
}


void VertexFormat::addElement( const VertexFormat::Element &ielem )
{
	VertexFormat::Element elem = ielem;

	elem.offset = getSize();
	elem.size = VertexFormat::elementSizes[elem.type];
	elem.glType = VertexFormat::glElementTypes[elem.type];
	elem.numComponents = VertexFormat::elementNumComponents[elem.type];
	
	_size+=elem.size;

	_elements.push_back(elem);

	_elementCount[elem.type]++;
	
	// add element to string desc
	_format += elementNames[elem.type];
	_format += " ";
}


VertexWriter::VertexWriter( VertexFormat * fmt, void * buf)
{
	init(fmt,buf);
}


void VertexWriter::init( VertexFormat * fmt, void * buf)
{
	this->format = fmt;
	
	buffer = (char*)buf;
	count = 0;
	
	for( int i = 0; i < VertexFormat::NUMVERTEXELEMENTS; i++ )
		for( int j = 0; j < MAX_ELEMENT_COUNT; j++ )
			indexTable[i][j] = -1;
			
	for( int i = 0; i < fmt->getNumElements(); i++ )
	{
		VertexFormat::ELEMENT_TYPE e = fmt->getElementType(i);
		int j = 0;
		while( indexTable[e][j] != -1 )
			j++;
		indexTable[e][j] = i;
	}
	
	stride = fmt->getSize();
}


void	VertexWriter::position( float x, float y, float z )
{
	float * buf = (float*)(buffer+count*stride);
	buf[0] = x;
	buf[1] = y;
	buf[2] = z;
	++count;
}


void	VertexWriter::uv( float u, float v, int index )
{
	int c = count - 1;
	int i = indexTable[VertexFormat::UV][index];
	if( i == -1 )
		return;

	const VertexFormat::Element & e = format->getElement(i);
	float * buf = (float*)(buffer+c*stride+e.offset);
	buf[0] = u;
	buf[1] = v;
}


void	VertexWriter::uvw( float u, float v, float w, int index  )
{
	int c = count - 1;
	int i = indexTable[VertexFormat::UVW][index];
	if( i == -1 )
		return;
		
	const VertexFormat::Element & e = format->getElement(i);
	float * buf = (float*)(buffer+c*stride+e.offset);
	buf[0] = u;
	buf[1] = v;
	buf[2] = w;
}


void	VertexWriter::color( unsigned int clr )
{
	int c = count - 1;
	int i = indexTable[VertexFormat::COLOR][0];
	if( i == -1 )
		return;

	const VertexFormat::Element & e = format->getElement(i);
	unsigned int * buf = (unsigned int*)(buffer+c*stride+e.offset);
	buf[0] = clr;
}


void	VertexWriter::normal( float x, float y, float z )
{
	int c = count - 1;
	int i = indexTable[VertexFormat::NORMAL][0];
	if( i == -1 )
		return;

	const VertexFormat::Element & e = format->getElement(i);
	float * buf = (float*) (buffer+c*stride+e.offset);
	buf[0] = x;
	buf[1] = y;
	buf[2] = z;
}


}
