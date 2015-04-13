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

namespace cm
{

class VertexFormat
{
public:
	/// vertex element types
	enum  ELEMENT_TYPE{
		POS3 = 0,
		POS4,
		UV,
		UVW,
		COLOR,
		NORMAL,
		
		
		// generic attributes for GL2.0 and GLES2.0 or extra attributes in previous versions
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		
		BYTE,
		BYTE2,
		BYTE3,
		BYTE4,

		
		NUMVERTEXELEMENTS,
		
		/// used for skipping data in streams
		OFFSET_FLOAT,
		OFFSET_FLOAT2,
		OFFSET_FLOAT3,
		OFFSET_FLOAT4,

		UNKNOWN_ELEMENT
	};


	/// Vertex element structure
	struct  Element
	{
		Element()
		{

		}

		Element( ELEMENT_TYPE type )
		:
		type(type)
		{
		}


		ELEMENT_TYPE		type;
		int					offset;
		int					size;
		
		GLenum				glType;
		int					numComponents;
		
	};


	VertexFormat();
	VertexFormat( const char * str );
	VertexFormat( const VertexFormat & fmt );

	~VertexFormat();
	

	///   Clear all local data
	void			release();

	///   Adds an VertexElementDesc given a VERTEXELEMENT enum
	void			addElement( ELEMENT_TYPE elem ) 
	{
		addElement(Element(elem));
	}

	///   Adds existing Element to list
	void			addElement( const Element &elem );

	///   create with string descriptor
	bool			create( const char * str );

	///   Get element at index
	Element &		getElement( int i ) { return _elements[i]; }

	///   Get element at index ( const ref )
	const Element &	 getElement( int i ) const { return _elements[i]; }

	///   Get element at index ( const ref )
	ELEMENT_TYPE getElementType( int i ) const { return _elements[i].type; }


	///   Get num elements in element list
	int			getNumElements() const { return _elements.size(); }

	///   get size in bytes
	int			getSize() const  { return _size; }

	///   Get string representation of Stream
	const char		*toString() const { return _format.c_str(); }

	/// Get number of elements of specified type
	int			getElementCount( ELEMENT_TYPE elem ) const { return _elementCount[elem]; }
	

protected:

	
	void			incSize( int bytes ) { _size+=bytes; }
protected:

	/// separate uv count since uv and uvw are always texcoords.
	int				  _uvCount; 


	std::vector <Element> _elements;
	
	int				  _elementCount[NUMVERTEXELEMENTS];
	
	int				  _size;		/// Size of Stream

	std::string		  _format;

	
	static int		elementSizes[NUMVERTEXELEMENTS];
	static GLint	elementNumComponents[NUMVERTEXELEMENTS];
	static GLenum	glElementTypes[NUMVERTEXELEMENTS];
	


};

/// Utility class to write components to a buffer
class VertexWriter
{
	public:
		VertexWriter( VertexFormat * fmt, void * buf );
		void	init( VertexFormat * fmt, void * buf );
		
		void	position( float x, float y, float z );
		void	uv( float u, float v, int index = 0 );
		void	uvw( float u, float v, float w, int index = 0 );
		void	color( unsigned int clr );
		void	normal( float x, float y, float z );
		
		enum 
		{	
			MAX_ELEMENT_COUNT = 5
		};
		
		int	indexTable[VertexFormat::NUMVERTEXELEMENTS][MAX_ELEMENT_COUNT];
		VertexFormat * format;
		char * buffer;
		int stride;
		int count;
};

}