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


#include "Image.h"
#include "cvGabor/cvGabor.h"

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

namespace cm
{

static CvPoint toCv( const Vec2 & v )
{
    return cvPoint(v.x,v.y);
}

static CvScalar toCv( const Color & c )
{
    return cvScalar(c.r*255,c.g*255,c.b*255,c.a*255);
}

static CvPoint tpoint_( const M33 & matrix, const Vec2 & v )
{
	Vec2 trans = matrix.trans();
	return toCv(mul(v,matrix)); //v+trans);//
}

static CvPoint tpoint_( const M33 & matrix, float x, float y )
{
	Vec2 trans = matrix.trans();
	return toCv(mul(Vec2(x,y),matrix)); //v+trans);//
}

#define tpoint(args...) tpoint_(matrix,args)

static Image::FORMAT getFormatFromBPP( int bpp )
{
	switch( bpp )
	{
		case 1:
			return Image::GRAYSCALE;
			break;
		case 3:
			return Image::RGB;
			break;
		case 4:
			return Image::BGRA;
			break;
			
	}

	assert(0);
	return Image::UNKNOWN;
}

Image::Image()
	:
	buf(0),
	fmt(Image::UNKNOWN),
	bytesPerPixel(0),
	width(0),
	height(0),
	stride(0),
	texture(0),
	retainBuffer(false),
	cvImg(NULL),
	cvImgTmp(NULL),
	cvMask(NULL),
	briConLutMatrix(NULL),
	dirty(false)
{
	matrix.identity();
}

Image::~Image()
{
	release();
}

Image::Image( int w, int h, int fmt_ )
	:
	buf(0),
	fmt(Image::UNKNOWN),
	bytesPerPixel(0),
	width(0),
	height(0),
	stride(0),
	texture(0),
	retainBuffer(false),
	cvImg(NULL),
	cvImgTmp(NULL),
	cvMask(NULL),
	briConLutMatrix(NULL),
	dirty(false)
{
	create(w,h,fmt_);
	matrix.identity();
}

Image::Image( const std::string & path, int fmt_ )
	:
	buf(0),
	fmt(Image::UNKNOWN),
	bytesPerPixel(0),
	width(0),
	height(0),
	stride(0),
	texture(0),
	retainBuffer(false),
	cvImg(NULL),
	cvImgTmp(NULL),
	cvMask(NULL),
	briConLutMatrix(NULL),
	dirty(false)
{
	load(path,fmt_);
	matrix.identity();
}

Image::Image( const Image & img )
	:
	buf(0),
	fmt(Image::UNKNOWN),
	bytesPerPixel(0),
	width(0),
	height(0),
	stride(0),
	texture(0),
	retainBuffer(false),
	cvImg(NULL),
	cvImgTmp(NULL),
	cvMask(NULL),
	briConLutMatrix(NULL)
{
	create(img.width,img.height,img.getFormat(),img.buf);
	matrix.identity();
}

void Image::release()
{
	SAFE_DELETE(texture);
	
	if( cvImg != NULL  )
		cvReleaseImage(&cvImg);
	cvImg = NULL;

	if( cvImgTmp != NULL  )
		cvReleaseImage(&cvImgTmp);
	cvImgTmp = NULL;
    
    
    if( cvMask != NULL  )
		cvReleaseImage(&cvMask);
	cvMask = NULL;


    if( briConLutMatrix != NULL )
        cvReleaseMat(&briConLutMatrix);
    briConLutMatrix = NULL;
    
	matrix.identity();
}


void Image::allocCv()
{
	//cvImg = cvCreateImageHeader(cvSize(width,height), IPL_DEPTH_8U,bytesPerPixel ); 
	//cvImg->imageData = (char*)buf; 
	//cvImg->imageDataOrigin = cvImg->imageData; 
	stride = cvImg->widthStep/bytesPerPixel;
	buf = (unsigned char*)cvImg->imageDataOrigin;

	cvMask=cvCreateImage(cvSize(width,height), IPL_DEPTH_8U,1 ); 
	
	cvImgTmp = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U,bytesPerPixel ); 

	//if( width != nd16(width) || height != nd16(height ) )
	//   debugPrint("Image is not properly aligned for using opencv operations!");
	   
	briConLutMatrix = cvCreateMat(1,256,CV_8UC1);

	curColor(1,1,1,1);
	linew = 1;
	matrix.identity();
}

