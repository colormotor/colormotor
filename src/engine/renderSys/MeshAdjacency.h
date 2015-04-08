/*
 *  MeshAdjacency.h
 *
 *  Created by Daniel Berio on 8/7/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once

#include "engine/engineIncludes.h"

namespace cm
{

class Mesh;

class MeshAdjacency
{
	public:
		struct Face
		{
			int v[3];
		};
		
		struct Edge
		{
			// faces
			int f[2];
			// indices of edge
			int e[2];
		};
		
		
		MeshAdjacency();
		~MeshAdjacency() { release(); }
		
		void release();
		
		int computeAdjacency( Mesh * m );
		void computeFaceNormalsAndCentroids();
		
		Mesh * getMesh() { return mesh; }
		
		Face * getFaces()  { return & faces[0]; }
		Edge * getEdges()  { return & edges[0]; }
		int getNumFaces()  { return faces.size(); }
		int getNumEdges()  { return edges.size(); }
		
		cm::Vec3 * getFaceNormals()  { return &faceNormals[0]; }
		cm::Vec3 * getFaceCentroids()  { return & faceCentroids[0]; }
		
	protected:
		Mesh * mesh;
		
		void extrudeEdge( int a, int b );
		
		std::vector <Face> faces;
		std::vector <Edge> edges;
		
		std::vector <cm::Vec3> faceNormals;
		std::vector <cm::Vec3> faceCentroids;
		
};

}
