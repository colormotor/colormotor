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

#pragma once

namespace CMGUI
{

class BitMask
{
public:
	BitMask()
	:
	mask(0)
	{
	}
	
	BitMask( int w, int h )
	: 
	mask(0)
	{
		init(w,h);
	}
	
	~BitMask()
	{
	}
	
	void release()
	{
		if( mask != 0 )
			delete [] mask;
		mask = 0;
	}
	
	void init( int w, int h )
	{
		release();
		
		// get closest number of bytes
		int n = w*h;
		size = n/8;
		
		int rem = n-(size*8);
		if( rem > 0 )
			size++;
		
		mask = new char[size];
		width = w;
		height = h;
		
		memset(mask,0,size);
	}
	
	inline void clear()
	{
		memset(mask,0,size);
	}
	
	inline int getBit( int x, int y )
	{
		int i = y*width+x;
		int byte = i>>3;
		int bit = i-(byte<<3);
		
		if( mask[byte] & (1<<bit) )
			return 1;
			
		return 0;

	}
	
	inline void setBit( int x, int y )
	{
		int i = y*width+x;
		int byte = i>>3;
		int bit = i-(byte<<3);
		
		mask[byte] = mask[byte] | (1<<bit);
	} 
	
	inline void unsetBit(  int x, int y )
	{
		int i = y*width+x;
		int byte = i>>3;
		int bit = i-(byte<<3);
		
		mask[byte] = mask[byte] &~ (1<<bit);
	} 

	void setRect( int x, int y, int w, int h )
	{
		// could be optimized
		int x2 = x+w;
		if( x2 >= width )
			x2 = width-1;
		int y2 = y+h;
		if( y2 >= height )
			y2 = height-1;
			
		for( int i = y; i < y2; i++ )
			for( int j = x; j < x2; j++ )
				setBit(j,i);
	}
	
	void print()
	{
		printf("Bitmask:\n");
		for( int y = 0; y < height; y++ )
		{
			for( int x = 0; x < width; x++ )
			{
				printf("%d",getBit(x,y));
			}
			printf("\n");
		}
	}
	
private:
	char * mask;
	int width;
	int height;
	
	// in bytes
	int size;
};


}
