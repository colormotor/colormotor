/*
 *  HalfEdge.h
 *
 *  Created by Daniel Berio on 7/14/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once

#include <vector>

namespace cm
{
	class Mesh;
}

namespace cm2
{




class HalfEdge
{
public:

	struct Face;
	
	struct Edge
	{
	public:
		Edge()
		:
		next(0),
		twin(0)
		{
		}
		int index;      // Vertex index at the end of this half-edge
		int edgeIndex;
		Face *face;	// Face 
		Edge* twin; // Oppositely oriented adjacent half-edge
		Edge* next; // Next half-edge around the face
		
		void getIndices( int & a, int & b ) const
		{
			a = index;
			b = getPrev()->index;
		}
		
		Edge * getPrev() { return next->next; }
		const Edge * getPrev() const { return next->next; }
	};

	struct Face
	{
		Face()
		:
		edge(0)
		{
		}
	
		Edge * edge;
		int index;

		void getIndices( int & a, int & b, int & c ) const
		{
			a = edge->index;
			b = edge->next->index;
			c = edge->next->next->index;
		}
	};
	
	HalfEdge();
	virtual ~HalfEdge();
	void release();
	
	void	compute( cm::Mesh * mesh );
		
	Edge *getEdges() {return &edges[0]; }
	Face *getFaces() {return &faces[0]; }
	const Edge *getEdges() const {return &edges[0]; }
	const Face *getFaces() const {return &faces[0]; }

	int getNumEdges() const { return edges.size(); }
	int getNumFaces() const { return faces.size(); }
	
	void capHoles();
		
protected:
	typedef std::vector<Edge> EdgeVector;
	EdgeVector edges;
	typedef std::vector<Face> FaceVector;
	FaceVector faces;
	
	int boundaryCount;
};

}