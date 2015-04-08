/*
 *  CMTextureLines.cpp
 *
 *  Created by Daniel Berio on 6/9/10.
 *  http://www.enist.org
 *  Copyright 2010. All rights reserved.
 *
 */

#include "TextureLines.h"


namespace cm
{

TextureLines::TextureLines()
{
	maxu=maxv=1.0;
}

TextureLines::~TextureLines()
{
	release();
}


void TextureLines::release()
{
}

void TextureLines::begin( const Matrix4x4 & worldView )
{
	invWorldView = worldView.inverted();
	gfx::beginVertices(TRIANGLELIST);
}


void TextureLines::smudgeLine( float w, const cm::Vec3 & a, const cm::Vec3 & b, const cm::Color & clr1, const cm::Color & clr2   )
{
	cm::Vec3 d = b-a;
	
	cm::Vec3 v = invWorldView.z();
	cm::Vec3 u = cross(d,v);
	u.normalize();
	
	u*=w*0.5;
	
	cm::Vec3 t1[3] =
	{	a - u, a + u, b + u };
	cm::Vec3 t2[3] =
	{	a - u, b + u, b - u };
	
	
	
	cm::Vec2 uv[4] = 
	{
		cm::Vec2(0,0), cm::Vec2(0,maxv), cm::Vec2(maxu,maxv), cm::Vec2(maxu,0)
	};
	
	unsigned int c1 = clr1.bgra();
	unsigned int c2 = clr2.bgra();

	gfx::color(c1);
	gfx::uv(uv[0].x,uv[0].y);
	gfx::vertex(&t1[0][0]);
	
	gfx::color(c1);
	gfx::uv(uv[1].x,uv[1].y);
	gfx::vertex(&t1[1][0]);
	
	gfx::color(c2);
	gfx::uv(uv[2].x,uv[2].y);
	gfx::vertex(&t1[2][0]);

	
	gfx::color(c1);
	gfx::uv(uv[0].x,uv[0].y);
	gfx::vertex(&t2[0][0]);
	
	gfx::color(c2);
	gfx::uv(uv[2].x,uv[2].y);
	gfx::vertex(&t2[1][0]);
	
	gfx::color(c2);
	gfx::uv(uv[3].x,uv[3].y);
	gfx::vertex(&t2[2][0]);
		
}



void TextureLines::line( float w, const cm::Vec3 & a, const cm::Vec3 & b, const cm::Color & clr1, const cm::Color & clr2   )
{
	cm::Vec3 d = b-a;
	
	cm::Vec3 v = invWorldView.z();
	cm::Vec3 u = cross(d,v);
	u.normalize();
	//u(0,0,1);
	float hw = w*0.5;
	
	u*=hw;
	
	
	float mag = length(d);
	cm::Vec3 nd = d/mag;
	
	// cap -- segment -- cap 
	
	cm::Vec3 segs[4] =
	{
		a-nd*hw, a,
		b,b+nd*hw
	};
	
	cm::Vec3 cap1[4] =
	{
		segs[0] - u, segs[0] + u, segs[1] + u, segs[1] - u
	};
	
	cm::Vec3 mid[4] =
	{
		segs[1] - u, segs[1] + u, segs[2] + u, segs[2] - u
	};
	
	cm::Vec3 cap2[4] =
	{
		segs[2] - u, segs[2] + u, segs[3] + u, segs[3] - u
	};


	float hu = 0.5 * maxu;
	
	unsigned int c1 = clr1.bgra();
	unsigned int c2 = clr2.bgra();

	// cap1
	gfx::color(c1);
	gfx::uv(0.0,0.0);
	gfx::vertex(&cap1[0][0]);
	
	gfx::uv(0.0,maxv);
	gfx::vertex(&cap1[1][0]);
	
	gfx::uv(hu,maxv);
	gfx::vertex(&cap1[2][0]);
	
	
	gfx::uv(0.0,0.0);
	gfx::vertex(&cap1[0][0]);
	
	gfx::uv(hu,maxv);
	gfx::vertex(&cap1[2][0]);
	
	gfx::uv(hu,0.0);
	gfx::vertex(&cap1[3][0]);
	
	
	// middle line
	gfx::color(c1);
	gfx::uv(hu,0.0);
	gfx::vertex(&mid[0][0]);
	
	gfx::color(c1);
	gfx::uv(hu,maxv);
	gfx::vertex(&mid[1][0]);
	
	gfx::color(c2);
	gfx::uv(hu,maxv);
	gfx::vertex(&mid[2][0]);
	
	
	gfx::color(c1);
	gfx::uv(hu,0.0);
	gfx::vertex(&mid[0][0]);
	
	gfx::color(c2);
	gfx::uv(hu,maxv);
	gfx::vertex(&mid[2][0]);
	
	gfx::color(c2);
	gfx::uv(hu,0.0);
	gfx::vertex(&mid[3][0]);
	
	
	// cap2
	gfx::color(c2);
	gfx::uv(hu,0.0);
	gfx::vertex(&cap2[0][0]);
	
	gfx::uv(hu,maxv);
	gfx::vertex(&cap2[1][0]);
	
	gfx::uv(maxu,maxv);
	gfx::vertex(&cap2[2][0]);
	
	
	gfx::uv(hu,0.0);
	gfx::vertex(&cap2[0][0]);
	
	gfx::uv(maxu,maxv);
	gfx::vertex(&cap2[2][0]);
	
	gfx::uv(maxu,0.0);
	gfx::vertex(&cap2[3][0]);
	

}


void TextureLines::end()
{
	gfx::endVertices();
}

}

