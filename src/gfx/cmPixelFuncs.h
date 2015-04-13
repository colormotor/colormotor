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

/// This file just contains pixel callbacks for automatically writing pixels in textures
/// \todo getter funcs.. like Color getPixel...( x,y,z,...)

#define MAKERGBA(r,g,b,a) ((unsigned int)((((unsigned char)(a*255.0f)&0xff)<<24)|(((unsigned char)(r*255.0f)&0xff)<<16)|(((unsigned char)(g*255.0f)&0xff)<<8)|((unsigned char)(b*255.0f)&0xff)))


/// 32 bit color (RGBA)
void	pixelA8R8G8B8 ( void * out, int x, int y, int z, int pitch, int slicepitch, float r, float g, float b, float a )
{
	unsigned int * dst = (unsigned int  *)((unsigned char*)out + z*slicepitch + y*pitch + x * 4);
	*dst = MAKERGBA(r,g,b,a);//(int32)clr;
}

/// 8 bit color (grayscale) just reads r component
void	pixelL8 ( void * out, int x, int y,int z, int pitch, int slicepitch, float r, float g, float b, float a )
{
	unsigned char *dst = (unsigned char *)out + z*slicepitch + y*pitch + x;
	*dst = r*255;
}

/// 4 float component color
void	pixelA32B32G32R32F( void * out, int x, int y,int z,  int pitch, int slicepitch, float r, float g, float b, float a )
{
	float *dst = (float *)((unsigned char*)out + z*slicepitch + y*pitch + x * (sizeof(float)*4));
	*dst++ = r;
	*dst++ = g;
	*dst++ = b;
	*dst++ = a;
}

/// 4 half float component color
void	pixelA16B16G16R16F( void * out, int x, int y, int z, int pitch, int slicepitch, float r, float g, float b, float a )
{
	//slow!
	// \todo PLATFORM SPECIFIC!!!!!
/*
	D3DXFLOAT16 r = D3DXFLOAT16(clr.r);
	D3DXFLOAT16 g = D3DXFLOAT16(clr.g);
	D3DXFLOAT16 b = D3DXFLOAT16(clr.b);
	D3DXFLOAT16 a = D3DXFLOAT16(clr.a);

	D3DXFLOAT16 *dst = (D3DXFLOAT16 *)((uchar*)out  + z*slicepitch+ y*pitch + x * (sizeof(D3DXFLOAT16)*4));
	*dst++ = r;
	*dst++ = g;
	*dst++ = b;
	*dst++ = a;*/
	
	/// \todo IMPLEMENT float16 !!!!
}


/// set float pixel, only r component
void	pixelR32F ( void * out, int x, int y, int z, int pitch, int slicepitch, float r, float g, float b, float a )
{
	float	* dst = (float *)((unsigned char*)out + z*slicepitch + y*pitch + x * sizeof(float));
	*dst = r;
}

/// set 24 bit pixel (RGB)
void	pixelR8G8B8 ( void * out, int x, int y,int z,  int pitch, int slicepitch, float r, float g, float b, float a )

{
	unsigned char *dst = (unsigned char *)out + z*slicepitch + y*pitch + x * 3;
	*dst++ = b*255;
	*dst++ = g*255;
	*dst++ = r*255;
}