/*
 *  HalfEdge.cpp
 *
 *  Created by Daniel Berio on 7/14/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */


#include "HalfEdgeCm.h"
#include "engine/renderSys/Mesh.h"

using namespace cm;
namespace cm2
{

static std::string getString( const cm::Vec3 & v )
{
	std::string s;
	stdPrintf(s,"%.5f %.5f %.5f",v.x,v.y,v.z);
	return s;
}


HalfEdge::HalfEdge()
:
boundaryCount(0)
{
}

HalfEdge::~HalfEdge()
{
	release();
}

void HalfEdge::release()
{
	boundaryCount = 0;
	edges.clear();
	faces.clear();
}

//#define USESTD
#ifdef USESTD

typedef std::map<unsigned long,HalfEdge::Edge*> HEMap;

static unsigned long getKey( unsigned int a, unsigned int b )
{
	return a | (b<<16);
}

static int getA( unsigned long key )
{
	return key&0xFFFF;
}

static int getB( unsigned long key )
{
	return (key>>16)&0xFFFF;
}

static unsigned long getTwin( unsigned long key )
{
	return getKey( getB(key), getA(key) );
}

#else

struct EdgeKey
{
	EdgeKey()
	{
		a = b = 0;
	}
	
	EdgeKey( unsigned int a, unsigned int b )
	:
	a(a),
	b(b)
	{
	}
	
	unsigned int hash() const { return (a*73856093) ^ (b*83492791); }
	bool equals( const EdgeKey & e ) const { return a == e.a && b == e.b; }
	
	EdgeKey twin() 
	{
		return EdgeKey(b,a);
	}
	
	unsigned int a;
	unsigned int b;
};

typedef HashMap< EdgeKey, HalfEdge::Edge *, 4000 > HEMap;

#endif

void	HalfEdge::compute( cm::Mesh * mesh )
{
	int numIndices = mesh->getNumIndices();
	if(numIndices == 0)
		numIndices = mesh->getNumVertices();
		
	int *indices = new int[numIndices];
	
	mesh->getUniqueIndices(indices);
	
	int numFaces = numIndices/3;
	int numEdges = numFaces*3;
	
	// create edges and faces
	edges.resize(numEdges);
	faces.resize(numFaces);
	
	Edge * e = getEdges();
	
	HEMap m;
	
	int eIndex = 0;
	
	// compute half edges without twins
	int j = 0;
	
	for( int i = 0; i < numFaces; i++ )
	{
		Face * f = &faces[i];
		f->edge = e;
		f->index = i;

		int a = indices[j++];
		int b = indices[j++];
		int c = indices[j++];
		
		// edge c -> a
		e->index = a;
		e->next = e+1;
		e->face = f;
		e->edgeIndex = eIndex++;
		#ifdef USESTD
			m[ getKey( c,a) ] = e;
		#else
			m.addOnce( EdgeKey(c,a), e );
		#endif
		
		e++;
		
		// edge a -> b
		e->index = b;
		e->next = e+1;
		e->face = f;
		e->edgeIndex = eIndex++;
		#ifdef USESTD
			m[ getKey( a,b) ] = e;
		#else
			m.addOnce( EdgeKey(a,b), e );
		#endif
		
		e++;

		// edge b -> c
		e->index = c;
		e->next = e-2;
		e->face = f;
		e->edgeIndex = eIndex++;
		#ifdef USESTD
			m[ getKey( b,c) ] = e;
		#else
			m.addOnce( EdgeKey(b,c), e );
		#endif
		
		e++;
	}
	
	int sz = m.size();
	if( m.size() != numEdges )
	{
		printf("Mesh seems fucked up: Expected %d edges but found %d\n",numEdges,sz);
	}
	
	#ifdef USESTD
	// sarch for twins 
	for( HEMap::iterator it = m.begin(); it != m.end(); it++ )
	{
		unsigned long key = it->first;
		Edge * he = it->second;
		
		// find twin if present
		HEMap::iterator tit = m.find( getTwin(key) );
		
		if( tit != m.end() )
		{
			Edge * twin = tit->second;
			he->twin = twin;
			twin->twin = he;
		}
		else
		{
			he->twin = 0;
			boundaryCount++;
		}
	}	
	#else
	for( int i = 0; i < m.size(); i++ )
	{
		HEMap::Entry * e = m.getEntry(i);
		Edge * he = e->obj;
		
		HEMap::Entry * te = m.get( e->key.twin() );
		if( te )
		{
			Edge * twin = te->obj;
			he->twin = twin;
			twin->twin = he;
		}
		else
		{
			he->twin = 0;
			boundaryCount++;
		}
	}
	#endif
	
	delete [] indices;
}


void	HalfEdge::capHoles()
{
	// find all edges that don't have a twin
	std::vector<Edge*> bedges;
	for( int i = 0; i < getNumEdges(); i++ )
	{
		Edge * e = &edges[i];
		if(e->twin == 0)
			bedges.push_back(e);
	}
	
	/*
	
	for every edge
		search for a 
	 
	*/
}




}