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
 
#include "gfx/cmGLInternal.h"
#include "gfx/cmGeometry.h"


namespace cm
{

#define VBOFFSET(ptr,i) ((char*)(ptr) + (i))


static int texCount = 0;
static int genericCount = 0;
static int curStartVertex = 0;

static VertexBuffer * curVertexBuffers[MAX_VERTEX_STREAMS];
static IndexBuffer * curIndexBuffer = 0;
static bool bInitialized = false;

static bool glClientStates[VertexFormat::NUMVERTEXELEMENTS];

static void initializeGeometry()
{
	if(bInitialized)
		return;
		
	for( int i = 0; i < MAX_VERTEX_STREAMS; i++ )
		curVertexBuffers[i] = 0;
		
	/// TODO ...
	for (int i = 0; i < VertexFormat::NUMVERTEXELEMENTS; i++ )
	{
		glClientStates[i] = false;
	}
	
	curIndexBuffer = 0;
	
	bInitialized = true;
}

/// GL2.0 version
static void disableGLArrays2( VertexFormat * fmt )
{
	CM_GLCONTEXT
	
	for( int i = 0; i < fmt->getNumElements(); i++ )
	{
		glDisableVertexAttribArray(genericCount);
		genericCount++;
	}
	
}


/// GL1.0 version
static void disableGLArrays( VertexFormat * fmt )
{
	CM_GLCONTEXT
	
	for( int i = 0; i < fmt->getNumElements(); i++ )
	{
		VertexFormat::ELEMENT_TYPE elem = fmt->getElementType( i ); 
		
		switch( elem )
		{
			case VertexFormat::POS3:
			case VertexFormat::POS4:
				glDisableClientState(GL_VERTEX_ARRAY); 
				break;
				
			case VertexFormat::UVW:
			case VertexFormat::UV:
				glClientActiveTexture(GL_TEXTURE0+texCount);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
				++texCount;
				break;
				
			case VertexFormat::COLOR:
				glDisableClientState(GL_COLOR_ARRAY); 
				break;
				
			case VertexFormat::NORMAL:
				glDisableClientState(GL_NORMAL_ARRAY); 
				break;
				
			default:
				glDisableVertexAttribArray(genericCount);
				break;
				
		}
		
		++genericCount;
	}
}

/// Sets gl pointers
/// This can be optimized alot by only disabling client states that are not used.
static void setGLPointers( VertexFormat * fmt, void * buf, int offset )
{
	CM_GLCONTEXT
	
	int sz = fmt->getSize();

	for( int i = 0; i < fmt->getNumElements(); i++ )
	{
		const VertexFormat::Element &elem = fmt->getElement( i ); 
		
		switch( elem.type )
		{
			case VertexFormat::POS3:
					glVertexPointer(	3,
											GL_FLOAT,
											sz,
											VBOFFSET(buf,elem.offset+offset) );
					glEnableClientState(GL_VERTEX_ARRAY); 

				break;
				
			case VertexFormat::POS4:
				glVertexPointer(	4,
									 GL_FLOAT,
									 sz,
									 VBOFFSET(buf,elem.offset+offset) );
				glEnableClientState(GL_VERTEX_ARRAY); 
				break;
				
			case VertexFormat::UV:
				glClientActiveTexture(GL_TEXTURE0+texCount);
				glTexCoordPointer(2, GL_FLOAT, sz, VBOFFSET(buf,elem.offset+offset) ); 
				glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
				++texCount;
				break;
				
			case VertexFormat::UVW:
				glClientActiveTexture(GL_TEXTURE0+texCount);
				glTexCoordPointer(3, GL_FLOAT, sz, VBOFFSET(buf,elem.offset+offset) ); 
				glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
				++texCount;
				break;
				
				
			case VertexFormat::COLOR:
				glEnableClientState(GL_COLOR_ARRAY); 
				glColorPointer(4,GL_UNSIGNED_BYTE,sz,VBOFFSET(buf,elem.offset+offset) );  // CHECK ME!
				break;
				
			case VertexFormat::NORMAL:
				glNormalPointer(GL_FLOAT, sz, VBOFFSET(buf,elem.offset+offset) ); 
				glEnableClientState(GL_NORMAL_ARRAY); 
				break;
				
			default:
				glVertexAttribPointer(genericCount,
									    elem.numComponents,
									    elem.glType,
									    GL_FALSE,
									    sz,
									    VBOFFSET(buf,elem.offset+offset) ); 
										

				glEnableVertexAttribArray(genericCount);

			
				
		}
		
		genericCount++;
	}
}


/// This can be optimized alot by only disabling client states that are not used.
static void setGLPointers2( VertexFormat * fmt, void * buf, int offset )
{
	CM_GLCONTEXT
	
	int sz = fmt->getSize();
	
	for( int i = 0; i < fmt->getNumElements(); i++ )
	{
		const VertexFormat::Element &elem = fmt->getElement( i ); 
		
		glVertexAttribPointer(genericCount,
							elem.numComponents,
							elem.glType,
							GL_FALSE,
							sz,
							VBOFFSET(buf,elem.offset+offset) ); 
		
		glEnableVertexAttribArray(genericCount);
		
		genericCount++;
	}
}



//////////////////////////////////////////////////////////



Geometry::Geometry()
{
	initializeGeometry();
	_vbs.clear();
	_ib = 0;
}

//////////////////////////////////////////////////////////


Geometry::~Geometry()
{
	release();
}

//////////////////////////////////////////////////////////



void Geometry::release( bool deleteVertexBuffers , bool deleteIndexBuffer  )
{
	if( deleteVertexBuffers )
	{
		clearVertexBuffers();
	}
	
	if( deleteIndexBuffer )
	{
		clearIndexBuffer();
	}
}

void Geometry::clearVertexBuffers()
{
	for( int i = 0; i < getNumVertexBuffers(); i++ )
	{
		if(_vbs[i])
			delete _vbs[i];
		_vbs[i] = 0;
	}
	
	_vbs.clear();
}

void Geometry::clearIndexBuffer()
{
	_ib = 0;
}
	

//////////////////////////////////////////////////////////



/// Adds a vertex buffer with specified format, filled with user data
VertexBuffer * Geometry::addVertexBufferWithData( const char * fmt, void * data, int numVertices,  HardwareBuffer::USAGE usage )
{
	
	VertexBuffer * vb = addVertexBuffer( fmt, numVertices, usage );
	if( !vb ) 
		return 0;
		
	vb->update( data, numVertices,0 );
	
	return vb;
	
}

//////////////////////////////////////////////////////////


/// Adds a vertex buffer with specified format
VertexBuffer * Geometry::addVertexBuffer( const char * fmt, int numVertices, HardwareBuffer::USAGE usage )
{
	VertexBuffer * vb = new VertexBuffer();
	
	if( ! vb->init(fmt,numVertices,usage) )
	{
		delete vb;
		return 0;
	}
		
	_vbs.push_back(vb);
	
	return vb;
}


//////////////////////////////////////////////////////////



/// Creates an index buffer , filled with user data
IndexBuffer * Geometry::createIndexBufferWithData( void * data, int numIndices, IndexBuffer::TYPE indexType, HardwareBuffer::USAGE usage  )
{
	IndexBuffer * ib = createIndexBuffer( numIndices, indexType, usage );
	if( !ib ) 
		return 0;
		
	ib->update(data,numIndices,0);
	
	return ib;
}

//////////////////////////////////////////////////////////


IndexBuffer * Geometry::createIndexBuffer( int numIndices, IndexBuffer::TYPE indexType, HardwareBuffer::USAGE usage  )
{
	IndexBuffer * ib = new IndexBuffer();
	if( ! ib->init( numIndices, indexType, usage ) )
	{
		delete ib;
		return 0;
	}
	
	_ib = ib;
	
	return ib;
		
}

//////////////////////////////////////////////////////////

bool		Geometry::updateVertexBuffer( int index, void * data, int count, int start  )
{
	return _vbs[index]->update(data,count,start);
}

//////////////////////////////////////////////////////////

bool		Geometry::updateIndexBuffer( void * data, int count, int start  )
{
	return _ib->update(data,count,start);
}

//////////////////////////////////////////////////////////


/// TODO add tests

bool Geometry::bind( int startVertex )
{
	CM_GLCONTEXT
	
	bool allreadyBound = true;
	for( int i = 0; i < getNumVertexBuffers(); i++ )
	{
		if( curVertexBuffers[i] != _vbs[i] )
		{
			allreadyBound = false;
			break;
		}
	}
	
	if( _ib != curIndexBuffer )
		allreadyBound = false;
	
	// TODO: Also this could be highly optimized!
	if( curStartVertex != startVertex )
		allreadyBound = false;
		
	if( allreadyBound )
		return true;
		

	unbind();
	
	curStartVertex = startVertex;
	
	// bind vertex buffers
	for ( int i = 0; i < getNumVertexBuffers(); i++ )
	{
		VertexBuffer * vb = _vbs[i];
		curVertexBuffers[i] = vb;
		VertexFormat * fmt = vb->getVertexFormat();
		
		glBindBuffer(GL_ARRAY_BUFFER, vb->getGLId());
		
		if( cmGLVersion() < 2 )
			setGLPointers(fmt,0,startVertex*fmt->getSize() );
		else 
			setGLPointers2(fmt,0,startVertex*fmt->getSize() );
	}
	
	if( _ib != 0 )
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ib->getGLId());
	}
	
	return true;
}


