/*
 *  Mesh.cpp
 *
 *  Created by Daniel Berio on 7/14/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */


#include "Mesh.h"

namespace cm
{


void IndexArray::makeGridTriangles( int w, int h )
{
	clear();
	resize((w-1)*(h-1)*2);
	
	// init indices for triangle list
	int i = 0;
	for( int y = 0; y < h-1; y++ )
	{
		int v = y*w;
		for( int x = 0; x < w-1; x++ )
		{
			// ccw from cur vertex 
			int a = v;
			int b = v+w;
			int c = v+1+w;
			int d = v+1;
			
			setIndex( i++, a );
			setIndex( i++, d );
			setIndex( i++, b );
			setIndex( i++, b );
			setIndex( i++, d );
			setIndex( i++, c );
			
			v++;
		}
	}
}



void IndexArray::makeConsecutive( int start, int end )
{
	clear();
	for( int i = start; i < end; i++ )
		addIndex(i);
}

//////////////////////////////////////////////////////////////


void Mesh::setNormalColor( const cm::Vec3 & n )
{
	//float l = (n.x*0.5 + 0.5) + (n.y*0.5 + 0.5) + (n.z*0.5 + 0.5);
	//l*=0.33333333333333;
	float r = n.x*0.5 + 0.5;
	float g = n.y*0.5 + 0.5;
	float b = n.z*0.5 + 0.5;
	colors[ normals.size() ] = GL_MAKERGBA( r,g,b,1.0f ); //l,l,l,1.0f );//
}

Mesh::Mesh( const TriMesh & mom )
{
	beginCalled = false;
	begin();
	if(!indices)
		createIndices(INDEX_32);
	const std::vector<unsigned int> inds = mom.getIndices();
	for( int i = 0; i < inds.size(); i++ )
		indices->addIndex(inds[i]);
	vertices = mom.getVertices();
	const std::vector<Color> & clrs = mom.getColorsRGBA(); 
	if(clrs.size())
	{
		for(int i = 0; i < clrs.size(); i++)
		{
			const Color & clr = clrs[i];
			colors.push_back(GL_MAKERGBA(clr.r,clr.g,clr.b,clr.a));
		}
	}

	computeFaceNormals();
	end();

	//create( mom.getVertices(), mom.getIndices() ); 
}

Mesh::Mesh()
:
	bDebugNormals(false),
	transformDirty(false),
	bColors(false),
	primitive(TRIANGLELIST),
	indices(0)
{
	beginCalled = false;
}

Mesh::~Mesh()
{
	release();
}

void Mesh::release()
{
	clearIndices();
	clearVertices();
	geometry.release();
}

void Mesh::clearIndices()
{
	if(indices)
		indices->clear();
}

void Mesh::clearVertices()
{
	vertices.clear();
	colors.clear();
	uvs.clear();
	normals.clear();
}

/// Add vertex
void Mesh::vertex( const cm::Vec3 & v ) 
{ 
	vertices.push_back(v); 
	if( bColors ) 
		colors.push_back(color); 
}

/// Add normal
void Mesh::normal( const cm::Vec3 & n ) 
{ 
	if( bDebugNormals ) 
		setNormalColor( n ); 
	
	normals.push_back( n );  
}

/// Add 2d texture coordinate
void Mesh::uv( const cm::Vec2 & uv ) 
{ 
	uvs.push_back(uv); 
}

/// Set current color, also enables color buffer
void Mesh::setColor( unsigned int clr ) 
{ 
	color = clr; 
	bColors = true; 
}

void Mesh::index( int i )
{
	if(!indices)
		createIndices();
		
	indices->addIndex(i);
}

/// Add a quad face
void Mesh::quad( int a, int b, int c, int d ) 
{
	primitive = QUADS;
	
	if(!indices)
		createIndices();
		
	indices->addIndex(a);
	indices->addIndex(b);
	indices->addIndex(c);
	indices->addIndex(d);
}

/// Add a triangle face
void Mesh::triangle( int a, int b, int c ) 
{
	primitive = TRIANGLELIST;
	
	if(!indices)
		createIndices();
		
	indices->addIndex(a);
	indices->addIndex(b);
	indices->addIndex(c);
}

void Mesh::fromMesh( const TriMesh & mom )
{
	//beginCalled = false;
	begin();
	if(!indices)
		createIndices(INDEX_32);
	const std::vector<unsigned int> inds = mom.getIndices();
	
	if(!inds.size())
	{
		for( int i = 0; i < mom.getNumVertices(); i++)
			indices->addIndex(i);	
	}
	else
	{
		for( int i = 0; i < inds.size(); i++ )
			indices->addIndex(inds[i]);
	}
	
	vertices = mom.getVertices();
	const std::vector<Color> & clrs = mom.getColorsRGBA(); 
	if(clrs.size())
	{
		for(int i = 0; i < clrs.size(); i++)
		{
			const Color & clr = clrs[i];
			colors.push_back(GL_MAKERGBA(clr.r,clr.g,clr.b,clr.a));
		}
	}

	computeFaceNormals();
	end();

	//create( mom.getVertices(), mom.getIndices() ); 
}

//////////////////////////////////////////////////////////////
	
void Mesh::cube( const cm::Vec3 & center , float size, bool bTexCoords, bool bNormals, float texMultiplier )
{
	cube( center, cm::Vec3(size,size,size), bTexCoords, bNormals, texMultiplier );
}

//////////////////////////////////////////////////////////////


void Mesh::cube( const cm::Vec3 & center, const cm::Vec3 & size, bool bTexCoords, bool bNormals, float texMultiplier )
{
	cm::Vec3 verts[24] = 
	{
		 cm::Vec3( -1.0f, -1.0f,  1.0 ),// FRONT
		 cm::Vec3(  1.0f, -1.0f,  1.0 ),
		 cm::Vec3(  1.0f,  1.0f,  1.0 ),
		 cm::Vec3( -1.0f,  1.0f,  1.0 ),
	   
		 cm::Vec3( -1.0f, -1.0f, -1.0 ), // BACK
		 cm::Vec3( -1.0f,  1.0f, -1.0 ),
		 cm::Vec3(  1.0f,  1.0f, -1.0 ),
		 cm::Vec3(  1.0f, -1.0f, -1.0 ),
	   
		 cm::Vec3( -1.0f,  1.0f, -1.0 ), // TOP
		 cm::Vec3( -1.0f,  1.0f,  1.0 ),
		 cm::Vec3(  1.0f,  1.0f,  1.0 ),
		 cm::Vec3(  1.0f,  1.0f, -1.0 ),
	   
		 cm::Vec3( -1.0f, -1.0f, -1.0 ), // BOTTOM
		 cm::Vec3(  1.0f, -1.0f, -1.0 ),
		 cm::Vec3(  1.0f, -1.0f,  1.0 ),
		 cm::Vec3( -1.0f, -1.0f,  1.0 ),
	    
		 cm::Vec3(  1.0f, -1.0f, -1.0 ), // RIGHT
		 cm::Vec3(  1.0f,  1.0f, -1.0 ),
		 cm::Vec3(  1.0f,  1.0f,  1.0 ),
		 cm::Vec3(  1.0f, -1.0f,  1.0 ),
	   
		 cm::Vec3( -1.0f, -1.0f, -1.0 ), // LEFT
		 cm::Vec3( -1.0f, -1.0f,  1.0 ),
		 cm::Vec3( -1.0f,  1.0f,  1.0 ),
		 cm::Vec3( -1.0f,  1.0f, -1.0 )
	};


	static cm::Vec3 n[24] = 
	{
		cm::Vec3( 0.0,0.0,1.0 ),
		cm::Vec3( 0.0,0.0,1.0 ),
		cm::Vec3( 0.0,0.0,1.0 ),
		cm::Vec3( 0.0,0.0,1.0 ),
		
		cm::Vec3( 0.0,0.0,-1.0 ), 
		cm::Vec3( 0.0,0.0,-1.0 ),
		cm::Vec3( 0.0,0.0,-1.0 ),
		cm::Vec3( 0.0,0.0,-1.0 ),
		
		cm::Vec3( 0.0,1.0,0.0 ),
		cm::Vec3( 0.0,1.0,0.0 ),
		cm::Vec3( 0.0,1.0,0.0 ),
		cm::Vec3( 0.0,1.0,0.0 ),
		
		cm::Vec3( 0.0,-1.0,0.0 ),
		cm::Vec3( 0.0,-1.0,0.0 ),
		cm::Vec3( 0.0,-1.0,0.0 ),
		cm::Vec3( 0.0,-1.0,0.0 ),
		
		cm::Vec3( 1.0,0.0,0.0 ),
		cm::Vec3( 1.0,0.0,0.0 ),
		cm::Vec3( 1.0,0.0,0.0 ),
		cm::Vec3( 1.0,0.0,0.0 ),
		
		cm::Vec3( -1.0,0.0,0.0 ),
		cm::Vec3( -1.0,0.0,0.0 ),
		cm::Vec3( -1.0,0.0,0.0 ),
		cm::Vec3( -1.0,0.0,0.0 )
	};

	static cm::Vec2 tex[24] = 
	{
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier
		
	};

	static unsigned short inds[6*6] = { 0, 2, 3, 0, 1, 2, 
											4, 6, 7, 4, 5, 6, 
											8, 10, 11, 8, 9, 10, 
											12, 14, 15, 12, 13, 14, 
											16, 18, 19, 16, 17, 18, 
											20, 22, 23, 20, 21, 22 };
			

	for( int i = 0; i < 36; i++ )
	{
		int c = inds[i];
		
		vertex( center + verts[c]*size );

		if( bTexCoords )
		{
			uv( tex[c] );
		}
		
		if( bNormals )
		{
			normal( n[c] );
		}
	}

	// if we are not in a begin/end pair update the vertices directly
	// we assume that this is the only geometry we want to add
	if(!beginCalled)
		end();
	
}

void Mesh::box( const cm::Vec3 & min, const cm::Vec3 & max, bool bTexCoords, bool bNormals, float texMultiplier )
{
	cm::Vec3 verts[8] = 
	{
		cm::Vec3( min.x, max.y, max.z ),
		cm::Vec3( min.x, min.y, max.z ),
		cm::Vec3( max.x, min.y, max.z ),
		cm::Vec3( max.x, max.y, max.z ),
		
		cm::Vec3( min.x, max.y, min.z ),
		cm::Vec3( min.x, min.y, min.z ),
		cm::Vec3( max.x, min.y, min.z ),
		cm::Vec3( max.x, max.y, min.z ),
	};
	
	static unsigned short vi[6*6] = { 0,1,2,0,2,3, //front
									  4,6,5,4,7,6, // back
									  4,0,3,4,3,7, // top
									  1,5,6,1,6,2, // bottom
									  3,2,6,3,6,7, // right
									  0,5,1,0,4,5 }; // left
									  
		


	static cm::Vec3 n[24] = 
	{
		cm::Vec3( 0.0,0.0,1.0 ),
		cm::Vec3( 0.0,0.0,1.0 ),
		cm::Vec3( 0.0,0.0,1.0 ),
		cm::Vec3( 0.0,0.0,1.0 ),
		
		cm::Vec3( 0.0,0.0,-1.0 ), 
		cm::Vec3( 0.0,0.0,-1.0 ),
		cm::Vec3( 0.0,0.0,-1.0 ),
		cm::Vec3( 0.0,0.0,-1.0 ),
		
		cm::Vec3( 0.0,1.0,0.0 ),
		cm::Vec3( 0.0,1.0,0.0 ),
		cm::Vec3( 0.0,1.0,0.0 ),
		cm::Vec3( 0.0,1.0,0.0 ),
		
		cm::Vec3( 0.0,-1.0,0.0 ),
		cm::Vec3( 0.0,-1.0,0.0 ),
		cm::Vec3( 0.0,-1.0,0.0 ),
		cm::Vec3( 0.0,-1.0,0.0 ),
		
		cm::Vec3( 1.0,0.0,0.0 ),
		cm::Vec3( 1.0,0.0,0.0 ),
		cm::Vec3( 1.0,0.0,0.0 ),
		cm::Vec3( 1.0,0.0,0.0 ),
		
		cm::Vec3( -1.0,0.0,0.0 ),
		cm::Vec3( -1.0,0.0,0.0 ),
		cm::Vec3( -1.0,0.0,0.0 ),
		cm::Vec3( -1.0,0.0,0.0 )
	};

	static cm::Vec2 tex[24] = 
	{
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier,
		
		cm::Vec2( 0.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,0.0 )*texMultiplier,
		cm::Vec2( 1.0,1.0 )*texMultiplier,
		cm::Vec2( 0.0,1.0 )*texMultiplier
		
	};

	static unsigned short inds[6*6] = { 0, 2, 3, 0, 1, 2, 
											4, 6, 7, 4, 5, 6, 
											8, 10, 11, 8, 9, 10, 
											12, 14, 15, 12, 13, 14, 
											16, 18, 19, 16, 17, 18, 
											20, 22, 23, 20, 21, 22 };
			
	for( int i = 0; i < 36; i++ )
	{
		int c = inds[i];
		int vc = vi[i];
		
		vertex( verts[vc] );

		if( bTexCoords )
		{
			uv( tex[c] );
		}
		
		if( bNormals )
		{
			normal( n[c] );
		}
	}

	// if we are not in a begin/end pair update the vertices directly
	// we assume that this is the only geometry we want to add
	if(!beginCalled)
		end();
}


//////////////////////////////////////////////////////////////

	
void Mesh::inCube( const cm::Vec3 & center , float size, bool bTexCoords, bool bNormals )
{
	cube( center, cm::Vec3(size,size,size), bTexCoords, bNormals );
	
}

//////////////////////////////////////////////////////////////


void Mesh::inCube( const cm::Vec3 & center, const cm::Vec3 & size, bool bTexCoords, bool bNormals )
{
	cm::Vec3 verts[24] = 
	{
		 cm::Vec3( -1.0f, -1.0f,  1.0 ),// FRONT
		 cm::Vec3(  1.0f, -1.0f,  1.0 ),
		 cm::Vec3(  1.0f,  1.0f,  1.0 ),
		 cm::Vec3( -1.0f,  1.0f,  1.0 ),
	   
		 cm::Vec3( -1.0f, -1.0f, -1.0 ), // BACK
		 cm::Vec3( -1.0f,  1.0f, -1.0 ),
		 cm::Vec3(  1.0f,  1.0f, -1.0 ),
		 cm::Vec3(  1.0f, -1.0f, -1.0 ),
	   
		 cm::Vec3( -1.0f,  1.0f, -1.0 ), // TOP
		 cm::Vec3( -1.0f,  1.0f,  1.0 ),
		 cm::Vec3(  1.0f,  1.0f,  1.0 ),
		 cm::Vec3(  1.0f,  1.0f, -1.0 ),
	   
		 cm::Vec3( -1.0f, -1.0f, -1.0 ), // BOTTOM
		 cm::Vec3(  1.0f, -1.0f, -1.0 ),
		 cm::Vec3(  1.0f, -1.0f,  1.0 ),
		 cm::Vec3( -1.0f, -1.0f,  1.0 ),
	    
		 cm::Vec3(  1.0f, -1.0f, -1.0 ), // RIGHT
		 cm::Vec3(  1.0f,  1.0f, -1.0 ),
		 cm::Vec3(  1.0f,  1.0f,  1.0 ),
		 cm::Vec3(  1.0f, -1.0f,  1.0 ),
	   
		 cm::Vec3( -1.0f, -1.0f, -1.0 ), // LEFT
		 cm::Vec3( -1.0f, -1.0f,  1.0 ),
		 cm::Vec3( -1.0f,  1.0f,  1.0 ),
		 cm::Vec3( -1.0f,  1.0f, -1.0 )
	};


	static cm::Vec3 n[24] = 
	{
		cm::Vec3( 0.0,0.0,1.0 ),
		cm::Vec3( 0.0,0.0,1.0 ),
		cm::Vec3( 0.0,0.0,1.0 ),
		cm::Vec3( 0.0,0.0,1.0 ),
		
		cm::Vec3( 0.0,0.0,-1.0 ), 
		cm::Vec3( 0.0,0.0,-1.0 ),
		cm::Vec3( 0.0,0.0,-1.0 ),
		cm::Vec3( 0.0,0.0,-1.0 ),
		
		cm::Vec3( 0.0,1.0,0.0 ),
		cm::Vec3( 0.0,1.0,0.0 ),
		cm::Vec3( 0.0,1.0,0.0 ),
		cm::Vec3( 0.0,1.0,0.0 ),
		
		cm::Vec3( 0.0,-1.0,0.0 ),
		cm::Vec3( 0.0,-1.0,0.0 ),
		cm::Vec3( 0.0,-1.0,0.0 ),
		cm::Vec3( 0.0,-1.0,0.0 ),
		
		cm::Vec3( 1.0,0.0,0.0 ),
		cm::Vec3( 1.0,0.0,0.0 ),
		cm::Vec3( 1.0,0.0,0.0 ),
		cm::Vec3( 1.0,0.0,0.0 ),
		
		cm::Vec3( -1.0,0.0,0.0 ),
		cm::Vec3( -1.0,0.0,0.0 ),
		cm::Vec3( -1.0,0.0,0.0 ),
		cm::Vec3( -1.0,0.0,0.0 )
	};

	static cm::Vec2 tex[24] = 
	{
		cm::Vec2( 0.0,0.0 ),
		cm::Vec2( 1.0,0.0 ),
		cm::Vec2( 1.0,1.0 ),
		cm::Vec2( 0.0,1.0 ),
		
		cm::Vec2( 0.0,0.0 ),
		cm::Vec2( 1.0,0.0 ),
		cm::Vec2( 1.0,1.0 ),
		cm::Vec2( 0.0,1.0 ),
		
		cm::Vec2( 0.0,0.0 ),
		cm::Vec2( 1.0,0.0 ),
		cm::Vec2( 1.0,1.0 ),
		cm::Vec2( 0.0,1.0 ),
		
		cm::Vec2( 0.0,0.0 ),
		cm::Vec2( 1.0,0.0 ),
		cm::Vec2( 1.0,1.0 ),
		cm::Vec2( 0.0,1.0 ),
		
		cm::Vec2( 0.0,0.0 ),
		cm::Vec2( 1.0,0.0 ),
		cm::Vec2( 1.0,1.0 ),
		cm::Vec2( 0.0,1.0 ),
		
		cm::Vec2( 0.0,0.0 ),
		cm::Vec2( 1.0,0.0 ),
		cm::Vec2( 1.0,1.0 ),
		cm::Vec2( 0.0,1.0 )
		
	};

	static unsigned short inds[6*6] = { 0, 2, 3, 0, 1, 2, 
											4, 6, 7, 4, 5, 6, 
											8, 10, 11, 8, 9, 10, 
											12, 14, 15, 12, 13, 14, 
											16, 18, 19, 16, 17, 18, 
											20, 22, 23, 20, 21, 22 };
			
	for( int i = 35; i >=0; i-- )
	{
		int c = inds[i];
		
		vertex( center + verts[c]*size );

		if( bTexCoords )
		{
			uv( tex[c] );
		}
		
		if( bNormals )
		{
			normal( n[c] );
		}
	}

	// if we are not in a begin/end pair update the vertices directly
	// we assume that this is the only geometry we want to add
	if(!beginCalled)
		end();
}


//////////////////////////////////////////////////////////////


void Mesh::sphere( const cm::Vec3 & center , float radius )
{
	assert(false);
}

//////////////////////////////////////////////////////////////

void Mesh::begin( )
{
	clearIndices();
	clearVertices();
	
	tm.identity();
	transformDirty = false;

	beginCalled = true;
}

//////////////////////////////////////////////////////////////

	
void Mesh::end( bool vertexFormatChanged )
{
	updateIndices();
	updateVertices(vertexFormatChanged);

	beginCalled = false;
}

//////////////////////////////////////////////////////////////

void Mesh::updateIndices()
{
	// indices first
	if( getNumIndices() )
	{
		IndexBuffer * ib = geometry.getIndexBuffer();
		if( !ib || ib->getSize() < getNumIndices() )
			createIndexBuffer();
		else
			updateIndexBuffer();
	}
}

//////////////////////////////////////////////////////////////

void Mesh::updateVertices( bool vertexFormatChanged )
{
	VertexBuffer * vb = geometry.getVertexBuffer(0);
	if( !vb 
		|| vertexFormatChanged 
		|| getNumVertices() > vb->getSize() )
		createVertexBuffers();
	else 
		updateVertexBuffers();

}

//////////////////////////////////////////////////////////////

void Mesh::create( const std::vector <Vec3> & vts, const std::vector <unsigned int> & inds )
{
	begin();
	if(!indices)
		createIndices(INDEX_32);
	for( int i = 0; i < inds.size(); i++ )
		indices->addIndex(inds[i]);
	vertices = vts;
	computeFaceNormals();
	end();
}

//////////////////////////////////////////////////////////////
	
void Mesh::draw()
{
	CM_GLERROR;
	
	geometry.bind();
	if( getNumIndices() )
		geometry.drawIndexed( primitive, getNumIndices() );
	else
		geometry.draw( primitive, getNumVertices() );
	geometry.unbind();
	
	CM_GLERROR;
}

//////////////////////////////////////////////////////////////

	
void Mesh::updateIndexBuffer()
{
	geometry.updateIndexBuffer( indices->get(), getNumIndices() );
}
//////////////////////////////////////////////////////////////

	
void Mesh::updateVertexBuffers()
{
	int ind = 0;
	geometry.updateVertexBuffer( ind++, &vertices[0], vertices.size() );
	if(hasTexCoords())
		geometry.updateVertexBuffer( ind++, &uvs[0], uvs.size() );
	if(hasColors())
		geometry.updateVertexBuffer( ind++, &colors[0], colors.size() );
	if(hasNormals() && !bDebugNormals)
		geometry.updateVertexBuffer( ind++, &normals[0], normals.size() );
}

//////////////////////////////////////////////////////////////

	
void Mesh::createIndexBuffer()
{
	geometry.clearIndexBuffer();
	geometry.createIndexBufferWithData( indices->get(), getNumIndices(), (getIndexSize()==2)?IndexBuffer::INDEX_16:IndexBuffer::INDEX_32);
}
//////////////////////////////////////////////////////////////

	
void Mesh::createVertexBuffers()
{
	geometry.clearVertexBuffers();
	geometry.addVertexBufferWithData( "POS3", &vertices[0], vertices.size() );
	if(hasTexCoords())
		geometry.addVertexBufferWithData( "UV", &uvs[0], uvs.size() );
	if(hasColors())
		geometry.addVertexBufferWithData( "COLOR", &colors[0], colors.size() );
	if(hasNormals() && (!bDebugNormals))
		geometry.addVertexBufferWithData( "NORMAL", &normals[0], normals.size() );
}


//////////////////////////////////////////////////////////////

void Mesh::removeIndices()
{
	geometry.clearIndexBuffer();
	SAFE_DELETE(indices);
}

//////////////////////////////////////////////////////////////

void Mesh::createIndices( Mesh::INDEX_SIZE size  )
{
	SAFE_DELETE(indices);
	
	if( size == INDEX_DEFAULT )
	{
		if( getNumVertices() > 0xFFFF )
			size = INDEX_32;
		else
			size = INDEX_16;
	}
	
	switch( size )
	{
		case INDEX_16:
			indices = new IndexArray16();
			break;

		case INDEX_32:
			indices = new IndexArray32();
			break;

		default:
			indices = new IndexArray32();
			break;
			
	}
}

//////////////////////////////////////////////////////////////
	
void Mesh::makeVerticesFromFaces()
{
	if(!getNumIndices())
		return;
		
	// tmp
	std::vector <cm::Vec3> tvertices;
	std::vector <cm::Vec3> tnormals;
	std::vector <cm::Vec2> tuvs;
	std::vector <unsigned int> tcolors;
	
	tvertices = vertices;
	vertices.clear();
	
	if( hasNormals() )
	{
		tnormals = normals;
		normals.clear();
	}
	
	if( hasTexCoords() )
	{
		tuvs = uvs;
		uvs.clear();
	}
	
	if( hasColors() )
	{
		tcolors = colors;
		colors.clear();
	}
	
	for( int i = 0; i < getNumIndices(); i++ )
	{
		int ind = indices->getIndex(i);
		
		vertices.push_back( tvertices[ ind ] );
		if(tnormals.size())
			normals.push_back( tnormals[ ind ] );
		if(tuvs.size())
			uvs.push_back( tuvs[ ind ] );
		if(tcolors.size())
			colors.push_back( tcolors[ ind ] );
	}
	
	removeIndices();
}

//////////////////////////////////////////////////////////////

	
void Mesh::computeFaceNormals()
{
	// TODO make it work with quads
	if(!getNumVertices())
		return;
		
	makeVerticesFromFaces();
	
	normals.resize( getNumVertices() );
	if( bDebugNormals )
		colors.clear();

	for( int i = 0; i < getNumVertices(); i+=3 )
	{
		cm::Vec3 n = computeNormal( vertices[ i ], 
								vertices[ i+1 ], 
								vertices[ i+2 ] );
								
		normals[i] = n;
		normals[i+1] = n;
		normals[i+2] = n;
		
		if( bDebugNormals )
		{
			colors.push_back(0);
			setNormalColor(n);
			colors.push_back(0);
			setNormalColor(n);
			colors.push_back(0);
			setNormalColor(n);
		}
	}
}

//////////////////////////////////////////////////////////////

void Mesh::drawNormals( float len )
{
	if(!hasNormals())
		return;
		
	int nn = normals.size();
	int nv = vertices.size();
	
	cm::Vec3 * verts = getVertices();
	cm::Vec3 * norms = getNormals();
	
	gfx::beginVertices( LINELIST );
	for( int i = 0; i < nv; i++ )
	{
		gfx::vertex( verts[i] );
		gfx::vertex( verts[i] + norms[i]*len );
	}
	gfx::endVertices();
}

//////////////////////////////////////////////////////////////

AABox Mesh::getBoundingBox() const
{
	AABox b;
	for( int i = 0; i < getNumVertices(); i++ )
	{
		b.addVertex(vertices[i],i==0);
	}
	return b;
}

//////////////////////////////////////////////////////////////

void Mesh::transform( const cm::M44 & m )
{
	for( int i = 0; i < getNumVertices(); i++ )
	{
		vertices[i] = mul( vertices[i], m );
	}
	updateVertices();
}


//////////////////////////////////////////////////////////////

typedef std::map<cm::Vec3,int> VertexMap;

typedef HashMap<cm::Vec3,int,3000> VertexIndexCache;

void Mesh::getUniqueIndices( int * uniqueIndices )
{
	VertexMap m;
	
	VertexIndexCache cache;

	// if there are no indices in mesh create a temporary index array
	// with consecutive indices
	bool createdIndices = false;
	
	std::vector <int> inds;
	if( !indices )
	{
		for( int i = 0; i < getNumVertices(); i++ )
			inds.push_back(i);
	}
	else
	{
		for( int i = 0; i < indices->size(); i++ )
			inds.push_back(indices->getIndex(i));
	}

	cm::Vec3 * verts = getVertices();
	
	// find unique indices
	for( int i = 0; i < inds.size(); i++ )
	{
		int ind = inds[i];
		const cm::Vec3 & v = verts[ ind ];
		
		VertexIndexCache::Entry * e = cache.get(v);
		
		if( !e) 
		{
			cache.add(v,ind);
			uniqueIndices[i] = ind;
		}
		else
		{
			// vertex allready exists
			uniqueIndices[i] = e->obj;
		}
	}
	
	int s = cache.size();
	cache.clear();
}



}