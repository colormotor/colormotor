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

template <class PT, class RT> class Triangle2D
{
public:
	PT a;
	PT b;
	PT c;
	
	Triangle2D() {}
	Triangle2D( const PT & a, const PT & b, const PT & c )
	:
	a(a),b(b),c(c)
	{
	}
	
	bool isPointIn( const PT & p ) const;
	RT area();
	PT centroid() const;
	PT orthocenter();
	PT circumcenter;
	
};