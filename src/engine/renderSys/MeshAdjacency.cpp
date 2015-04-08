/*
 *  MeshAdjacency.cpp
 *
 *  Created by Daniel Berio on 8/7/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#include "MeshAdjacency.h"
#include "Mesh.h"

namespace cm
{

MeshAdjacency::MeshAdjacency()
{
}

void MeshAdjacency::release()
{
	faces.clear();
	mesh = 0;
}

// based on Lengyel, Eric. “Building an Edge List for an Arbitrary Mesh”. Terathon Software 3D Graphics Library, 2005. http://www.terathon.com/code/edges.html

int MeshAdjacency::computeAdjacency(  Mesh * mesh )
{
	this->mesh = mesh;
	
	int triangleCount = mesh->getNumIndices() / 3;
	if(!triangleCount)
		triangleCount = mesh->getNumVertices() / 3;
		
	faces.resize( triangleCount );
	mesh->getUniqueIndices((int*)(&faces[0]));
	
	int vertexCount = mesh->getNumVertices();
    int maxEdgeCount = faces.size()*3; // num faces * 3
    
	int *firstEdge = new int[vertexCount + maxEdgeCount];
    int *nextEdge = firstEdge + vertexCount;
    
    for (int a = 0; a < vertexCount; a++) firstEdge[a] = -1;
    
    // First pass over all triangles. This finds all the edges satisfying the
    // condition that the first vertex index is less than the second vertex index
    // when the direction from the first vertex to the second vertex represents
    // a counterclockwise winding around the triangle to which the edge belongs.
    // For each edge found, the edge index is stored in a linked list of edges
    // belonging to the lower-numbered vertex index i. This allows us to quickly
    // find an edge in the second pass whose higher-numbered vertex index is i.
    
    int edgeCount = 0;
    const Face *triangle = &faces[0];
    for (int a = 0; a < triangleCount; a++)
    {
        int i1 = triangle->v[2];
        for (int b = 0; b < 3; b++)
        {
            int i2 = triangle->v[b];
            if (i1 < i2)
            {
				edges.push_back(Edge());
				Edge * edge = &edges.back();
                
                edge->e[0] = (int) i1;
                edge->e[1] = (int) i2;
                edge->f[0] = (int) a;
                edge->f[1] = (int) a;
                
                int edgeIndex = firstEdge[i1];
                if (edgeIndex == -1)
                {
                    firstEdge[i1] = edgeCount;
                }
                else
                {
                    for (;;)
                    {
                        int index = nextEdge[edgeIndex];
                        if (index == -1)
                        {
                            nextEdge[edgeIndex] = edgeCount;
                            break;
                        }
                        
                        edgeIndex = index;
                    }
                }
                
                nextEdge[edgeCount] = -1;
                edgeCount++;
            }
            
            i1 = i2;
        }
        
        triangle++;
    }
    
    // Second pass over all triangles. This finds all the edges satisfying the
    // condition that the first vertex index is greater than the second vertex index
    // when the direction from the first vertex to the second vertex represents
    // a counterclockwise winding around the triangle to which the edge belongs.
    // For each of these edges, the same edge should have already been found in
    // the first pass for a different triangle. So we search the list of edges
    // for the higher-numbered vertex index for the matching edge and fill in the
    // second triangle index. The maximum number of comparisons in this search for
    // any vertex is the number of edges having that vertex as an endpoint.
    
    triangle = &faces[0];
    for (int a = 0; a < triangleCount; a++)
    {
        int i1 = triangle->v[2];
        for (int b = 0; b < 3; b++)
        {
            int i2 = triangle->v[b];
            if (i1 > i2)
            {
                for (int edgeIndex = firstEdge[i2]; edgeIndex != -1;
                        edgeIndex = nextEdge[edgeIndex])
                {
                    Edge *edge = &edges[edgeIndex];
                    if ((edge->e[1] == i1) &&
                            (edge->f[0] == edge->f[1]))
                    {
                        edge->f[1] = (int) a;
                        break;
                    }
                }
            }
            
            i1 = i2;
        }
        
        triangle++;
    }
    
    delete[] firstEdge;
    return edgeCount;
}



void MeshAdjacency::computeFaceNormalsAndCentroids()
{
	int numFaces = getNumFaces();
	
	faceNormals.resize( numFaces );
	faceCentroids.resize( numFaces );
	
	cm::Vec3 * srcVerts = mesh->getVertices();
	
	for( int i = 0; i < numFaces; i++ )
	{
		Face & f = faces[i];
		
		const cm::Vec3 & pa = srcVerts[f.v[0]];
		const cm::Vec3 & pb = srcVerts[f.v[1]];
		const cm::Vec3 & pc = srcVerts[f.v[2]];
		
		// compute face normal
		faceNormals[i] = computeNormal(pa,pb,pc);
		
		faceCentroids[i] = (pa+pb+pc)/3;
	}

}

}
		