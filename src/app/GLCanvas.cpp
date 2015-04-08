/*
 *  GLCanvas.cpp
 *  resapiTest
 *
 *  Created by Daniel Berio on 5/4/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */



#include "GLCanvas.h"
#include "cmGfx.h"

#define RGBA(r,g,b,a) ((unsigned int)((((unsigned char)(a)&0xff)<<24)|(((unsigned char)(b)&0xff)<<16)|(((unsigned char)(g)&0xff)<<8)|((unsigned char)(r)&0xff)))
#define GET_B( clr )  (( (clr) >> 16 )  &0xFF)
#define GET_G( clr )  (( (clr) >> 8  )  &0xFF)
#define GET_R( clr )  (  (clr)  	    &0xFF)
#define GET_A( clr )  (( (clr) >> 24 )  &0xFF)


namespace cm
{

#define DEFAULTFONT font //Fonts::uni9

#ifndef PI
#define PI				3.14159265358979323846
#endif

GLCanvas::GLCanvas()
{
	_strokeColor = 0;
	
	_fillColor = 0;
	
	_textColor = 0;
	
	_bFill = false;
	_bLine = true;
	
	setFont(&Fonts::uni9);
	
	_triangulate = false;
}	

GLCanvas::~GLCanvas()
{

}	

void GLCanvas::setup()
{
	CM_GLCONTEXT
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

bool GLCanvas::init( int w, int h, void * data  )
{
	width = w;
	height = h;
	return true;
}	

void GLCanvas::image( void * data, float x, float y, float w, float h )
{
CM_GLCONTEXT
	Texture * tex = ( Texture * ) data;
	glColor4ub( GET_R(_fillColor), GET_G(_fillColor), GET_B(_fillColor), GET_A(_fillColor) );
	tex->draw(x,y,w,h);
	glColor4f(1,1,1,1);
}	

void GLCanvas::patternRect( void * data, float x, float y, float w, float h, float xscale, float yscale )
{
	Texture * tex = ( Texture * ) data;
	tex->bind();
	tex->setWrapS(Texture::REPEAT);
	tex->setWrapT(Texture::REPEAT);
	float u = (w*xscale)/tex->getWidth();
	float v = (h*yscale)/tex->getHeight();
	tex->draw(x,y,w,h,false,u,v);
	tex->unbind();
}

void GLCanvas::blitImage( void * data, float x, float y, float w, float h, float imgx, float imgy )
{
// here its hard to remove dependencies.....
	Texture * tex = ( Texture * ) data;
	tex->drawPart(x,y,w,h,imgx/tex->getWidth(),imgy/tex->getHeight(),w/tex->getWidth(),h/tex->getHeight());
}


#define circleSubdivision	30

void GLCanvas::circle( float x, float y, float radius )
{
	CM_GLCONTEXT
	
	float t = 0;
	float inc = (PI*2)/circleSubdivision;
	

	
	// center for triangle fan
	if( _bFill )
	{
		pts[0].pos.x = x;
		pts[0].pos.y = y;
		pts[0].fillColor = _fillColor;
		pts[0].strokeColor = _strokeColor;
	}
	
	int c = 1;
	
	for( int i = 0; i <= circleSubdivision; i++ )
	{
		float px = x+cosf(t)*radius;
		float py = y+sinf(t)*radius;
		
		pts[c].pos.x = px;
		pts[c].pos.y = py;
		pts[c].fillColor = _fillColor;
		pts[c].strokeColor = _strokeColor;
		c++;

		t+=inc;
	}
	
	pts[c] = pts[1];
	
	// draw it
	if(_bFill)
	{
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &pts[0].fillColor );
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT,  sizeof(Vertex),  &pts[0].pos );
		glDrawArrays( GL_TRIANGLE_FAN, 0, circleSubdivision+2 );
		glDisableClientState( GL_COLOR_ARRAY );
	}
	
	if(_bLine)
	{
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &pts[1].strokeColor );
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT,  sizeof(Vertex),  &pts[1].pos );
		glDrawArrays( GL_LINE_LOOP, 0, circleSubdivision );
		glDisableClientState( GL_COLOR_ARRAY );
	}
	
}


void GLCanvas::ellipse( float x, float y, float w, float h  )
{
}	