static bool convert(IplImage * img, IplImage * to,int fmt,int toFmt)
{
	int copyType;

	if(fmt == toFmt)
	{
		cvCopy(img,to);
		return true;
	}

	if( fmt == Image::GRAYSCALE &&
		toFmt == Image::RGB )	
		copyType = CV_RGB2GRAY;

	else if( fmt == Image::RGB &&
		toFmt == Image::GRAYSCALE )
		copyType = CV_GRAY2RGB;

	else if( fmt == Image::BGRA &&
		toFmt == Image::GRAYSCALE )
		copyType = CV_GRAY2BGRA;

	else if( fmt == Image::GRAYSCALE &&
		toFmt == Image::BGRA )
		copyType = CV_BGRA2GRAY;

	else if( fmt == Image::BGRA &&
		toFmt == Image::RGB )
		copyType = CV_RGB2BGRA;

	else if( fmt == Image::RGB &&
		toFmt == Image::BGRA )
		copyType = CV_BGRA2RGB;
	else
		return false;

	cvCvtColor( img, to, copyType );
	return true;
}
void Image::copy( const Image & img )
{
	dirty = true;
	
	// Initialize 
	if( ! buf )
	{
		create(img.width,img.height,img.getFormat(),img.buf);
		return;
	}

	// resize if necessary
	if( ! buf ||
	    width != img.width ||
		height != img.height )
	{
		create(img.width,img.height,getFormat());
	}


	// if same format simply copy
	if( fmt == img.getFormat() )
	{
		copy(img.buf);
		return;	
	}
	
	int copyType;
	int otherFmt = img.getFormat();

	convert(img.cvImg,cvImg,fmt,otherFmt);	
}


void Image::create( int width_, int height_, int fmt_  )
{
	release();
	int reqn[] = { 3,4,1,0 };
	bytesPerPixel = reqn[fmt_];
	fmt = (Image::FORMAT)fmt_;
	width = width_;
	height = height_;
	
	cvImg = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U,bytesPerPixel);
	
	
	allocCv();
	
	clear();
}

void Image::create(  int width_, int height_, int fmt_, void * buf_ )
{
	release();
	int reqn[] = { 3,4,1,0 };
	bytesPerPixel = reqn[fmt_];
	fmt = (Image::FORMAT)fmt_;
	width = width_;
	height = height_;
	
	cvImg = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U,bytesPerPixel);
	
	allocCv();
	copy(buf_);
}
	

void Image::copy( void * buf_ )
{
	unsigned char * from = (unsigned char *)buf_;
	dirty = true;
	for( int i = 0; i < height; i++ )
		memcpy(&buf[i*stride*bytesPerPixel],&from[i*width*bytesPerPixel],width*bytesPerPixel);
}

void Image::clear()
{
	dirty = true;
	memset(buf,0,getSize());
}

void Image::invert()
{
	dirty = true;
	cvNot(cvImg, cvImg);
}

#define CVSWAP  cvCopy( cvImgTmp, cvImg );

void Image::mirror( bool x, bool y )
{
	dirty = true;
	int flipMode = 0;

	if( y && !x ) flipMode = 0;
	else if( !y && x ) flipMode = 1;
	else if( y && x ) flipMode = -1;
	else return;

	cvFlip( cvImg, cvImgTmp, flipMode );
	CVSWAP
}

void Image::thresh( int val, bool inv )
{
	dirty = true;
	int type = inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY;
	cvThreshold(cvImg, cvImg, val, 255, type);
}

void Image::adaptiveThresh( float blockSize_, float meanShift, bool inv , bool gauss  )
{
	dirty = true;
	int blockSize = blockSize_;

    if( blockSize < 2 ) {
        blockSize = 3;
    }

    if( blockSize % 2 == 0 ) {
        blockSize++;
    }

    int threshold_type = CV_THRESH_BINARY;
    if(inv) threshold_type = CV_THRESH_BINARY_INV;

    int adaptive_method = CV_ADAPTIVE_THRESH_MEAN_C;
    if(gauss) adaptive_method = CV_ADAPTIVE_THRESH_GAUSSIAN_C;

    cvAdaptiveThreshold( cvImg, cvImgTmp, 255, adaptive_method,
                         threshold_type, blockSize, meanShift );
   	CVSWAP

}


void Image::range( int minr, int maxr )
{
	dirty = true;
	cvInRangeS(cvImg, cvScalarAll(minr), cvScalarAll(maxr), cvImg);
}

void Image::erode()
{
	dirty = true;
	cvErode( cvImg, cvImgTmp, 0, 1 );
	CVSWAP
}

void Image::dilate( )
{
	dirty = true;
	cvDilate( cvImg, cvImgTmp, 0, 1 );
	CVSWAP
}

void Image::blur( int value )
{
	dirty = true;
	if( value % 2 == 0 ) 
        value++;
    
	cvSmooth( cvImg, cvImgTmp, CV_BLUR , value);
	CVSWAP

}

void Image::blurGaussian( int value )
{
	dirty = true;
	if( value % 2 == 0 ) 
        value++;

	cvSmooth( cvImg, cvImgTmp, CV_GAUSSIAN ,value );
	CVSWAP
}

