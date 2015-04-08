///////////////////////////////////////////////////////////////////////////                                                     
//	 _________  __   ____  ___  __  _______  __________  ___			
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \			
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/			
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.		
//																		
//  Daniel Berio 2008-2011												
//	http://www.enist.org/												
//																	
//																		
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma once 

namespace cm
{



/// Fill an array with indices for a grid of triangles of size width x height 
/// Useful for generating index buffers for terrains or any other smooth surface
/// index buffer size must be (width-1)*(height-1)*6
/// TODO: define cw ccw orientation.
template <class IndexT> void fillGridTriangleIndices( IndexT * indices, int width, int height, int step = 1 );

/// Fill an array with indices for a grid of quads of size width x height 
/// index buffer size must be (width-1)*(height-1)*4
template <class IndexT> void fillGridQuadIndices( IndexT * indices, int width, int height, int step = 1 );




///////////////////////

template <class IndexT> void fillGridTriangleIndices( IndexT * indices, int width, int height, int step )
{
	// init indices for triangle list
	int i = 0;
	for( int y = 0; y < height-step; y+=step )
	{
		int v = y*width;
		for( int x = 0; x < width-step; x+=step )
		{
			// ccw from cur vertex 
			int a = v;
			int b = v+width*step;
			int c = v+step+width*step;
			int d = v+step;
			
			indices[i++] = a;
			indices[i++] = d;
			indices[i++] = b;
			indices[i++] = b;
			indices[i++] = d;
			indices[i++] = c;
			
			v+=step;
		}
	}

}

template <class IndexT> void fillGridQuadIndices( IndexT * indices, int width, int height, int step  )
{
	// init indices for triangle list
	int i = 0;
	for( int y = 0; y < height-step; y+=step )
	{
		int v = y*width;
		for( int x = 0; x < width-step; x+=step )
		{
			// ccw from cur vertex 
			int a = v;
			int b = v+width*step;
			int c = v+step+width*step;
			int d = v+step;
						
			indices[i++] = a;
			indices[i++] = b;
			indices[i++] = c;
			indices[i++] = d;
			
			v+=step;
		}
	}

}


}