void GLCanvas::rect( float x, float y, float w, float h )
{
	CM_GLCONTEXT
	
	numPts = 0;
	
	pts[0].pos.x = x;
	pts[0].pos.y = y;
	pts[0].fillColor = _fillColor;
	pts[0].strokeColor = _strokeColor;

	pts[1].pos.x = x+w;
	pts[1].pos.y = y;
	pts[1].fillColor = _fillColor;
	pts[1].strokeColor = _strokeColor;
	
	pts[2].pos.x = x+w;
	pts[2].pos.y = y+h;
	pts[2].fillColor = _fillColor;
	pts[2].strokeColor = _strokeColor;
	
	pts[3].pos.x = x;
	pts[3].pos.y = y+h;
	pts[3].fillColor = _fillColor;
	pts[3].strokeColor = _strokeColor;
	
	if(_bFill)
	{
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &pts[0].fillColor );
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT,  sizeof(Vertex),  &pts[0].pos );
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
		glDisableClientState( GL_COLOR_ARRAY );
	}
	
	if(_bLine)
	{
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &pts[0].strokeColor );
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT,  sizeof(Vertex),  &pts[0].pos );
		glDrawArrays( GL_LINE_LOOP, 0, 4 );
		glDisableClientState( GL_COLOR_ARRAY );
	}
	
	
}
	
	
void GLCanvas::verticalGradient( const float * r, const float * clr1, const float * clr2 ) 
{
	CM_GLCONTEXT
	
	numPts = 0;
		
	unsigned int top = RGBA(clr1[0]*255,clr1[1]*255,clr1[2]*255,clr1[3]*255);
	unsigned int bottom = RGBA(clr2[0]*255,clr2[1]*255,clr2[2]*255,clr2[3]*255);
	
	// lazy...
	float x = r[0];
	float y = r[1];
	float w = r[2]-r[0];
	float h = r[3]-r[1];
	
	pts[0].pos.x = x;
	pts[0].pos.y = y;
	pts[0].fillColor = top;
	pts[0].strokeColor = top;

	pts[1].pos.x = x+w;
	pts[1].pos.y = y;
	pts[1].fillColor = top;
	pts[1].strokeColor = top;
	
	pts[2].pos.x = x+w;
	pts[2].pos.y = y+h;
	pts[2].fillColor = bottom;
	pts[2].strokeColor = bottom;
	
	pts[3].pos.x = x;
	pts[3].pos.y = y+h;
	pts[3].fillColor = bottom;
	pts[3].strokeColor = bottom;
	

	glEnableClientState( GL_COLOR_ARRAY );
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &pts[0].fillColor );
	glEnableClientState(GL_VERTEX_ARRAY);		
	glVertexPointer(2, GL_FLOAT,  sizeof(Vertex),  &pts[0].pos );
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glDisableClientState( GL_COLOR_ARRAY );
}

void GLCanvas::horizontalGradient( const float * r, const float * clr1, const float * clr2 ) 
{
	CM_GLCONTEXT
	
	numPts = 0;
		
	unsigned int left = RGBA(clr1[0]*255,clr1[1]*255,clr1[2]*255,clr1[3]*255);
	unsigned int right = RGBA(clr2[0]*255,clr2[1]*255,clr2[2]*255,clr2[3]*255);
	
	// lazy...
	float x = r[0];
	float y = r[1];
	float w = r[2]-r[0];
	float h = r[3]-r[1];
	
	pts[0].pos.x = x;
	pts[0].pos.y = y;
	pts[0].fillColor = left;
	pts[0].strokeColor = left;

	pts[1].pos.x = x+w;
	pts[1].pos.y = y;
	pts[1].fillColor = right;
	pts[1].strokeColor = right;
	
	pts[2].pos.x = x+w;
	pts[2].pos.y = y+h;
	pts[2].fillColor = right;
	pts[2].strokeColor = right;
	
	pts[3].pos.x = x;
	pts[3].pos.y = y+h;
	pts[3].fillColor = left;
	pts[3].strokeColor = left;
	

	glEnableClientState( GL_COLOR_ARRAY );
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &pts[0].fillColor );
	glEnableClientState(GL_VERTEX_ARRAY);		
	glVertexPointer(2, GL_FLOAT,  sizeof(Vertex),  &pts[0].pos );
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glDisableClientState( GL_COLOR_ARRAY );
}
	
void GLCanvas::line( float x0, float y0, float x1, float y1 )
{
	CM_GLCONTEXT
	
	unsigned int colors[2] = { _strokeColor, _strokeColor };
	float verts[4] = 
	{	x0,y0, x1,y1 };
		
	glEnableClientState( GL_COLOR_ARRAY );
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors );
	glEnableClientState(GL_VERTEX_ARRAY);		
	glVertexPointer(2, GL_FLOAT, 0, verts );
	glDrawArrays( GL_LINES, 0, 2 );
	glDisableClientState( GL_COLOR_ARRAY );
}