void Image::brightnessContrast(float brightness, float contrast)
{
	dirty = true;
	int i;
	
	/*
	 * The algorithm is by Werner D. Streidt
	 * (http://visca.com/ffactory/archives/5-99/msg00021.html)
	 * (note: uses values between -1 and 1)
	 */

	double delta;
	double a;
	double b;
	if( contrast > 0 )
	{
		delta = 127.*contrast;
		a = 255./(255. - delta*2);
		b = a*(brightness*100 - delta);
	}else{
		delta = -128.*contrast;
		a = (256.-delta*2)/255.;
		b = a*brightness*100. + delta;
	}

	for( i = 0; i < 256; i++ )
	{
		int v = cvRound(a*i + b);
		if( v < 0 )
			v = 0;
		if( v > 255 )
			v = 255;
		briConLutMatrix->data.ptr[i] = (uchar)v;
		//briConLut[i] = (uchar)v;
	}

	//cvSetData( briConLutMatrix, briConLut, 0 );

	cvLUT( cvImg, cvImgTmp, briConLutMatrix); 
	CVSWAP
}

	
void Image::gabor( float phi,float nu,float sigma, int type )
{
	if( fmt != Image::GRAYSCALE )
	{
		printf("Image::gabor Error, image has to be grayscale\n");
		return;
	}
	
	CvGabor gabor(phi,nu,sigma);
	gabor.conv_img(cvImg,cvImgTmp,type);
	CVSWAP
	dirty = true;
}

void Image::gabor4( float nu, float sigma, int type)
{
	if( fmt != Image::GRAYSCALE )
	{
		printf("Image::gabor Error, image has to be grayscale\n");
		return;
	}
	
	IplImage * tmp =cvCreateImage(cvSize(width,height), IPL_DEPTH_8U,1 ); 
	
	float inc = TWOPI/4;
	for( int i = 0; i < 4; i++ )
	{
		CvGabor gabor(inc*i,nu,sigma);
		gabor.conv_img(cvImg,tmp,type);
		if(!i)
			cvCopy(tmp,cvImgTmp);
		else
			cvAdd(cvImgTmp,tmp,cvImgTmp);
	}
	CVSWAP
	dirty = true;
	
	cvReleaseImage(&tmp);
}


	
int Image::pixelsSum( int step ) const
{
	int n = getSize();
	int sum = 0;
	for( int i = 0; i < n; i+=step )
		sum += buf[i];
	return sum;
}


void Image::warpPerspective( const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D ) 
{
	dirty = true;
    // compute matrix for perspectival warping (homography)
    CvPoint2D32f cvsrc[4];
    CvPoint2D32f cvdst[4];
    CvMat* translate = cvCreateMat( 3,3, CV_32FC1 );
    cvSetZero( translate );

    cvdst[0].x = 0;
    cvdst[0].y = 0;
    cvdst[1].x = width;
    cvdst[1].y = 0;
    cvdst[2].x = width;
    cvdst[2].y = height;
    cvdst[3].x = 0;
    cvdst[3].y = height;

    cvsrc[0].x = A.x;
    cvsrc[0].y = A.y;
    cvsrc[1].x = B.x;
    cvsrc[1].y = B.y;
    cvsrc[2].x = C.x;
    cvsrc[2].y = C.y;
    cvsrc[3].x = D.x;
    cvsrc[3].y = D.y;

    cvWarpPerspectiveQMatrix( cvsrc, cvdst, translate );  // calculate homography
    cvWarpPerspective( cvImg, cvImgTmp, translate );
    CVSWAP
    cvReleaseMat( &translate );
}



//--------------------------------------------------------------------------------
void Image::warpIntoMe( const Image& mom, const Vec2 *src, const Vec2 *dst )
{
	dirty = true;
    if( getFormat() != mom.getFormat() ) 
    {

    	// compute matrix for perspectival warping (homography)
    	CvPoint2D32f cvsrc[4];
    	CvPoint2D32f cvdst[4];
    	CvMat* translate = cvCreateMat( 3, 3, CV_32FC1 );
    	cvSetZero( translate );
    	for (int i = 0; i < 4; i++ ) {
    		cvsrc[i].x = src[i].x;
    		cvsrc[i].y = src[i].y;
    		cvdst[i].x = dst[i].x;
    		cvdst[i].y = dst[i].y;
    	}
    	cvWarpPerspectiveQMatrix( cvsrc, cvdst, translate );  // calculate homography
    	cvWarpPerspective( mom.cvImg, cvImg, translate);
    	cvReleaseMat( &translate );

    } else {
      assert(0);
    }
}


void Image::absDifference( const cm::Image & img )
{
	dirty = true;
	 cvAbsDiff( cvImg, img.cvImg, cvImgTmp );
     CVSWAP
}

	//--------------------------------------------------------------------------------
	static bool sort_carea_compare( const CvSeq* a, const CvSeq* b) {
		// use opencv to calc size, then sort based on size
		float areaa = fabs(cvContourArea(a, CV_WHOLE_SEQ));
		float areab = fabs(cvContourArea(b, CV_WHOLE_SEQ));
		
		//return 0;
		return (areaa > areab);
	}
	
	
	
