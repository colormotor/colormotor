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

#include "cmCanvas.h"
#include "cmMath.h"

namespace cm
{

void Canvas::textf( float x, float y, const char * fmt, ... )
{	
	char txt[2048];
	va_list	parameter;
	va_start(parameter,fmt);
	vsnprintf(txt,2047,fmt,parameter);
	va_end(parameter);
	text(x,y,txt);
}


static float curve( float t, float p1, float ctrl, float end )
{
	float d1 = p1+(ctrl-p1)*t;
	float d2 = ctrl+(end-ctrl)*t;
	return d1+(d2-d1)*t;
}

void Canvas::curveVertex( float ctrlX, float ctrlY,  float anchorX, float anchorY )
{
#define curveSubdivision	20
	float inc = 1.0f / curveSubdivision;
	float t = inc;
	
	Vec3 p;
	getLastPolyPoint((float*)p);
	
	for( int i = 1; i < curveSubdivision-1; i++ )
	{
		float x = curve(t,p.x,ctrlX,anchorX);
		float y = curve(t,p.y,ctrlY,anchorY);
		vertex(x,y);
		t+=inc;
	}
	
	vertex(anchorX,anchorY);
}

static Vec2 getArcControlPoint( float x1, float y1, float t1, float x2, float y2, float t2 )
{
	float x12  = x2 - x1;
	float y12  = y2 - y1;
	
	float l12  = sqrt(x12 * x12 + y12 * y12);
	float t12  = atan2(y12, x12);

	float l13  = l12 * sin(t2 - t12) / sin(t2 - t1);

	float x3  = x1 + l13 * cos(t1);
	float y3  = y1 + l13 * sin(t1);

	return Vec2(x3, y3);
}


/// adapted source code from http://www.d-project.com/
void Canvas::arc( float x, float y, float r, float rt1, float rt2 )
{

	int div = cm::max(1, floor(fabs(rt1 - rt2) * 0.2) );

	float lx,ly,lt;
	
	float t1=radians(rt1);
	float t2=radians(rt2);
	
	#define RIGHT_ANGLE PID2
	
	for(int i = 0; i <= div; i++ )
	{
		float ct = t1 + (t2 - t1) * i / div;
		float cx = cos(ct) * r + x;    
		float cy = sin(ct) * r + y;    

		if (i == 0) 
		{
			vertex(cx,cy);
		} 
		else 
		{
			//Vec2 cp = getArcControlPoint(lx, ly, lt + RIGHT_ANGLE, cx, cy, ct + RIGHT_ANGLE); 
			//curveVertex(cp.x, cp.y,cx,cy);   
			vertex(cx,cy);         
		}

		lx = cx;
		ly = cy;
		lt = ct;
	}
}


/// draw a filled arc
void Canvas::drawFilledArc( float x, float y,  float r1, float r2, float t1, float t2 )
{
	beginPoly();
	arc(x, y, r1, t1, t2 );
	arc(x, y, r2, t2, t1 );
	endPoly(true);
}

/// draw a pie
void Canvas::drawPie( float x, float y,  float r, float t1, float t2 )
{
	beginPoly();
	vertex(x,y);
	arc(x, y, r, t1, t2);
	endPoly(true);
}

}