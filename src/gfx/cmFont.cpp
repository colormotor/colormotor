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

#include "gfx/CMGLInternal.h"
#include "gfx/CMVertexFormat.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "CMFont.h"
#include "CMGfxUtils.h"




namespace cm
{

#include "font_uni9.h"
//#include "font_small12.h"


Font::Font()
:
bmpData(0),
data(0),
texture(0),
bakedChars(0)
{
}

Font::Font( const unsigned char * buf )
:
bmpData(0),
data(0),
texture(0),
bakedChars(0)
{
	loadAsBinary(buf);
}

Font::Font( const unsigned char * data, float size, const char * charsToBake )
:
bmpData(0),
data(0),
texture(0)
{
	loadTTFData(data,size,charsToBake);
}

Font::~Font()
{
}


void Font::release()
{
	stbtt_bakedchar * stbdata = (stbtt_bakedchar*)data;
	SAFE_DELETE_ARRAY(stbdata);
	data = 0;
	SAFE_DELETE_ARRAY(bmpData);
	SAFE_DELETE(texture);
	SAFE_DELETE_ARRAY(bakedChars);
}

// not that optimal....
void Font::computeOptimalBitmapSize( int numBakedChars )
{
	int maxCharsPerRow = 512/fontSize;
	if(maxCharsPerRow > numBakedChars )
		maxCharsPerRow = numBakedChars;
		
	bmpWidth = maxCharsPerRow*fontSize;
	
	int numRows = 1;
	int n = numBakedChars-maxCharsPerRow;
	while(n>0)
	{
		numRows++;
		n-=maxCharsPerRow;
	}
	
	bmpHeight = (numRows+1)*fontSize;
}

bool Font::load( const char * path, float size, const char * charsToBake )
{
	unsigned char ttf_buffer[1<<20];
	FILE * f;
	
	f = fopen(path, "rb");
	if(!f)
	{
		debugPrint("Could not load font %s\n",path);
		return false;
	}
	
	size_t nread = fread(ttf_buffer, 1, 1<<20, f );
	//std::string dump = hexDump( ttf_buffer, nread );
	//printf("%s\n",dump.c_str());
	fclose(f);
	
	return loadTTFData(ttf_buffer,size,charsToBake);
	
}

bool Font::loadTTFData( const unsigned char * ttf_buffer, float size, const char * charsToBake )
{
	release();
	
	fontSize = size;
	
	numChars = 95;
	startChar = 32;
	
	data = new stbtt_bakedchar[numChars];
	
	computeOptimalBitmapSize( charsToBake?strlen(charsToBake):numChars );
	
	bmpData = new unsigned char[bmpWidth*bmpHeight];
	
	if(charsToBake)
	{
		bakedChars = new char[strlen(charsToBake)+1];
		strcpy(bakedChars,charsToBake);
		stbtt_BakeFontBitmapWithChars(ttf_buffer,0, size, bmpData,bmpWidth,bmpHeight, startChar,numChars,charsToBake, (stbtt_bakedchar*)data);
	}
	else
		stbtt_BakeFontBitmap(ttf_buffer,0, size, bmpData,bmpWidth,bmpHeight, startChar,numChars, (stbtt_bakedchar*)data);
   
	return true;
	
}


int Font::getStringLengthForSize( const char * text, float size )
{
	stbtt_bakedchar * d = (stbtt_bakedchar*)data;
	
	float sz = 0;
	int n = strlen(text);
	for( int i = 0; i < n; i++ )
	{
		int ind = text[i]-startChar;
		if( ind >= numChars )
			continue;
		sz += d[ind].xadvance;
		if( sz > size )
			return i;
	}
	
	return n;
}

float Font::getStringSize( const char * text, int n )
{
	if(n==-1)
		n = strlen(text);
		
	float size = 0;

	stbtt_bakedchar * d = (stbtt_bakedchar*)data;
	for( int i = 0; i < n; i++ )
	{
		int ind = text[i]-startChar;
		if( ind >= numChars )
			continue;
		stbtt_bakedchar & bb = d[ind];
		size += d[ind].xadvance;
	}
	
	return size;
}

void Font::createGLData()
{
	texture = new Texture();
	texture->create( bmpData, bmpWidth, bmpHeight, Texture::L8 );
}

void Font::format( float x, float y,  const char * str, ... )
{
	static char txt[4096];
	va_list	parameter;
	va_start(parameter,str);
	vsnprintf(txt,4096,str,parameter);
	va_end(parameter);
	print(x,y,txt);
}

void Font::print( float x, float y, const char * text )
{
	if( !texture )
		createGLData();
	
	float startx = x;
	float starty = y;
	
	texture->bind(0);
	
	beginVertices(TRIANGLELIST);
	
	int i;
	stbtt_bakedchar * tdata = (stbtt_bakedchar*)data;
	
	while (*text) 
	{
	  char c = *text;
	  if( c == '\n' ||
		  c == '\r' )
	  {
		  x = startx;
		  y += fontSize+5;
	  }
	  else
	  if (c >= 32 && c < 128) 
	  {
		 stbtt_aligned_quad q;
		 
		 i = c-32;
		 if( tdata[i].baked )
		 {
			 stbtt_GetBakedQuad(tdata, bmpWidth,bmpHeight, i, &x,&y,&q,1,1);//1=opengl,0=old d3d	
			 		 
			 uv(q.s0*texture->info.maxU,q.t0*texture->info.maxV); vertex(q.x0,q.y0,0);
			 uv(q.s1*texture->info.maxU,q.t0*texture->info.maxV); vertex(q.x1,q.y0);
			 uv(q.s1*texture->info.maxU,q.t1*texture->info.maxV); vertex(q.x1,q.y1);
			 
			 uv(q.s0*texture->info.maxU,q.t0*texture->info.maxV); vertex(q.x0,q.y0,0);
			 uv(q.s1*texture->info.maxU,q.t1*texture->info.maxV); vertex(q.x1,q.y1);
			 uv(q.s0*texture->info.maxU,q.t1*texture->info.maxV); vertex(q.x0,q.y1);
		 }
	  }
	  ++text;
   }
   
	endVertices();
	
	texture->unbind();
}

void Font::printFlipped( float x, float y, const char * text )
{
	if( !texture )
		createGLData();
	
	float startx = x;
	float starty = y;
	
	texture->bind(0);
	
	beginVertices(TRIANGLELIST);
	
	int i;
	stbtt_bakedchar * tdata = (stbtt_bakedchar*)data;
	
	while (*text) 
	{
		char c = *text;
		if( c == '\n' ||
		   c == '\r' )
		{
			x = startx;
			y -= fontSize+5;
		}
		else
			if (c >= 32 && c < 128) 
			{
				stbtt_aligned_quad q;
				
				i = c-32;
				if( tdata[i].baked )
				{
					stbtt_GetBakedQuad(tdata, bmpWidth,bmpHeight, i, &x,&y,&q,1,1);//1=opengl,0=old d3d	
					
					uv(q.s0*texture->info.maxU,q.t1*texture->info.maxV); vertex(q.x0,q.y0,0);
					uv(q.s1*texture->info.maxU,q.t1*texture->info.maxV); vertex(q.x1,q.y0);
					uv(q.s1*texture->info.maxU,q.t0*texture->info.maxV); vertex(q.x1,q.y1);
					
					uv(q.s0*texture->info.maxU,q.t1*texture->info.maxV); vertex(q.x0,q.y0,0);
					uv(q.s1*texture->info.maxU,q.t0*texture->info.maxV); vertex(q.x1,q.y1);
					uv(q.s0*texture->info.maxU,q.t0*texture->info.maxV); vertex(q.x0,q.y1);
				}
			}
		++text;
	}
	
	endVertices();
	
	texture->unbind();
}



void Font::substring( float x, float y, const char * text, int from, int to )
{
	if( !texture )
		createGLData();
	
	texture->bind(0);
		
	gfx::beginVertices(TRIANGLELIST);
	
	int i;
	stbtt_bakedchar * tdata = (stbtt_bakedchar*)data;
	
	int c = 0;
	while (*text) {
	  if (*text >= 32 && *text < 128) {
		 stbtt_aligned_quad q;
		 
		 i = *text-32;
		 if( tdata[i].baked )
		 {
			 stbtt_GetBakedQuad(tdata, bmpWidth,bmpHeight, i, &x,&y,&q,1,1);//1=opengl,0=old d3d
			 
			 if(c>=from&&c<to)
			 {
				 gfx::uv(q.s0*texture->info.maxU,q.t0*texture->info.maxV); gfx::vertex(q.x0,q.y0,0);
				 gfx::uv(q.s1*texture->info.maxU,q.t0*texture->info.maxV); gfx::vertex(q.x1,q.y0);
				 gfx::uv(q.s1*texture->info.maxU,q.t1*texture->info.maxV); gfx::vertex(q.x1,q.y1);
				 
				 gfx::uv(q.s0*texture->info.maxU,q.t0*texture->info.maxV); gfx::vertex(q.x0,q.y0,0);
				 gfx::uv(q.s1*texture->info.maxU,q.t1*texture->info.maxV); gfx::vertex(q.x1,q.y1);
				 gfx::uv(q.s0*texture->info.maxU,q.t1*texture->info.maxV); gfx::vertex(q.x0,q.y1);
			}
			
			++c;
		 }
	  }
	  ++text;
   }
   
	gfx::endVertices();
	
	texture->unbind();
}


unsigned char * Font::getAsBinary( int  * size_ )
{
	int numBmpBytes = bmpWidth * bmpHeight;
	int numBakedChars = bakedChars?strlen(bakedChars):numChars;
	int numDataBytes = numBakedChars * sizeof(stbtt_bakedchar);
	
	// float font size
	int size = 4;
	// DWORD startChar
	size += 4;
	// DWORD numChars
	size += 4;
	// DWORD bmpWidth
	size += 4;
	// DWORD bmpHeight
	size += 4;
	// DWORD numBmpBytes
	size += 4;
	// DWORD length of baked chars string
	size += 4;
	
	if( bakedChars )
		size += strlen(bakedChars)+1; // baked chars
	
	// buffer font data ( numChars * sizeof(stbtt_bakedchar) )
	size += numDataBytes;
	// buffer bmp data ( numBmpBytes )
	size += numBmpBytes;
	
	unsigned char * buf = new unsigned char [size];
	unsigned char * bufStart = buf;
	
	writeFLOAT( fontSize, buf ); buf+=4;
	writeDWORD( startChar, buf ); buf+=4;
	writeDWORD( numChars, buf ); buf+=4;
	writeDWORD( bmpWidth, buf ); buf+=4;
	writeDWORD( bmpHeight, buf ); buf+=4;
	writeDWORD( numBmpBytes, buf ); buf+=4;
	
	int n = bakedChars?strlen(bakedChars):0;
	writeDWORD( n, buf); buf+=4;
	
	if( bakedChars )
	{
		// write bakedChars string
		memcpy( buf, bakedChars, strlen(bakedChars)+1 );
		buf+=strlen(bakedChars)+1;
		
		stbtt_bakedchar * tdata = (stbtt_bakedchar*)data;
		// copy only baked chars
		for( int i = 0; i < numBakedChars; i++ )
		{
			int c = bakedChars[i]-startChar;
			memcpy( buf, &tdata[c], sizeof(stbtt_bakedchar) );
			buf+=sizeof(stbtt_bakedchar);
		}
	}
	else
	{
		// copy all char data
		memcpy( buf, data, numDataBytes );
		buf += numDataBytes;
	}
	
	memcpy( buf, bmpData, numBmpBytes );
	
	if(size_)
		*size_ = size;
		
	return bufStart;
}

bool Font::loadAsBinary( const unsigned char * inBuf )
{
	release();
	
	unsigned char * buf = (unsigned char*)inBuf;
	
	fontSize = readFLOAT(buf); buf+=4;
	startChar = readDWORD(buf); buf+=4;
	numChars = readDWORD(buf); buf+=4;
	bmpWidth = readDWORD(buf); buf+=4;
	bmpHeight = readDWORD(buf); buf+=4;
	int numBmpBytes = readDWORD(buf); buf+=4;
	int numBakedChars = readDWORD(buf); buf+=4;
	
	if( numBakedChars )
	{
		// read baked chars string
		bakedChars = new char[numBakedChars+1];
		memcpy( bakedChars, buf, numBakedChars+1 );
		buf+=numBakedChars+1;
	}
	else
	{
		bakedChars = 0;
		numBakedChars = numChars;
	}
	
	int numDataBytes = numBakedChars * sizeof(stbtt_bakedchar);
	
	data = new stbtt_bakedchar[numChars];
	bmpData = new unsigned char[bmpWidth*bmpHeight];
	
	stbtt_bakedchar * tdata = (stbtt_bakedchar*)data;
	
	if( bakedChars )
	{
		// load only characters that have been baked
		for( int i = 0; i < numChars; i++ )
		{
			tdata[i].baked = 0;
		}
		
		for( int i = 0; i < numBakedChars; i++ )
		{
			int c = bakedChars[i]-startChar;
			memcpy( &tdata[c], buf, sizeof(stbtt_bakedchar) );
			buf+=sizeof(stbtt_bakedchar);
		}
	}
	else
	{
		// load everything
		memcpy( data, buf, numDataBytes );
		buf+=numDataBytes;
	}
	
	memset( bmpData, 0, bmpWidth*bmpHeight );
	memcpy( bmpData, buf, numBmpBytes );
	
	return true;
}
		
bool Font::saveHeader( const char * path, const char * varName )
{
	int size;
	unsigned char * binData = getAsBinary(&size);
	
	FILE * f = fopen(path,"w");
	
	
	fprintf(f,"// Binary font:\n");
	fprintf(f,"// size %g\n",fontSize);
	if(bakedChars)
		fprintf(f,"// characters %s\n",bakedChars);
	
	std::string str = hexDump( binData, size );

	fprintf(f,"const unsigned char %s[] = {\n%s\n};\n\n",varName,str.c_str());
	
	fclose(f);
	
	delete [] binData;
	
	return true;
}
		
		
}