Shape Image::findContours( float w , float h, float minArea, float maxArea,  float simplifyTol, bool approx , bool holes , int maxContours )
{
	// get width/height disregarding ROI
	cvCopy( cvImg, cvImgTmp );
	
	IplImage* cvimg = cvImgTmp;
	float rw = w/cvimg->width;
	float rh = h/cvimg->height;
	
	CvMemStorage*           contour_storage;
	CvMemStorage*           storage;
	
	CvSeq* contour_list = NULL;
	contour_storage = cvCreateMemStorage( 1000 );
	storage	= cvCreateMemStorage( 1000 );
	
	CvContourRetrievalMode  retrieve_mode
	= (holes) ? CV_RETR_LIST : CV_RETR_EXTERNAL;
	cvFindContours( cvimg, contour_storage, &contour_list,
				   sizeof(CvContour), retrieve_mode, approx ? CV_CHAIN_APPROX_SIMPLE : CV_CHAIN_APPROX_NONE );
	CvSeq* contour_ptr = contour_list;
	
	std::vector<CvSeq*> seqs;
	Shape res;
	// put the contours from the linked list, into an array for sorting
	while( (contour_ptr != NULL) ) 
	{
		float area = fabs( cvContourArea(contour_ptr, CV_WHOLE_SEQ) );
		if( (area > minArea) && (area < maxArea) )
			seqs.push_back(contour_ptr);
		
		contour_ptr = contour_ptr->h_next;
	}
	
	
	// sort the pointers based on size
	if( seqs.size() > 1 ) {
		sort( seqs.begin(), seqs.end(), sort_carea_compare );
	}
	
	
	// now, we have seqs.size() contours, sorted by size in the array
	// seqs let's get the data out and into our structures that we like
	for( int i = 0; i < std::min(maxContours, (int)seqs.size()); i++ ) 
	{
		Contour p = Contour();// res.appendContour();
		
		// get the points for the blob:
		CvPoint           pt;
		CvSeqReader       reader;
		cvStartReadSeq( seqs[i], &reader, 0 );
		
		for( int j=0; j < seqs[i]->total; j++ ) 
		{
			CV_READ_SEQ_ELEM( pt, reader );
			p.addPoint(pt.x*rw,pt.y*rh);
		}

		p.close();

		res.addContour(p);
	}
	
	
	if( simplifyTol > 0.01 )
	{
		res.simplify(simplifyTol);
	}
	// Free the storage memory.
	// Warning: do this inside this function otherwise a strange memory leak
	if( contour_storage != NULL ) { cvReleaseMemStorage(&contour_storage); }
	if( storage != NULL ) { cvReleaseMemStorage(&storage); }
	
	return res;
}

	std::vector<Shape> Image::segmentation(  int n,  float w , float h, float minArea, float maxArea, float simplifyTol,  bool approx , bool holes , int maxContours )
{
	Image tmp(*this);
	
	std::vector<Shape> res;
	float inc = 256.0/(n);
	
	float minr = 0;
	float maxr = inc;
	
	bool bRange = true;
	
	if( n == 1 )
		bRange = false;
	
	for( int i = 0; i < n; i++ )
	{
		tmp.copy(*this);
		maxr = minr+inc;
		
		if(bRange)
		{
			tmp.range(minr,maxr);
			tmp.blur(5);
		}
		else 
		{
			tmp.thresh(128);
		}
		
		minr = maxr;
		res.push_back(tmp.findContours(w,h,minArea,maxArea,simplifyTol,approx,holes));
	}
	
	return res;
}



bool Image::load( const std::string & path, int forceFmt )
{
	release();
	dirty = true;
	int reqn[] = { 3,4,1,0 };

	int cvflag = CV_LOAD_IMAGE_UNCHANGED;
	IplImage * img = cvLoadImage(path.c_str(),CV_LOAD_IMAGE_UNCHANGED);

	if(!img)
	{
		assert(0);
		return false;
	}

	int nChannels = img->nChannels;

	if(forceFmt==Image::UNKNOWN)
		forceFmt = getFormatFromBPP(nChannels);

	create(img->width,img->height,forceFmt);

	if( nChannels == 1 && forceFmt == Image::GRAYSCALE )
		cvCopy(img,cvImg);
	else if ( nChannels == 1 && forceFmt == Image::RGB )
		cvCvtColor(img,cvImg,CV_GRAY2RGB);
	else if ( nChannels == 1 && forceFmt == Image::BGRA )
		cvCvtColor(img,cvImg,CV_GRAY2BGRA);
	else if ( nChannels == 3 && forceFmt == Image::GRAYSCALE )
		cvCvtColor(img,cvImg,CV_RGB2GRAY);
	else if ( nChannels == 3 && forceFmt == Image::RGB )
		cvCopy(img,cvImg);
	else if ( nChannels == 3 && forceFmt == Image::BGRA )
		cvCvtColor(img,cvImg,CV_RGB2BGRA);
	else if ( nChannels == 4 && forceFmt == Image::GRAYSCALE )
		cvCvtColor(img,cvImg,CV_BGRA2GRAY);
	else if ( nChannels == 4 && forceFmt == Image::RGB )
		cvCvtColor(img,cvImg,CV_BGRA2RGB);
	else if ( nChannels == 4 && forceFmt == Image::BGRA )
		cvCopy(img,cvImg);
	else
	{
		assert(false);
		return false;
	}

	cvReleaseImage(&img);

	bytesPerPixel = cvImg->nChannels;
	width = cvImg->width;
	height = cvImg->height;
	
	fmt = getFormatFromBPP(bytesPerPixel);
	
	allocCv();
	return true;
}
	
