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

#include "gfx/CMGfxIncludes.h"
#include "gfx/CMTexture.h"

namespace cm 
{

class Font
{
	public:
		Font();
		
		/// Construct font from ttf data
		Font( const unsigned char * ttf_data, float size, const char * charsToBake = 0 );
		
		/// Construct font from binary data
		Font( const unsigned char * buf );
		
		~Font();
		
		void release();
		
		bool load( const char * path, float size, const char * charsToBake = 0 );
		
		float getSize() const { return fontSize; }

		/// Load from ttf buffer
		bool loadTTFData( const unsigned char * ttf_data, float size, const char * charsToBake = 0 );
		
		float getStringSize( const char * text, int len = -1 );
		int getStringLengthForSize( const char * text, float size );
		
		Texture * getTexture() const { return texture; }
		
		int getBitmapWidth() const { return bmpWidth; }
		int getBitmapHeight() const { return bmpHeight; }
		
	//	void printLength( float x, float y, con
		void substring( float x, float y, const char * text, int from, int to );
		void print( float x, float y, const char * text );
		void format( float x, float y,  const char * str, ... );
		void printFlipped( float x, float y, const char * text );
	
		unsigned char * getAsBinary( int * size = 0 );
		bool loadAsBinary( const unsigned char * buf );
		
		/// Save a header file with font data
		bool saveHeader( const char * path, const char * varName );
		void createGLData();
		
	private:
		void computeOptimalBitmapSize( int numBakedChars );
		
		
		
		float fontSize;
		int bmpWidth;
		int bmpHeight;
		int numChars;
		int startChar;
		
		char* bakedChars;
		void * data;
		unsigned char * bmpData;
		Texture * texture;
};

namespace Fonts
{
	extern Font uni9; 
	//extern Font small12;
};

}