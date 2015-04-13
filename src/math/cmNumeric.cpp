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
 --                                http://www.enist.org
 --                                drand48@gmail.com
 --
 --------------------------------------------------------------------
 ********************************************************************/
 
/****************************************************
 *
 * A series of utilities for numeric manipulation with
 * the std library. Attempts to imitiate numpy,
 * Easy to use but very unoptimized! :)
 * © Daniel Berio 2015
 * http://www.enist.org
 *
 ****************************************************/

#include "cmNumeric.h"



namespace cm
{
    /// Based on https://code.google.com/p/stacker/source/browse/trunk/GraphicsLibrary/OBB.h?spec=svn41&r=41
    /// And http://jamesgregson.blogspot.co.uk/2011/03/latex-test.html
    
    matrixf covariance2( const std::vector<Vec2> & P )
    {
        Vec3 mu(0.0, 0.0, 0.0);
        
        for( int i=0; i<P.size(); i++ )
        {
            mu += P[i];
        }
        
        mu/=float(P.size());
        
        float cxx=0.0, cxy=0.0, cyy=0.0;
        
        for( int i=0; i<P.size(); i++ )
        {
            const Vec2 & p = P[i];
            cxx += p.x*p.x - mu.x*mu.x;
            cxy += p.x*p.y - mu.x*mu.y;
            cyy += p.y*p.y - mu.y*mu.y;
        }
        
        matrixf C(2,2);
        C.zeros();
        
        C(0,0) = cxx; C(0,1) = cxy;
        C(1,0) = cxy; C(1,1) = cyy;
        
        return C;
    }
    
    matrixf covariance3( const std::vector<Vec3> & P )
    {
        Vec3 mu(0.0, 0.0, 0.0);
        
        for( int i=0; i<P.size(); i++ )
        {
            mu += P[i];
        }
        
        mu/=float(P.size());
        
        float cxx=0.0, cxy=0.0, cxz=0.0, cyy=0.0, cyz=0.0, czz=0.0;
        
        for( int i=0; i<P.size(); i++ )
        {
            const Vec3 & p = P[i];
            cxx += p.x*p.x - mu.x*mu.x;
            cxy += p.x*p.y - mu.x*mu.y;
            cxz += p.x*p.z - mu.x*mu.z;
            cyy += p.y*p.y - mu.y*mu.y;
            cyz += p.y*p.z - mu.y*mu.z;
            czz += p.z*p.z - mu.z*mu.z;
        }
        
        matrixf C(3,3);
        C.zeros();
        
        C(0,0) = cxx; C(0,1) = cxy; C(0,2) = cxz;
        C(1,0) = cxy; C(1,1) = cyy; C(1,2) = cyz;
        C(2,0) = cxz; C(2,1) = cyz; C(2,2) = czz;
        
        return C;
    }
    


}