//////////////////////////////////////////////////////////


bool Geometry::unbind()
{
	return resetAll();
}

//////////////////////////////////////////////////////////


bool Geometry::resetAll()
{
	CM_GLCONTEXT
	
	texCount = 0;
	genericCount = 0;
	
	int i = 0;
	VertexBuffer * vb = curVertexBuffers[0];
	
	while( vb && i < MAX_VERTEX_STREAMS )
	{
		VertexFormat * fmt = vb->getVertexFormat();
		
		if( cmGLVersion() < 2 )
			disableGLArrays(fmt);
		else
			disableGLArrays2(fmt);
			
		curVertexBuffers[i]  = 0;
		++i;
		vb = curVertexBuffers[i];
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glClientActiveTexture(GL_TEXTURE0);
	
	return true;
}

//////////////////////////////////////////////////////////


bool Geometry::draw( PRIMITIVE prim, int numElements, int startVertex )
{
	CM_GLCONTEXT
	
	// TODO checkme
	glDrawArrays(getGLPRIMITIVETYPE(prim), startVertex, numElements); 
	
	if( CM_GLERROR )
		debugPrint("error in Geometry::draw\n");
	return true;
}



//////////////////////////////////////////////////////////



bool Geometry::drawIndexed( PRIMITIVE prim, int numElements, int startIndex )
{
	CM_GLCONTEXT
	
	if( !_ib )
	{
		debugPrint( "No index buffer set! \n" );
		return false;
	}
	
	glDrawElements(getGLPRIMITIVETYPE(prim),
				   numElements,
				   _ib->getGLIndexFormat(),
				   VBOFFSET(0,_ib->getStride()*startIndex));

	return true;
}

#ifndef PI
#define PI				3.14159265358979323846
#endif


void	Geometry::sphere( const char * format, float radius, unsigned int clr,  int latitudeSubd, int longitudeSubd )
{
	release();
	
	
	int n = (latitudeSubd+1)*longitudeSubd;
	int ni = (longitudeSubd-1)*latitudeSubd*6;
	
	addVertexBuffer(format,n); 
	createIndexBuffer(ni,IndexBuffer::INDEX_16);
	
	VertexFormat * fmt = getVertexBuffer(0)->getVertexFormat();
	char * vbuf = new char[n*fmt->getSize()];
	unsigned short * ibuf = new unsigned short[ ni ];
	
	float pid2 = PI/2;
	float pi2 = PI*2;
	
	int i,j;
	double theta,phi;

	//-PI/2 <= theta <= PI/2 ( range PI )	// longitude ( angolo su spicchio )
	//0 <= phi <= TWOPI						// latitude ( angolo orizzontale )

	// deltas
	double dtheta = PI / (longitudeSubd-1);
	double dphi = pi2 / latitudeSubd;

	theta = - pid2;

	float x,y,z;
	float nx,ny,nz;
	float u,v;
	
	int ind = 0;
	
	VertexWriter vr(fmt,vbuf);
	
	int vcount = 0;
	
	for (j=0;j<longitudeSubd;j++) 
	{
		theta = - pid2 + dtheta*j;
		phi = 0.0f;

		for (i=0;i<=latitudeSubd;i++) // bisogna fare <=  per le uv altrimenti non torna l'ultimo poligono
		{
			phi = dphi * i;

			float ctheta,cphi,stheta,sphi;
			
			ctheta = cos(theta);
			cphi = cos(phi);
			stheta = sin(theta);
			sphi = sin(phi);

			nx = x = ctheta*cphi;
			ny = y = ctheta*sphi;
			nz = z = stheta;
			
			x*=radius;
			y*=radius;
			z*=radius;
			

			float texphi = acosf(nz);
			v = texphi / PI;
			
			u =  acosf( std::max( std::min(ny / sinf(texphi),1.0f), -1.0f) ) / (2.0f * PI) ;
			u = (nx > 0.0f) ? u :  1 - u;
			
	
			vr.position(x,y,z);
			vr.normal(nx,ny,nz);
			vr.color(clr);
			vr.uv(u,v);
			
			if( j < (longitudeSubd-1) && i < (latitudeSubd) ) //
			{
				int l = latitudeSubd+1;
				int a = j*l+i;
				int b = (j+1)*l+i;
				int c = (j+1)*l+ ((i+1));//%i_nLatitude);
				int d = (j)*l+ ((i+1));//%i_nLatitude);

				ibuf[ind+0] = a;
				ibuf[ind+1] = c;
				ibuf[ind+2] = b;
				
				ibuf[ind+3] = a;
				ibuf[ind+4] = d;
				ibuf[ind+5] = c;
				ind += 6;
				
			}
		
			vcount++;
		}
	}
	
	_vbs[0]->update(vbuf,n);
	_ib->update(ibuf,ni);
	delete [] vbuf;
	delete [] ibuf;
	
}

}