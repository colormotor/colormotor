/*
 *  OFCanvas.h
 *  resapiTest
 *
 *  Created by Daniel Berio on 4/19/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */



#include "cmCanvas.h"
#include "../cmGfx.h"

namespace cm
{
	
class GLCanvas : public Canvas, public UsingTriangulator
{
public:
	GLCanvas();
	~GLCanvas();
	
	bool enableTriangulation( bool flag ) { _triangulate = flag; return true; }
	
	void	onTriangulatedVertexIndex(int i);
	
	bool init( int w, int h, void * data = 0 );
	
	void image( void * data, float x, float y, float w = 0.0f, float h = 0.0f );
	void blitImage( void * data, float x, float y, float w, float h, float imgx, float imgy );
	
	void patternRect( void * data, float x, float y, float w, float h, float xscale = 1, float yscale = 1 );
		
	void circle( float x, float y, float radius );
	void ellipse( float x0, float y0, float x1, float y1  );
	void rect( float x, float y, float w, float h );
	void line( float x0, float y0, float x1, float y1 );
	void rect( const float * r ) { rect(r[0],r[1],r[2]-r[0],r[3]-r[1]); }
	
	void verticalGradient( const float * r, const float * clr1, const float * clr2 );
	void horizontalGradient( const float * r, const float * clr1, const float * clr2 );

	void text( float x, float y, const char * str );
	void textSubstring( float x, float y, const char * str, int from, int to );
	void textVertical( float x, float y, const char * str );
	
	int getStringLengthForSize( const char * text, float size );
	float getStringExtent( const char * str, int n = -1 );
	float getTextHeight();
	
	void getLastPolyPoint( float * pt );
	void beginPoly();
	void endPoly(  bool close = true  );
	
	void vertex( float x, float y );
//	void curveVertex( float x, float y );
//	void bezierVertex( float x, float y , float cx, float cy );
	
	// todo curve + bezier
	
	void textColor( float r, float g, float b, float a = 1.0f );
	void fillColor( float r, float g, float b, float a = 1.0f );
	void strokeColor( float r, float g, float b, float a = 1.0f );
	void strokeWidth( float w );
	
	void textColor( const float * clr ) { textColor(clr[0],clr[1],clr[2],clr[3]); }
	void fillColor( const float * clr ) { fillColor(clr[0],clr[1],clr[2],clr[3]); }
	void strokeColor( const float * clr ) { strokeColor(clr[0],clr[1],clr[2],clr[3]); }
	
	
	void noFill();
	void noStroke();

	
	void setup();
	
	cm::Font * getFont();
	void setFont( cm::Font * font );
private:
	enum 
	{
		MAX_POINTS = 10000
	};
	
	struct Vertex
	{
		struct
		{
			float x;
			float y;
		} pos;
		
		unsigned int fillColor;
		unsigned int strokeColor;
	};
	
	bool	_triangulate;
	
	Vertex pts[MAX_POINTS];
	unsigned short indices[MAX_POINTS];
	
	int	numPts;
	int numInds;
	
	unsigned int _textColor;
	unsigned int _fillColor;
	unsigned int _strokeColor;
		
	bool _bFill;
	bool _bLine;
	
	cm::Font * font;
};
	
}


