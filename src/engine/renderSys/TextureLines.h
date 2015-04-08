/*
 *  TextureLines.h
 *
 *  Created by Daniel Berio on 9/22/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

/*
 *  CMTextureLines.h
 *
 *  Created by Daniel Berio on 6/9/10.
 *  http://www.enist.org
 *  Copyright 2010. All rights reserved.
 *
 */

#pragma once

#include "engine/engineIncludes.h"

namespace cm
{

// Draws textured lines 
class TextureLines
{
	public:		
		TextureLines();
		~TextureLines();
		
		void release();
		void init( int nLines = 100 );
		void begin( const Matrix4x4 & worldView );
		
		void smudgeLine( float w, const cm::Vec3 & a, const cm::Vec3 & b, const cm::Color & clr1, const cm::Color & clr2  );
		inline void smudgeLine( float w, const cm::Vec3 & a, const cm::Vec3 & b, const cm::Color & clr = cm::Color::white() );
		
		void line( float w, const cm::Vec3 & a, const cm::Vec3 & b, const cm::Color & clr1, const cm::Color & clr2  );
		inline void line( float w, const cm::Vec3 & a, const cm::Vec3 & b, const cm::Color & clr = cm::Color::white() );

		void setTextureData( Texture * tex )
		{
			maxu=tex->info.maxU;
			maxv=tex->info.maxV;
		}
		
		void end();
	protected:
		float maxu;
		float maxv;
		
		Matrix4x4		  invWorldView;
};

inline void TextureLines::line( float w, const cm::Vec3 & a, const cm::Vec3 & b, const cm::Color & clr )
{	line(w,a,b,clr,clr); }
	
inline void TextureLines::smudgeLine( float w, const cm::Vec3 & a, const cm::Vec3 & b, const cm::Color & clr )
{	smudgeLine(w,a,b,clr,clr); }

}