bool Image::save( const std::string & path )
{
	cvSaveImage(path.c_str(),cvImg);
	//return writeToPng( path.c_str(), width,height,bytesPerPixel,buf );
}
	

void Image::setPixel( int x, int y, const Color & clr )
{
	dirty = true;
	unsigned char * p = &buf[y*stride*bytesPerPixel+x*bytesPerPixel];
	switch(fmt)
	{
		case RGB:
			p[0] =clr.r*255;
			p[1] =clr.g*255;
			p[2] =clr.b*255;
			return;
			
		case BGRA:
			p[0] =clr.b*255;
			p[1] =clr.g*255;
			p[2] =clr.r*255;
			p[3] =clr.a*255;
			return;
			
		case GRAYSCALE:
		{
			p[0] = clr.r;
			return;
		}	
		default:
			assert(0);
	}
	
}

Color Image::getPixel( int x, int y ) const
{
	x = clamp(x,0,width-1);
	y = clamp(y,0,height-1);
	
	unsigned char * p = &buf[y*stride*bytesPerPixel+x*bytesPerPixel];
	switch(fmt)
	{
		case RGB:
			return Color(DIV255*p[0],
						 DIV255*p[1],
						 DIV255*p[2],
						 1.0);
			
		case BGRA:
			return Color(DIV255*p[2],
						 DIV255*p[1],
						 DIV255*p[0],
						 DIV255*p[3]);
			
		case GRAYSCALE:
		{
			float v = DIV255*p[0];
			return Color(v,v,v,v);
		}	
		default:
			assert(0);
	}
	
	return Color(0,0,0,0);
}
	
float Image::getIntensity( int x, int y ) const
{
	if( x >= width )
		x = width-1;
	if( y >= height )
		y = height-1;
	if( x < 0 )
		x = 0;
	if( y < 0 )
		y = 0;
	
	unsigned char * p = &buf[y*stride*bytesPerPixel+x*bytesPerPixel];
	switch(fmt)
	{
		case RGB:
			return Color(DIV255*p[0],
						 DIV255*p[1],
						 DIV255*p[2],
						 1.0).luminosity();
			
		case BGRA:
			return Color(DIV255*p[2],
						 DIV255*p[1],
						 DIV255*p[0],
						 DIV255*p[3]).luminosity();
			
		case GRAYSCALE:
		{
			float v = DIV255*p[0];
			return v;
		}	
		default:
			assert(0);
	}
	
	return 0;
}

Texture * Image::getTexture()
{
	if(!buf)
		return 0;
	
	if(!texture)
	{
		texture = new Texture();
		dirty = false;
		Texture::FORMAT tfmt;
		switch (fmt) {
				
			case RGB:
				if( !texture->create(buf,stride,height,Texture::R8G8B8) )
				{
					debugPrint("Failed to create texture!");
					delete texture;
					return 0;
				}
				break;
				
			case BGRA:
				if( !texture->create(buf,stride,height,Texture::A8R8G8B8) )
				{
					debugPrint("Failed to create texture!");
					delete texture;
					return 0;
				}
				break;
				
			case GRAYSCALE:
				if( !texture->create(buf,stride,height,Texture::L8) )
				{
					debugPrint("Failed to create texture!");
					delete texture;
					return 0;
				}
				break;
				
			
			default:
				break;
		}

		texture->setSize(width,height);
	}
	
	
	return texture;
}

void Image::updateTexture()
{
	if(!getTexture())
	{
		getTexture();
	}
	
	dirty = false;
	texture->update(buf,stride,height);
}

	
void Image::draw( float x, float y, float w, float h, bool yUp )
{
	if(!buf)
		return;
	
	if(dirty)
		updateTexture();
	dirty = false;
	
	Texture * tex = getTexture();
	if(w==0)
		w = width;
	if(h==0)
		h = height;
	tex->draw(x,y,w,h,yUp);
}

