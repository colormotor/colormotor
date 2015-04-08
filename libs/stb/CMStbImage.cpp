/*
 *  CMStbImage.cpp
 *
 *  Created by Daniel Berio on 9/30/10.
 *  http://www.enist.org
 *  Copyright 2010. All rights reserved.
 *
 */


#include "stb_image.h"
#include "stb_image_write.h"

#include "CMGfx.h"

namespace cm
{
	
void swizzleRGB( unsigned char * data, int w, int h, int numComponents )
{
	if( numComponents < 3 )
		return;
		
	unsigned char * c = data;
	
	for( int y = 0; y < h; y++ )
	{
		c = &data[ y*w*numComponents ];
		for( int x = 0; x < w; x++ )
		{
			int r = c[0];
			int b = c[2];
			c[0] = b;
			c[2] = r;
			c += numComponents;
		}
	}
}

void flip( unsigned char * data, int w, int h, int numComponents )
{
	int pitch = w*numComponents;
	int sz = pitch*h;
	
	unsigned char * tmp = new unsigned char[sz];
	memcpy( tmp, data, sz );
	
	for( int y = 0; y < h; y++ )
	{
		unsigned char * src = &tmp[(h-1-y)*pitch];
		unsigned char * dst = &data[y*pitch];
		memcpy( dst,src,pitch );
	}
	
	delete [] tmp;
}

unsigned char * loadImage( const char * filename, int * width, int * height, int * numComponents, int requestedNumComponents )
{
	FILE * f = fopen(filename,"rb");
	if( !f ) 
	{
		debugPrint("Could not open file %s\n",filename);
		return 0;
	}
	
	fseek(f, 0L, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0L, SEEK_SET);

	unsigned char * buf = new unsigned char[len];
	fread( buf, 1, len, f );
	
	unsigned char * data = stbi_load_from_memory( buf,len,width,height,numComponents,requestedNumComponents );
	
	bool res = true;
	
	if(data==0)
	{
		debugPrint("STB error: %s\n",stbi_failure_reason());
		res = true;
	}
	
	delete [] buf;
	fclose(f);
	
	if( res )
	{
		int n = (requestedNumComponents)?requestedNumComponents:*numComponents;
		
		if( n==4 )
			swizzleRGB(data,*width,*height,n);
		//flip(data,*width,*height,n);
	}
	
	return data;
}

//////////////////////////////////////////////////////////////////////

/// Load a texture from an image
bool textureFromImage( cm::Texture * tex, const char * filename, int requestedNumComponents , int mipLevels )
{
	int imw,imh;
	int numComp = 0;
	
	unsigned char * data = loadImage( filename, &imw,&imh,&numComp,requestedNumComponents );
	
	if( data )
	{
		int n = requestedNumComponents?requestedNumComponents:numComp;
		Texture::FORMAT fmt;
		switch( n )
		{
			case 1:
				fmt = Texture::L8;
				//printf("L8 %s\n",hexDump(data,imw*imh*1).c_str());
				break;
			case 3:
				fmt = Texture::R8G8B8;
				//printf("R8G8B8 %s\n",hexDump(data,imw*imh*3).c_str());
				break;
				
			case 4:
				fmt = Texture::A8R8G8B8;
				//printf("A8R8G8B8 %s\n",hexDump(data,imw*imh*4).c_str());
				break;
				
			default:
				debugPrint("textureFromImage: Unknown format!\n");
				free(data);
				return 0;
		} 
		
		
		if( !tex->create(data,imw,imh,fmt, mipLevels ) )
		{
			debugPrint("textureFromImage: could not create texture!\n");
			free(data);
			return false;
		}
		
		free(data);
		
		return true;
	}
	else
		return false;
}

Texture * textureFromImage( const char * filename, int requestedNumComponents, int mipLevels  )
{
	int imw,imh;
	int numComp = 0;
	
	unsigned char * data = loadImage( filename, &imw,&imh,&numComp,requestedNumComponents );
		
	if( data )
	{
		int n = requestedNumComponents?requestedNumComponents:numComp;
		Texture::FORMAT fmt;
		switch( n )
		{
			case 1:
				fmt = Texture::L8;
				break;
			case 3:
				fmt = Texture::R8G8B8;
				break;
				
			case 4:
				fmt = Texture::A8R8G8B8;
				break;
				
			default:
				debugPrint("textureFromImage: Unknown format!\n");
				free(data);
				return 0;
		} 
		
		Texture * t = new Texture();
		
		if( !t->create(data,imw,imh,fmt, mipLevels ) )
		{
			debugPrint("textureFromImage: could not create texture!\n");
			free(data);
			delete t;
			return 0;
		}
		
		free(data);
		
		return t;
	}
	else
		return 0;
}


//////////

bool writeToPng( const char * filenmame, int w, int h, int numComp, const void * data, int strideInBytes )
{
	if(strideInBytes == 0)
		strideInBytes = w*numComp;
	return stbi_write_png( filenmame, w,h,numComp,data,strideInBytes );
}

//////////

bool writeTextureToPng( const char * filenmame, Texture * tex, int startx , int starty )
{
	int numComp = 0;
	
	switch( tex->getFormat() )
	{
		case Texture::L8:
			numComp = 1;
			break;
			
		case Texture::R8G8B8:
			numComp = 3;
			break;
			
		case Texture::A8R8G8B8:
			numComp = 4;
			break;
			
		default:
			debugPrint("Unsupported texture format!\n");
			return false;
	}
	
	int stride = tex->getHardwareWidth()*numComp;
	
	char * data = (char*)tex->lock(Texture::LOCK_READ);
	
	data+=(starty*stride)+startx*numComp;
	bool res = stbi_write_png( filenmame, tex->getHardwareWidth(),tex->getHardwareHeight(),numComp,data,stride );
	tex->unlock();
	
	return res;
}

//////////

bool writeToTga( const char * filenmame, int w, int h, int numComp, const void * data )
{
	return (bool)stbi_write_tga( filenmame, w,h,numComp,data );
}

//////////

bool writeTextureToTga( const char * filenmame, Texture * tex )
{
	int numComp = 0;
	
	switch( tex->getFormat() )
	{
		case Texture::L8:
			numComp = 1;
			break;
			
		case Texture::R8G8B8:
			numComp = 3;
			break;
			
		case Texture::A8R8G8B8:
			numComp = 4;
			break;
			
		default:
			debugPrint("Unsupported texture format!\n");
			return false;
	}
	
	int stride = tex->getHardwareWidth()*numComp;
	
	char * data = (char*)tex->lock(Texture::LOCK_READ);
	bool res = stbi_write_tga( filenmame, tex->getHardwareWidth(),tex->getHardwareHeight(),numComp,data );
	tex->unlock();
	
	return res;
}


}