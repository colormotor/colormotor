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

#include "cm_gfx.h"
	
namespace cm
{
	class EpsFile
	{
	public:

		EpsFile();
		~EpsFile();
		
		bool	open( const std::string & fname);
		void	close();
		
		// usually:
		// header();
		// newpath();
		// drawstuff...
		// stroke(); or fill();
		// showpage();
        void        pushMatrix();
        void        popMatrix();
        void        setMatrix( const M33& m );
        void 		applyMatrix( const M33& m );
		void 		identity();

		void		shape( const Shape& s );
		void		shape( const Contour & c );		
		void 		fillStroke();
		
		/////////////////////////////////////////////////////
		void		header(const Box & box );
		
		/////////////////////////////////////////////////////
		void		header(const char *autor = "ENIST", int x0 = 0, int y0 = 0, int x1 = 4000, int y1 = 3000 );
		
		/////////////////////////////////////////////////////
		void		newpath();

		/////////////////////////////////////////////////////
		void		closepath();

		/////////////////////////////////////////////////////
		void		setlinewidth( float w );

		/////////////////////////////////////////////////////
		void		setrgb( float r, float g, float b );

		/////////////////////////////////////////////////////
		void		setrgb( const V3 & clr );

		/////////////////////////////////////////////////////
		void		setcmyk(float c, float m, float y, float k );
		/////////////////////////////////////////////////////
		void		circle( const V2& pos, float radius );
		/////////////////////////////////////////////////////
		void		rect( float x, float y, float w, float h );
		/////////////////////////////////////////////////////
		void 		text( const V2& pos, float size, const std::string& str );

		/////////////////////////////////////////////////////
		void		setrgb(unsigned int clr);

		/////////////////////////////////////////////////////
		void		gray( float g );

		/////////////////////////////////////////////////////
		void		moveto( const V2& v );
		void		moveto( float x, float y );

		/////////////////////////////////////////////////////
		void		lineto( float x, float y );
		void		lineto( const V2& v );
		/////////////////////////////////////////////////////
		void		arrowto( float x, float y );
		void		arrowto( const V2& v );
		/////////////////////////////////////////////////////
		void		rlineto( float x, float y );
		void		rlineto( const V2& v );

		/////////////////////////////////////////////////////
		void		stroke();
		/////////////////////////////////////////////////////
		void		fill();
		/////////////////////////////////////////////////////
		void		showpage();
		/////////////////////////////////////////////////////
		void		gsave();
		/////////////////////////////////////////////////////
		void		grestore();
		/////////////////////////////////////////////////////
		void		setdash(float d0, float d1, float offset);
		/////////////////////////////////////////////////////
		void		setdash(float d0, float d1, float d2, float d3, float offset);

		/////////////////////////////////////////////////////
		void		translate( float x, float y );
		/////////////////////////////////////////////////////
		void		scale( float x, float y );
		/////////////////////////////////////////////////////
		void 		rotate( float ang );

		void 		setStrokeColor();
		void 		setFillColor();

		void 		strokeGray( float v );
		void 		strokeRgb( float r, float g, float b );
		void 		strokeRgb( const V4 & clr );
		void 		strokeCmyk( float c, float m, float y, float k );
		void 		fillGray( float v );
		void 		fillRgb( float r, float g, float b );
		void 		fillRgb( const V4& clr );
		void 		fillCmyk( float c, float m, float y, float k );
		void 		fillNone();
		void 		strokeNone();

		//// New higher level funcs
		void 		strokeShape( const Shape& s, const V4& clr );
		void 		fillShape( const Shape& s, const V4& clr );
		
		void		strokeCircle( const V2& center, float radius, const V4& clr );
		void		fillCircle( const V2& center, float radius, const V4& clr );

		void		strokeRect( float x, float y, float w, float h, const V4& clr );
		void		fillRect( float x, float y, float w, float h, const V4& clr );
		
		void		drawLine( const V2& a, const V2& b, const V4& clr );
		void		drawArrow( const V2& a, const V2& b, float size, const V4& clr );
		
		/////////////////////////////////////////////////////
		// TODO: translate rotate reset etc. 
		//       function and var definition.

		/////////////////////////////////////////////////////
		// print to file.
		void  print(const char *pFormat,... );

		bool isOpen() const { return _file != 0; }
		FILE * getFile() { return _file; }
	protected:
		enum
		{
			NONE = -1,
			RGB=0,
			CMYK=1,
			GRAY
		};

		int fillType;
		float fillColor[4];
		int strokeType;
		float strokeColor[4];

		FILE * _file;

		Box fileRect;
		float makey( float y) const;
		float makex( float x) const;
        
        const M33& mat() const { return mats.back(); }
        M33& mat() { return mats.back(); }
        
        std::vector<M33> mats;
        float lineWidth=0.5;
	};

}	