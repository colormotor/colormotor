

#pragma once 

#include "gfx/cmGfxIncludes.h"
#include "gfx/cmVertexBuffer.h"
#include "gfx/cmIndexBuffer.h"
#include "gfx/cmGfxUtils.h"


/*
 *  cmGeometry.h
 *  resapiTest
 *
 *  Created by Daniel Berio on 5/4/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */

#pragma once 

#include "gfx/cmGfxUtils.h"
#include "gfx/cmVertexBuffer.h"
#include "gfx/cmIndexBuffer.h"

namespace cm
{

#define MAX_VERTEX_STREAMS 8

/// TODO 
/// use a container for verte buffer that keeps track of generic and texture attribute indices
/// also use method for activating and deactivating streams

/// Geometry
/// Contains one or more vertex buffers and an (optional) index buffer
/// 
class Geometry : public GfxObject
{
public:
	Geometry();
	~Geometry();
	
	void release( bool deleteVertexBuffers = true, bool deleteIndexBuffer = true );
	void clearVertexBuffers();
	void clearIndexBuffer();
	
	/// Adds a vertex buffer with specified format, filled with user data
	VertexBuffer * addVertexBufferWithData( const char * fmt, void * data, int numVertices,  HardwareBuffer::USAGE usage = HardwareBuffer::USAGE_DEFAULT );
	/// Adds a vertex buffer with specified format
	VertexBuffer * addVertexBuffer( const char * fmt, int numVertices, HardwareBuffer::USAGE usage = HardwareBuffer::USAGE_DEFAULT );
	
	/// updates a vertex buffer 
	/// index is the index of vertex buffer in geometry ( in order of creation )
	bool		updateVertexBuffer( int index, void * data, int count, int start = 0 );
	
	/// updates index buffer
	bool		updateIndexBuffer( void * data, int count, int start = 0 );
	
	/// Creates an index buffer , filled with user data
	IndexBuffer * createIndexBufferWithData( void * data, int mumIndices, IndexBuffer::TYPE indexType = IndexBuffer::INDEX_16,HardwareBuffer::USAGE usage = HardwareBuffer::USAGE_DEFAULT );
	IndexBuffer * createIndexBuffer( int numIndices, IndexBuffer::TYPE indexType = IndexBuffer::INDEX_16,HardwareBuffer::USAGE usage = HardwareBuffer::USAGE_DEFAULT );
	void setIndexBuffer( IndexBuffer * ib ) { _ib = ib; }
	
	VertexBuffer * getVertexBuffer( int index ) { if( index >= _vbs.size() ) return 0; return _vbs[index]; }
	IndexBuffer * getIndexBuffer() { return _ib; }
	
	int	getNumVertexBuffers() const { return _vbs.size(); }
	
	bool bind( int startVertex = 0 );
	//bool bind( VertexFormat * fmt, int startVertex = 0 );
	
	bool unbind();
	static bool resetAll();
	
	bool draw( PRIMITIVE prim, int numElements, int startVertex = 0 );
	bool drawIndexed( PRIMITIVE prim, int numElements, int startIndex = 0 );
	
	// must be able to draw with specific vertex format i.e when we are rendering just to stencil buffer or depth
	//bool draw( VertexFormat * fmt, PRIMITIVE prim, int numElements, int startVertex = 0 );
	//bool drawIndexed( VertexFormat * fmt, PRIMITIVE prim, int numElements, int startVertex = 0, int startIndex = 0 );
	
	void	sphere( const char * format, float radius, unsigned int clr = 0xFFFFFFFF, int latitudeSubd = 32, int longitudeSubd = 32);
	// static void drawArraysIndexed( PRIMITIVE prim, void * indices, int numElements, int startElement );
	
protected:
	std::vector <VertexBuffer*> _vbs;
	RefPtr<IndexBuffer> _ib;
	
	int	_maxElements;
	
	//bool _bound;
	
};

}