void Image::grabFrameBuffer( int w, int h, int fmt_  )
{
	if( !buf ||
		width != w ||
		height != h ||
		fmt != fmt_ )
		create(w,h,fmt_);

	unsigned char * fb = new unsigned char [width*height*bytesPerPixel];
	switch(fmt)
	{
		case RGB:
			glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,fb); 
			break;
		case BGRA:
			glReadPixels(0,0,w,h,GL_BGRA,GL_UNSIGNED_BYTE,fb); 
			break;
		case GRAYSCALE:
			glReadPixels(0,0,w,h,GL_LUMINANCE,GL_UNSIGNED_BYTE,fb); 
			break;
	}

	copy(fb);
	delete [] fb;
	
	dirty = true;
}



void Image::fillShape( IplImage * img, const Shape2d & shape, const Color & clr,float srcW, float srcH  )
{
	int n = shape.size();
	
	CvPoint ** pts = new CvPoint*[n];
	int *npts = new int[n];
	
	float sx = (float)width/srcW;
	float sy = (float)height/srcH;
	
	for( int i = 0; i < n; i++ )
	{
		const Path2d & p = shape.getContour(i);
		pts[i] = new CvPoint[p.size()];
		for( int j = 0; j < p.size(); j++ )
		{
			const Vec2 & pt = p.getPoint(j);
			CvPoint cvp;
			cvp.x = pt.x*sx;
			cvp.y = pt.y*sy;
			
			pts[i][j] = cvp;
			pts[i][j].x = std::min(std::max(pts[i][j].x,0),width-1);
			pts[i][j].y = std::min(std::max(pts[i][j].y,0),height-1);
			
			//			assert( pts[i][j].x >= 0 && pts[i][j].x < getWidth() );
			//			assert( pts[i][j].y >= 0 && pts[i][j].y < getHeight() );
		}
		npts[i] = p.size();
	}
	
	cvFillPoly(img, pts, npts, n, cvScalar(clr.r*255) );
	
	delete [] npts;
	for( int i = 0; i < n; i++ )
		delete pts[i];
	delete [] pts;
}


void Image::fillShape( IplImage * img, const Shape & shape, const Color & clr,float srcW, float srcH  )
{
	int n = shape.size();
	
	CvPoint ** pts = new CvPoint*[n];
	int *npts = new int[n];
	
	float sx = (float)width/srcW;
	float sy = (float)height/srcH;
	
	for( int i = 0; i < n; i++ )
	{
		const Contour & p = shape.getContour(i);
		pts[i] = new CvPoint[p.size()];
		for( int j = 0; j < p.size(); j++ )
		{
			const Vec2 & pt = p.getPoint(j);
			CvPoint cvp;
			cvp.x = pt.x*sx;
			cvp.y = pt.y*sy;
			
			pts[i][j] = cvp;
			pts[i][j].x = std::min(std::max(pts[i][j].x,0),width-1);
			pts[i][j].y = std::min(std::max(pts[i][j].y,0),height-1);
			
			//			assert( pts[i][j].x >= 0 && pts[i][j].x < getWidth() );
			//			assert( pts[i][j].y >= 0 && pts[i][j].y < getHeight() );
		}
		npts[i] = p.size();
	}
	
	cvFillPoly(img, pts, npts, n, cvScalar(clr.r*255) );
	
	delete [] npts;
	for( int i = 0; i < n; i++ )
		delete pts[i];
	delete [] pts;
}
	

float Image::getIntensityConvex( const cm::Path2d & path, float srcW, float srcH )
{
	if( fmt != GRAYSCALE )
		return 0;

	if(srcW == 0.0)
		srcW = width;
	if(srcH == 0.0)
		srcH = height;
	
	float sx = (float)width/srcW;
	float sy = (float)height/srcH;
	
	std::vector<CvPoint> pts;
	for( int i = 0; i < path.size(); i++ )
	{
		const Vec2 & p = path[i];
		CvPoint cvp;
		cvp.x = p.x*sx;
		cvp.y = p.y*sy;
		pts.push_back(cvp);
	}

	cvSet(cvMask,cvScalar(0));
	cvFillConvexPoly (cvMask, &pts[0], pts.size() ,cvScalar(255));
	float v = cvMean(cvImg, cvMask);
	return v/255.0;
}


float Image::getIntensity( const cm::Shape2d & shape, float srcW, float srcH )
{
	if( fmt != GRAYSCALE )
		return 0;

	if(srcW == 0.0)
		srcW = width;
	if(srcH == 0.0)
		srcH = height;

	cvSet(cvMask,cvScalar(0));
	fillShape(cvMask,shape,Color(1,1,1,1),srcW,srcH);
	//cvFillConvexPoly (cvMask, &pts[0], pts.size() ,cvScalar(255));
	float v = cvMean(cvImg, cvMask);
	return v/255.0;

}


void  Image::setView( float x, float y, float w, float h )
{
	identity();
	translate(x,y);
	scale((float)width/w,(float)height/h);
	//matrix.transpose();
}