void GLCanvas::textSubstring( float x, float y, const char * str, int from, int to )
{
	CM_GLCONTEXT
	
	glColor4ub( GET_R(_textColor), GET_G(_textColor), GET_B(_textColor), GET_A(_textColor) );
	DEFAULTFONT->substring(x,y,str,from,to);
}

void GLCanvas::text( float x, float y, const char * str )
{
	CM_GLCONTEXT
	
//	glColor4f( _textColor[0], _textColor[1], _textColor[2], _textColor[3] );
	glColor4ub( GET_R(_textColor), GET_G(_textColor), GET_B(_textColor), GET_A(_textColor) );
	DEFAULTFONT->print(x,y,str);
}	

void GLCanvas::textVertical( float x, float y, const char * str )
{
	CM_GLCONTEXT
	
//	glColor4f( _textColor[0], _textColor[1], _textColor[2], _textColor[3] );
	glColor4ub( GET_R(_textColor), GET_G(_textColor), GET_B(_textColor), GET_A(_textColor) );
	glPushMatrix();
	glRotatef(0,0,1,90);
	DEFAULTFONT->print(x,y,str);
	glPopMatrix();
}	

int GLCanvas::getStringLengthForSize( const char * str, float size )
{
	return DEFAULTFONT->getStringLengthForSize(str,size);
}


float GLCanvas::getStringExtent( const char * str, int n )
{
	return DEFAULTFONT->getStringSize(str,n);
}

cm::Font * GLCanvas::getFont()
{
	return DEFAULTFONT;
}

void GLCanvas::setFont( Font * f )
{
	font = f;
}

float GLCanvas::getTextHeight()
{
	return 9; //FIXME
}


void GLCanvas::beginPoly()
{
	numPts = 0;
	numInds = 0;
	
	if( _triangulate )
		beginTriangulation();
}

void GLCanvas::onTriangulatedVertexIndex(int i)
{
	indices[numInds++] = i;
}

void GLCanvas::endPoly( bool close )
{	
	CM_GLCONTEXT
	
//	pts[numPts] = pts[0];
//	numPts++;
	
	if(_triangulate)
		endTriangulation();
	
	if(_bFill)
	{
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &pts[0].fillColor );
		glEnableClientState(GL_VERTEX_ARRAY);		
		glVertexPointer(2, GL_FLOAT,  sizeof(Vertex),  &pts[0].pos );
		
		if(_triangulate)
		{
			glDrawElements(GL_TRIANGLES,numInds,GL_UNSIGNED_SHORT,indices);
		}
		else
		{
			#ifndef CM_GLES
			glDrawArrays( GL_TRIANGLE_FAN, 0, numPts );
			#else
			glDrawArrays( GL_POLYGON , 0, numPts );
			#endif
		}
	}
	
	if(_bLine)
	{
		if(!_bFill)
		{
			glEnableClientState( GL_COLOR_ARRAY );
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &pts[0].strokeColor );
			glEnableClientState(GL_VERTEX_ARRAY);		
			glVertexPointer(2, GL_FLOAT,  sizeof(Vertex),  &pts[0].pos );
		}
		
		if(close)
			glDrawArrays( GL_LINE_LOOP, 0, numPts );
		else
			glDrawArrays( GL_LINE_STRIP, 0, numPts );
	}
	
	glDisableClientState( GL_COLOR_ARRAY );
}
		
				
void GLCanvas::getLastPolyPoint( float * pt )
{
	if(!numPts)
		return;
		
	int i = numPts-1;
	pt[0] = pts[i].pos.x;
	pt[1] = pts[i].pos.y;
}

void GLCanvas::vertex( float x, float y )
{
	pts[numPts].pos.x = x;
	pts[numPts].pos.y = y;
	pts[numPts].strokeColor = _strokeColor;
	pts[numPts].fillColor = _fillColor;
	
	++numPts;
}	


void GLCanvas::textColor( float r, float g, float b, float a  )
{
	r *= 255;
	g *= 255;
	b *= 255;
	a *= 255;
	
	_textColor = RGBA(r,g,b,a);
}

void GLCanvas::fillColor(float r, float g, float b, float a )
{
	_bFill = true;
	
	r *= 255;
	g *= 255;
	b *= 255;
	a *= 255;
	
	_fillColor = RGBA(r,g,b,a);
}	

void GLCanvas::strokeColor( float r, float g, float b, float a  )
{
	_bLine = true;
	
	r *= 255;
	g *= 255;
	b *= 255;
	a *= 255;
	
	_strokeColor = RGBA(r,g,b,a);
}	

void GLCanvas::strokeWidth( float w  )
{
	_bLine = true;
}	


void GLCanvas::noFill()
{
	_bFill = false;
}	

void GLCanvas::noStroke()
{
	_bLine = false;
}	

}