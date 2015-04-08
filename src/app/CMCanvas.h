/*
 *  CMCanvas.h
 *  resapiTest
 *
 *  Created by Daniel Berio on 4/19/10.
 *  Copyright 2010 http://www.enist.org. All rights reserved.
 *
 */

#pragma once

#include "cmCore.h"


namespace cm
{

class Canvas
{
public:
	Canvas(){}
	virtual ~Canvas() {}
	
	virtual bool init( int w, int h, void * data = 0 ) { return false; }
	
	virtual void image( void * data, int w, int h ) {} // does ovveriding with different data type override?
	virtual void blitImage( void * data, float x, float y, float w, float h, float imgx, float imgy ) {}
	
	virtual void patternRect( void * data, float x, float y, float w, float h, float xscale = 1, float yscale = 1 ) {}
	
	virtual void circle( float x, float y, float radius ) {}
	virtual void ellipse( float x0, float y0, float x1, float y1  ) {}
	virtual void rect( const float * r ) { rect(r[0],r[1],r[2]-r[0],r[3]-r[1]); }
	virtual void rect( float x, float y, float w, float h ) {}
	virtual void line( float x0, float y0, float x1, float y1 ) {}
	virtual void verticalGradient( const float * r, const float * clr1, const float * clr2 ) {}
	virtual void horizontalGradient( const float * r, const float * clr1, const float * clr2 ) {}
	
	virtual void bezier( float ctrlX, float ctrlY, float ctrlZ, float anchorX, float anchorY, float anchorZ ) {}
	
	virtual void textSubstring( float x, float y, const char * str, int from, int to ) {}
	virtual void text( float x, float y, const char * str ) {}
	virtual void textVertical( float x, float y, const char * str ) {}
	
	virtual void textf( float x, float y, const char * fmt, ... );
	
	virtual int getStringLengthForSize( const char * text, float size ) { return 0; }
	virtual float getStringExtent( const char * str, int n = -1 ) { return 0.0f; }
	virtual float getTextHeight() { return 0.0f; }
	
	virtual void getLastPolyPoint( float * pt ) {}
	
	virtual void beginPoly( ) {}
	virtual void endPoly(bool close = true) {}
	
	virtual void vertex( float x, float y ) {}
	void curveVertex( float ctrlX, float ctrlY,  float anchorX, float anchorY );	
	void arc( float x, float y, float r, float rt1, float rt2 );


	/// draw a filled arc
	void drawFilledArc( float x, float y,  float r1, float r2, float t1, float t2 );
	void drawPie( float x, float y,  float r, float t1, float t2 );

	void textColor( const float * clr ) { textColor(clr[0],clr[1],clr[2],clr[3]); }
	void fillColor( const float * clr ) { fillColor(clr[0],clr[1],clr[2],clr[3]); }
	void strokeColor( const float * clr ) { strokeColor(clr[0],clr[1],clr[2],clr[3]); }
	
	virtual void textColor( float r, float g, float b, float a = 1.0f ) {}
	virtual void fillColor( float r, float g, float b, float a = 1.0f ) {}
	virtual void strokeColor( float r, float g, float b, float alpha = 1.0f ) {}
	
	virtual void strokeWidth( float w ) {}
	
	virtual void noFill() {}
	virtual void noStroke() {}
	
	virtual void setup() {}

	
	
	int	width;
	int height;
	
};

}