void  Image::setView( float w, float h )
{
	setView(0,0,w,h);
}


void Image::color( const Color & clr )
{
	curColor = clr;
}

void Image::color( float r, float g, float b, float a )
{
	curColor(r,g,b,a);
}

void Image::color( float l, float a )
{
	curColor(l,l,l,a);
}

void Image::lineWidth( int w )
{
	this->linew = w;
}
void Image::identity()
{
	matrix.identity();
}

void Image::scale( float x, float y )
{
	matrix.scale(x,y);
}
 
void Image::rotate( float ang )
{
	matrix.rotate( radians(ang) );
}

void Image::translate( float x, float y )
{
	matrix.translate(x,y);
}

void Image::drawLine( const Vec2 & a, const Vec2 & b  )
{
	dirty = true;
	cvLine(cvImg, tpoint(a), tpoint(b), toCv(curColor), linew );//int thickness=1, int lineType=8, int shift=0)
}

void Image::drawRect( float x, float y, float w, float h )
{
	dirty = true;
	cvRectangle(cvImg, tpoint(x,y), tpoint(x+w,y+h), toCv(curColor));
}

void Image::fillRect( float x, float y, float w, float h )
{
	dirty = true;
	cvRectangle(cvImg, tpoint(x,y), tpoint(x+w,y+h), toCv(curColor), -1);
}


static void copyImage(IplImage* target, IplImage* source, int x, int y) 
{

    for (int ix=0; ix<source->width; ix++) {
        for (int iy=0; iy<source->height; iy++) {
            CvScalar clr = cvGet2D(source, iy, ix);
			int destx = ix+x;
			int desty = iy+y;
			if( destx > 0 && destx < target->width &&
				desty > 0 && desty < target->height )
				cvSet2D(target, iy+y, ix+x, clr);
        }
    }
}

void Image::drawImage( const cm::Image & img, float x, float y )
{
	dirty = true;
	copyImage(cvImg,img.cvImg,roundf(x),roundf(y));
}

void Image::fillShape( const Shape2d & shape )
{
	dirty = true;
	int n = shape.size();
	
	CvPoint ** pts = new CvPoint*[n];
	int *npts = new int[n];

	for( int i = 0; i < n; i++ )
	{
		const Path2d & p = shape.getContour(i);
		pts[i] = new CvPoint[p.size()];
		for( int j = 0; j < p.size(); j++ )
		{
			pts[i][j] = tpoint(p.getPoint(j));
			pts[i][j].x = std::min(std::max(pts[i][j].x,0),getWidth()-1);
			pts[i][j].y = std::min(std::max(pts[i][j].y,0),getHeight()-1);
			
//			assert( pts[i][j].x >= 0 && pts[i][j].x < getWidth() );
//			assert( pts[i][j].y >= 0 && pts[i][j].y < getHeight() );
		}
		npts[i] = p.size();
	}
	CvScalar ss = toCv(curColor);
	cvFillPoly(cvImg, pts, npts, n, toCv(curColor) );

	delete [] npts;
	for( int i = 0; i < n; i++ )
		delete pts[i];
	delete [] pts;
}

void Image::fillPath( const Path2d & path )
{
	dirty = true;
	fillShape(Shape2d(path));
}

void Image::drawShape( const Shape2d & shape )
{
	dirty = true;
	int n = shape.size();
	
	CvPoint ** pts = new CvPoint*[n];
	int *npts = new int[n];

	for( int i = 0; i < n; i++ )
	{
		const Path2d & p = shape.getContour(i);
		pts[i] = new CvPoint[p.size()];
		for( int j = 0; j < p.size(); j++ )
		{
			pts[i][j] = tpoint(p.getPoint(j));
			pts[i][j].x = std::min(std::max(pts[i][j].x,0),getWidth()-1);
			pts[i][j].y = std::min(std::max(pts[i][j].y,0),getHeight()-1);
		}
		npts[i] = p.size();
		cvPolyLine(cvImg, &pts[i], &npts[i], 1, p.isClosed(), toCv(curColor), linew);
	}

	delete [] npts;
	for( int i = 0; i < n; i++ )
		delete pts[i];
	delete [] pts;

	
}

void Image::drawPath( const Path2d & path )
{
	dirty = true;
	drawShape(Shape2d(path));
}



void Image::fillShape( const Shape & shape )
{
	dirty = true;
	int n = shape.size();
	
	CvPoint ** pts = new CvPoint*[n];
	int *npts = new int[n];

	for( int i = 0; i < n; i++ )
	{
		const Contour & p = shape.getContour(i);
		pts[i] = new CvPoint[p.size()];
		for( int j = 0; j < p.size(); j++ )
		{
			pts[i][j] = tpoint(p.getPoint(j));
			pts[i][j].x = std::min(std::max(pts[i][j].x,0),getWidth()-1);
			pts[i][j].y = std::min(std::max(pts[i][j].y,0),getHeight()-1);
			
//			assert( pts[i][j].x >= 0 && pts[i][j].x < getWidth() );
//			assert( pts[i][j].y >= 0 && pts[i][j].y < getHeight() );
		}
		npts[i] = p.size();
	}
	CvScalar ss = toCv(curColor);
	cvFillPoly(cvImg, pts, npts, n, toCv(curColor) );

	delete [] npts;
	for( int i = 0; i < n; i++ )
		delete pts[i];
	delete [] pts;
}

