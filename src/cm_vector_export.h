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

namespace svg
{
    class Document;
}

namespace cm
{
	class VectorExport
	{
	public:

		VectorExport();
		virtual ~VectorExport() {}
		
		virtual bool	begin( const std::string & fname, const Box& rect) {return false;}
		virtual void	end() {}
		
        //// New higher level funcs
        virtual void setLineWidth(float w) {this->lineWidth=w;}
        
        virtual void stroke( const Contour& s, const V4& clr ) {}
		virtual void fill( const Contour& s, const V4& clr ) {}
		virtual void stroke( const Shape& s, const V4& clr ) {}
		virtual void fill( const Shape& s, const V4& clr ) {}
		
		virtual void strokeCircle( const V2& center, float radius, const V4& clr ) {}
		virtual void fillCircle( const V2& center, float radius, const V4& clr ) {}

		virtual void strokeRect( float x, float y, float w, float h, const V4& clr ) {}
		virtual void fillRect( float x, float y, float w, float h, const V4& clr ) {}
		
		virtual void drawLine( const V2& a, const V2& b, const V4& clr ) {}
		virtual void drawArrow( const V2& a, const V2& b, float size, const V4& clr ) {}

        virtual void setStipple(float d0, float offset) {}
        
        virtual void pushMatrix();
        virtual void popMatrix();
        virtual void setMatrix( const M33& m );
        virtual void applyMatrix( const M33& m );
		virtual void identity();
        virtual void translate( float x, float y );
        virtual void rotate( float ang );
        virtual void scale( float x, float y );
        virtual void scale( float x );

        virtual void gsave(){}
        virtual void grestore(){}

        V2                  transform(const V2& p);
        Shape               transform(const Shape& S);
        Contour             transform(const Contour& P);

        bool exporting;
	protected:
		const M33& mat() const { return mats.back(); }
        M33& mat() { return mats.back(); }
        double scale() const 
		{ 
			arma::mat m(mat()); // armadillo does not allow submat on fixed...
			return sqrt(det(m.submat(arma::span(0,1),arma::span(0,1)))); 
		}
        std::vector<M33> mats;
        float lineWidth=0.5;
	};

    class SVGExport: public VectorExport
	{
	public:
        SVGExport() :VectorExport() {}

		bool	begin( const std::string & fname, const Box& rect);
		void	end();
		
        //// New higher level funcs
        void stroke( const Contour& s, const V4& clr );
		void fill( const Contour& s, const V4& clr );
		void stroke( const Shape& s, const V4& clr );
		void fill( const Shape& s, const V4& clr );
		void strokeCircle( const V2& center, float radius, const V4& clr );
		void fillCircle( const V2& center, float radius, const V4& clr );
		void strokeRect( float x, float y, float w, float h, const V4& clr );
		void fillRect( float x, float y, float w, float h, const V4& clr );
		void drawLine( const V2& a, const V2& b, const V4& clr );
		void drawArrow( const V2& a, const V2& b, float size, const V4& clr );
        void setStipple(float d0, float offset);
        
	protected:
	};


}	