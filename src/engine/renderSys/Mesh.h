/*
 *  Mesh.h
 *
 *  Created by Daniel Berio on 7/14/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once

#include "engine/engineIncludes.h"
#include "geom/TriMesh.h"

namespace cm
{

//////////////////////////////////////////////////
/// Base IndexArray class to handle both 32 and 16 bit indices transparently

class IndexArray
{
	public:
		virtual void addIndex( int i ) = 0;
		virtual int getIndex( int i ) const = 0;
		virtual void setIndex( int i, int ind ) = 0;
		virtual void * get() = 0;
		virtual int size() const = 0;
		virtual void clear() = 0;
		virtual void resize( int n ) = 0;
		virtual int getIndexSize() const = 0;
		
		void makeGridTriangles( int w, int h );
		void makeConsecutive( int start, int end );
		void makeConsecutive( int n ) { makeConsecutive(0,n); }
		
		
};

template <class INDEXT>
class TIndexArray : public IndexArray
{
public:
	~TIndexArray() 
	{
		indices.clear();
	}
	
	void addIndex( int i ) 
	{
		indices.push_back(i);
	}
	
	int getIndex( int i ) const 
	{
		return indices[i];
	}
	
	void setIndex( int i, int ind )
	{
		indices[i] = ind;
	}
	
	void * get()
	{
		return &indices[0];
	}
	
	int size() const
	{
		return indices.size();
	}
	
	void clear()
	{
		indices.clear();
	}
	
	void resize( int n )
	{
		indices.resize(n);
	}
	
	int getIndexSize() const { return sizeof(INDEXT); }
		
	std::vector<INDEXT> indices;
};

typedef TIndexArray<unsigned short> IndexArray16;
typedef TIndexArray<unsigned int> IndexArray32;


class Mesh : public Base
{

public:
	// These are public for easily copying stuff around with stl
	std::vector <cm::Vec3> vertices;
	std::vector <cm::Vec3> normals;
	std::vector <cm::Vec2> uvs;
	std::vector <unsigned int> colors;
	
	IndexArray *indices;
	
	PRIMITIVE primitive;
	
	enum INDEX_SIZE
	{	
		INDEX_DEFAULT = 0,
		INDEX_16 = 2,
		INDEX_32 = 4
	};

	// TODO: should support quad faces ( and poly ? )
	// IndexArray *quads;
	// IndexArray *triangles;
		
protected:
	unsigned int color;
	bool bColors;
	
	std::vector <Mesh*> children;
	
	Matrix4x4 tm;
	bool transformDirty;
	
	bool bDebugNormals;
public:
	Mesh( const TriMesh & mom );
	Mesh();
	~Mesh();
	
	virtual void release();
	
	void clearIndices();
	void clearVertices();
	
	/// Enabling this sets face colors based on normals
	void enableNormalDebug( bool flag = true ) { bDebugNormals = flag; if(flag) bColors = true; }
	
	int getNumIndices() const { if( !indices ) return 0; return indices->size(); }
	int getNumVertices() const { return vertices.size(); }
	
	IndexArray * getIndices() const { return indices; }
	int getIndexSize() const { if(!indices) return 0; return indices->getIndexSize(); }
	
	/// Get buffer with vertices, can be handy for faster access and manipulation
	cm::Vec3 * getVertices() { if( vertices.empty() ) return 0; return &vertices[0]; }
	/// Get buffer with tex coords
	cm::Vec2 * getTexCoords() { if( uvs.empty() ) return 0; return &uvs[0]; }
	/// Get buffer with colors
	unsigned int * getColors() { if( colors.empty() ) return 0; return &colors[0]; }
	/// Get buffer with normals
	cm::Vec3 * getNormals() { if( normals.empty() ) return 0; return &normals[0]; }
	
	bool hasNormals() const { return normals.size() != 0; }
	bool hasTexCoords() const { return uvs.size() != 0; }
	bool hasColors() const { return colors.size() != 0; }
	
	/// Add vertex
	void vertex( const cm::Vec3 & v );
	
	/// Add normal
	void normal( const cm::Vec3 & n );
	
	/// Add 2d texture coordinate
	void uv( const cm::Vec2 & uv );
	
	/// Set current color, also enables color buffer
	void setColor( unsigned int clr );
	
	/// Add a triangle face
	void triangle( int a, int b, int c );
	void quad( int a, int b, int c, int d );
	
	void index( int i );
	
	void setMatrix( const Matrix4x4 & m ) { tm = m; transformDirty = true; }
	const Matrix4x4 & getMatrix() const { return tm; }
	Matrix4x4 & getMatrix() { transformDirty = true; return tm; }
	
	void create( const std::vector <Vec3> & vts, const std::vector <unsigned int> & inds );
	
	void add( Mesh * mesh ) { children.push_back( mesh ); }
	int getNumChildren() const { return children.size(); }
	Mesh * getChild( int index ) { return children[index]; }
	
	void fromMesh( const TriMesh & mom );
	
	void inCube( const cm::Vec3 & center = cm::Vec3(0,0,0), float size = 1.0, bool bTexCoords = false, bool bNormals = true );
	void inCube( const cm::Vec3 & center, const cm::Vec3 &size, bool bTexCoords = false, bool bNormals = true );
	void extrusion( cm::Vec2 * polygon, cm::Vec3 * points, int numPoints, bool cap = true, bool flipNormals = false );
	void box( const cm::Vec3 & a, const cm::Vec3 & b, bool bTexCoords = false, bool bNormals = true, float texMultiplier = 1.0 );
	void cube( const cm::Vec3 & center = cm::Vec3(0,0,0), float size = 1.0, bool bTexCoords = false, bool bNormals = true, float texMultiplier = 1.0 );
	void cube( const cm::Vec3 & center, const cm::Vec3 &size, bool bTexCoords = false, bool bNormals = true, float texMultiplier = 1.0  );
	void sphere( const cm::Vec3 & center = cm::Vec3(0,0,0) , float radius = 1.0 );
	
	void computeFaceNormals();
	void makeVerticesFromFaces();
	
	void getUniqueIndices( int * inds );
	
	void removeIndices();
	void createIndices( INDEX_SIZE size = INDEX_DEFAULT );
	
	AABox getBoundingBox() const;
	void transform( const cm::M44 & m );
	
	///::::::: This part should be abstracted
	
	Geometry geometry;
	void begin();
	void end( bool vertexFormatChanged = false );
	void draw();
	void drawNormals( float len = 1.0 );
	void updateIndices();
	void updateVertices( bool vertexFormatChanged = false );
protected:
	
	void setNormalColor( const cm::Vec3 & n );
	void createVertexBuffers();
	void updateVertexBuffers();
	void createIndexBuffer();
	void updateIndexBuffer();

	bool beginCalled;

	
};


}