void Image::fillContour( const Contour & path )
{
	dirty = true;
	fillShape(Shape(path));
}

void Image::drawShape( const Shape & shape )
{
	dirty = true;
	int n = shape.size();
	
	CvPoint ** pts = new CvPoint*[n];
	int *npts = new int[n];

	for( int i = 0; i < n; i++ )
	{
		const Contour & p = shape.getContour(i);
		pts[i] = new CvPoint[p.size()];
		for( int j = 0; j < p.size(); j++ )
		{
			pts[i][j] = tpoint(p.getPoint(j));
			pts[i][j].x = std::min(std::max(pts[i][j].x,0),getWidth()-1);
			pts[i][j].y = std::min(std::max(pts[i][j].y,0),getHeight()-1);
		}
		npts[i] = p.size();
		cvPolyLine(cvImg, &pts[i], &npts[i], 1, p.isClosed(), toCv(curColor), linew);
	}

	delete [] npts;
	for( int i = 0; i < n; i++ )
		delete pts[i];
	delete [] pts;

	
}

void Image::drawContour( const Contour & path )
{
	dirty = true;
	drawShape(Shape(path));
}


void Image::drawContour( const std::vector<Vec2> P, bool closed )
{
	dirty = true;
	
	CvPoint ** pts = new CvPoint*[1];
	int *npts = new int[1];

	for( int i = 0; i < 1; i++ )
	{
		pts[i] = new CvPoint[P.size()];
		for( int j = 0; j < P.size(); j++ )
		{
			pts[i][j] = tpoint(P[j]);
			pts[i][j].x = std::min(std::max(pts[i][j].x,0),getWidth()-1);
			pts[i][j].y = std::min(std::max(pts[i][j].y,0),getHeight()-1);
		}
		npts[i] = P.size();
		cvPolyLine(cvImg, &pts[i], &npts[i], 1, closed, toCv(curColor), linew);
	}

	delete [] npts;
	for( int i = 0; i < 1; i++ )
		delete pts[i];
	delete [] pts;

}

void Image::fillRect( const cm::Rectf & r )
{
	dirty = true;
	this->fillRect(r.l,r.t,r.getWidth(),r.getHeight());
}

void Image::drawRect( const cm::Rectf & r )
{
	dirty = true;
	this->drawRect(r.l,r.t,r.getWidth(),r.getHeight());
}
	
void Image::add( const Image & img )
{
	int n = std::min(img.stride*img.height,stride*height);
	n*=bytesPerPixel;

    for( int i = 0; i < n; i++ )
    {
        int v = buf[i]+img.buf[i];
        buf[i] = (v>255)?255:v;
    }

    dirty = true;
}

void Image::subtract( const Image & img )
{
	int n = std::min(img.stride*img.height,stride*height);
	n*=bytesPerPixel;

    for( int i = 0; i < n; i++ )
    {
        int v = buf[i]-img.buf[i];
        buf[i] = (v<0)?0:v;
    }

    dirty = true;
}

void Image::floodFill( int x, int y, const Color & clr )
{

#ifndef CM_OFX
	cv::floodFill(cv::Mat(cvImg,false), cv::Point(x,y), cv::Scalar(clr.r*255,clr.g*255,clr.b*255,clr.a*255));
	dirty = true;
#else
    // HMMM NEED TO CHECK OFX CV VERSION, DOES NOT ACCEPT MAT AS FIRST ARG
    assert(false);
#endif
}

void Image::floodFill( int x, int y, Image & mask, const Color & clr )
{
	// Something must be wrong with cvImg, since there is a memory corruption happening somewere with flood fill
	// program would crash when mask is released
    #ifndef CM_OFX
	cv::floodFill(cv::Mat(cvImg,false), 
				  cv::Mat(mask.cvImg,false), 
				  cv::Point(x,y), 
				  cv::Scalar(clr.r*255),0, //,clr.g*255,clr.b*255,clr.a*255)
				  cv::Scalar(),
				  cv::Scalar(),
				  4 | (255 << 8) | cv::FLOODFILL_MASK_ONLY);
	//memcpy(mask.cvImg,mask.cvImgTmp,mask.width*mask.height);
	mask.dirty=true;
	dirty = true;
#else
    // HMMM NEED TO CHECK OFX CV VERSION, DOES NOT ACCEPT MAT AS FIRST ARG
    assert(false);
#endif
    
}



}