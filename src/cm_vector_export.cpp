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


#include "cm_vector_export.h"
#include "deps/simple_svg.hpp"

namespace cm
{
	VectorExport::VectorExport()
	{
		mats.push_back(arma::eye(3,3));
	}
	
    void VectorExport::pushMatrix()
    {
    	gsave();
    	mats.push_back(mats.back());
    }
    
    void VectorExport::popMatrix()
    {
    	grestore();
        mats.pop_back();
    }
	
	void VectorExport::applyMatrix( const M33& m )
	{
		mats.back() = mats.back() * m;
	}   

	void VectorExport::identity()
	{
		mats.back() = arma::eye(3,3);
	}

    void VectorExport::setMatrix( const M33& m )
    {
    	identity();
    	applyMatrix(m);
    }

	void		VectorExport::translate( float x, float y )
	{
		M33 m = trans2d(x, y);
		applyMatrix(m);
		//P(_file,"%g %g translate\n", makex(x*SCALE), makey(y*SCALE) );
	}
	
	void		VectorExport::rotate( float ang )
	{
		M33 m = rot2d(radians(ang));
		applyMatrix(m);
	}

    void		VectorExport::scale( float x, float y )
	{
		M33 m = scaling2d(x, y);
		applyMatrix(m);
		//P(_file,"%g %g scale\n",x,y);
	}
	
    void		VectorExport::scale( float x )
	{
		M33 m = scaling2d(x, x);
		applyMatrix(m);
		//P(_file,"%g %g scale\n",x,y);
	}

    V2           VectorExport::transform(const V2& p) { return affineMul(mat(), p); }
    Shape        VectorExport::transform(const Shape& S) { return S.transformed(mat()); };
    Contour      VectorExport::transform(const Contour& P) { return P.transformed(mat()); };

    #define CLR(clr) svg::Color((int)(clr.x*255), (int)(clr.y*255), (int)(clr.z*255))
    #define PT(v) svg::Point(v[0], v[1])
    #define invalid(p) ((p).has_nan() || (p).has_inf())

    static svg::Document doc;

    bool SVGExport::begin( const std::string & fname, const Box& rect)
    {
        svg::Dimensions dimensions(rect.width(), rect.height());
        doc = svg::Document(fname, svg::Layout(dimensions, svg::Layout::TopLeft));
        return true;
    }

    void SVGExport::end()
    {
        doc.save();
    }

    void SVGExport::stroke( const Contour& P_, const V4& clr )
    {
        Contour P = transform(P_);
        svg::Stroke s(lineWidth, CLR(clr), false, clr.w);
        if (P.closed)
        {
            svg::Polygon p(svg::Fill(), s);
            for (int i = 0; i < P.size(); i++)
            {
                if (invalid(P[i]))
                    continue;
                p << PT(P[i]);
            }
            doc << p;
        }
        else
        {
            svg::Polyline p(svg::Fill(), s);
            for (int i = 0; i < P.size(); i++)
            {
                if (invalid(P[i]))
                    continue;
                p << PT(P[i]);
            }
            doc << p;
        }
    }

    void SVGExport::fill( const Contour& P_, const V4& clr )
    {
        Contour P = transform(P_);
        svg::Fill f(CLR(clr), clr.w);
        svg::Polygon p(f, svg::Stroke());
        for (int i = 0; i < P.size(); i++)
        {
            if (invalid(P[i]))
                    continue;
            p << PT(P[i]);
        }
        doc << p;
    }
    
    void SVGExport::stroke( const Shape& s, const V4& clr )
    {
        for (int i = 0; i < s.size(); i++)
            stroke(s[i], clr);
    }

    void SVGExport::fill( const Shape& S_, const V4& clr )
    {
        Shape S = transform(S_);
        svg::Fill f(CLR(clr), clr.w);
        svg::Path p(f, svg::Stroke());
        for (int i = 0; i < S.size(); i++)
        {
            p.startNewSubPath();
            for (int j = 0; j < S[i].size(); j++)
            {
                if (invalid(S[i][j]))
                    continue;
                p << PT(S[i][j]);
            }
        }
        doc << p;
    }
    
    void SVGExport::strokeCircle( const V2& center, float radius, const V4& clr )
    {
        V2 c = transform(center);

        svg::Stroke s(lineWidth, CLR(clr), false, clr.w);
        doc << svg::Circle(PT(c), radius*scale()*2, svg::Fill(), s);
    }

    void SVGExport::fillCircle( const V2& center, float radius, const V4& clr )
    {
         V2 c = transform(center);

        svg::Fill f(CLR(clr), clr.w);
        doc << svg::Circle(PT(c), radius*scale()*2, f, svg::Stroke());
    }

    void SVGExport::strokeRect( float x, float y, float w, float h, const V4& clr )
    {
        V2 p(x, y);
        p = transform(p);
        svg::Stroke s(lineWidth, CLR(clr), false, clr.w);
        doc << svg::Rectangle(PT(p), w*scale(), h*scale(), svg::Fill(), s);
    }

    void SVGExport::fillRect( float x, float y, float w, float h, const V4& clr )
    {
        V2 p(x, y);
        p = transform(p);
        svg::Fill f(CLR(clr), clr.w);
        doc << svg::Rectangle(PT(p), w*scale(), h*scale(), f, svg::Stroke());
    }

    void SVGExport::drawLine( const V2& a, const V2& b, const V4& clr )
    {
        svg::Stroke s(lineWidth, CLR(clr), false, clr.w);
        svg::Polyline p(svg::Fill(), s);

        p << PT(transform(a));
        p << PT(transform(b));
        doc << p;
    }

    void SVGExport::drawArrow( const V2& a, const V2& b, float size, const V4& clr )
    {
        svg::Stroke s(lineWidth, CLR(clr), false, clr.w);
        svg::Polyline p(svg::Fill(), s);

        p << PT(transform(a));
        p << PT(transform(b));
        doc << p;
    }

    void SVGExport::setStipple(float d0, float offset)
    {

    }

}