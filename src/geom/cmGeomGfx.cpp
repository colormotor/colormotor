//
//  cmGeomGfx.cpp
//  colormotor
//
//  Created by colormotor on 10/16/13.
//
//

#include "cmGeomGfx.h"
#include "cmConfig.h"
#include "EpsFile.h"

namespace cm
{
	
    namespace gfx
    {
        
        // TriMesh2d
        void draw( const TriMesh2d &mesh )
        {
            if( mesh.getNumVertices() <= 0 )
                return;
            
            glVertexPointer( 2, GL_FLOAT, 0, &(mesh.getVertices()[0]) );
            glEnableClientState( GL_VERTEX_ARRAY );
            
            glDisableClientState( GL_NORMAL_ARRAY );
            
            if( mesh.hasColorsRgba() ) {
                glColorPointer( 4, GL_FLOAT, 0, &(mesh.getColorsRGBA()[0]) );
                glEnableClientState( GL_COLOR_ARRAY );
            }
            else
                glDisableClientState( GL_COLOR_ARRAY );
            
            if( mesh.hasTexCoords() ) {
                glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.getTexCoords()[0]) );
                glEnableClientState( GL_TEXTURE_COORD_ARRAY );
            }
            else
                glDisableClientState( GL_TEXTURE_COORD_ARRAY );
            glDrawElements( GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, &(mesh.getIndices()[0]) );
            
            glDisableClientState( GL_VERTEX_ARRAY );
            glDisableClientState( GL_NORMAL_ARRAY );
            glDisableClientState( GL_COLOR_ARRAY );
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        }
        
        // TriMesh2d
        void drawRange( const TriMesh2d &mesh, size_t startTriangle, size_t triangleCount )
        {
            glVertexPointer( 2, GL_FLOAT, 0, &(mesh.getVertices()[0]) );
            glEnableClientState( GL_VERTEX_ARRAY );
            
            glDisableClientState( GL_NORMAL_ARRAY );
            
            if( mesh.hasColorsRgba() ) {
                glColorPointer( 4, GL_FLOAT, 0, &(mesh.getColorsRGBA()[0]) );
                glEnableClientState( GL_COLOR_ARRAY );
            }
            else
                glDisableClientState( GL_COLOR_ARRAY );
            
            if( mesh.hasTexCoords() ) {
                glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.getTexCoords()[0]) );
                glEnableClientState( GL_TEXTURE_COORD_ARRAY );
            }
            else
                glDisableClientState( GL_TEXTURE_COORD_ARRAY );
            
            if(mesh.getNumIndices())
                glDrawRangeElements( GL_TRIANGLES, 0, mesh.getNumVertices(), triangleCount * 3, GL_UNSIGNED_INT, &(mesh.getIndices()[startTriangle*3]) );
            else
                glDrawArrays(GL_TRIANGLES, startTriangle*3, triangleCount*3);
            
            glDisableClientState( GL_VERTEX_ARRAY );
            glDisableClientState( GL_NORMAL_ARRAY );
            glDisableClientState( GL_COLOR_ARRAY );
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        }
        
        // TriMesh
        void draw( const TriMesh &mesh )
        {
            glVertexPointer( 3, GL_FLOAT, 0, &(mesh.getVertices()[0]) );
            glEnableClientState( GL_VERTEX_ARRAY );
            
            if( mesh.hasNormals() ) {
                glNormalPointer( GL_FLOAT, 0, &(mesh.getNormals()[0]) );
                glEnableClientState( GL_NORMAL_ARRAY );
            }
            else
                glDisableClientState( GL_NORMAL_ARRAY );
            
            if( mesh.hasColorsRGB() ) {
                glColorPointer( 3, GL_FLOAT, 0, &(mesh.getColorsRGB()[0]) );
                glEnableClientState( GL_COLOR_ARRAY );
            }
            else if( mesh.hasColorsRGBA() ) {
                glColorPointer( 4, GL_FLOAT, 0, &(mesh.getColorsRGBA()[0]) );
                glEnableClientState( GL_COLOR_ARRAY );
            }
            else
                glDisableClientState( GL_COLOR_ARRAY );
            
            if( mesh.hasTexCoords() ) {
                glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.getTexCoords()[0]) );
                glEnableClientState( GL_TEXTURE_COORD_ARRAY );
            }
            else
                glDisableClientState( GL_TEXTURE_COORD_ARRAY );
            
            if(mesh.getNumIndices())
                glDrawElements( GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, &(mesh.getIndices()[0]) );
            else
                glDrawArrays(GL_TRIANGLES, 0, mesh.getNumVertices());
            
            glDisableClientState( GL_VERTEX_ARRAY );
            glDisableClientState( GL_NORMAL_ARRAY );
            glDisableClientState( GL_COLOR_ARRAY );
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        }
        
        // TriMesh2d
        void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount )
        {
            glVertexPointer( 3, GL_FLOAT, 0, &(mesh.getVertices()[0]) );
            glEnableClientState( GL_VERTEX_ARRAY );
            
            if( mesh.hasNormals() ) {
                glNormalPointer( GL_FLOAT, 0, &(mesh.getNormals()[0]) );
                glEnableClientState( GL_NORMAL_ARRAY );
            }
            else
                glDisableClientState( GL_NORMAL_ARRAY );
            
            if( mesh.hasColorsRGB() ) {
                glColorPointer( 3, GL_FLOAT, 0, &(mesh.getColorsRGB()[0]) );
                glEnableClientState( GL_COLOR_ARRAY );
            }
            else if( mesh.hasColorsRGBA() ) {
                glColorPointer( 4, GL_FLOAT, 0, &(mesh.getColorsRGBA()[0]) );
                glEnableClientState( GL_COLOR_ARRAY );
            }
            else
                glDisableClientState( GL_COLOR_ARRAY );
            
            if( mesh.hasTexCoords() ) {
                glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.getTexCoords()[0]) );
                glEnableClientState( GL_TEXTURE_COORD_ARRAY );
            }
            else
                glDisableClientState( GL_TEXTURE_COORD_ARRAY );
            
            if(mesh.getNumIndices())
                glDrawRangeElements( GL_TRIANGLES, 0, mesh.getNumVertices(), triangleCount * 3, GL_UNSIGNED_INT, &(mesh.getIndices()[startTriangle*3]) );
            else
                glDrawArrays(GL_TRIANGLES, startTriangle*3, triangleCount*3);
            
            glDisableClientState( GL_VERTEX_ARRAY );
            glDisableClientState( GL_NORMAL_ARRAY );
            glDisableClientState( GL_COLOR_ARRAY );
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        }
        
        void draw( const std::vector <Vec2> & points, bool close )
        {
            draw( &points[0], points.size(), close );
        }
        
        void draw( const Vec2 * points, int n, bool close )
        {
            glEnableClientState( GL_VERTEX_ARRAY );
            glVertexPointer( 2, GL_FLOAT, 0, points );
            if( close )
                glDrawArrays( GL_LINE_LOOP, 0, n );
            else
                glDrawArrays( GL_LINE_STRIP, 0, n );
            glDisableClientState( GL_VERTEX_ARRAY );

#ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                Path2d path;
                for( int i = 0; i < n; i++ )
                {
                    path.addPoint(points[i]);
                }
                if(close)
                {
                    path.close();
                }
                gfx::getEps()->strokeShape(path, gfx::getCurrentColor());
            }
#endif
        }

        
        void drawSolid( const std::vector <Vec2> & points, int winding  )
        {
            drawSolid(&points[0], points.size(), winding);
        }
        
        void drawSolid( const Vec2 * points, int n, int winding )
        {
            draw( Triangulator( points,n ).calcMesh((Triangulator::Winding)winding) );
            #ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                Path2d path;
                for( int i = 0; i < n; i++ )
                {
                    path.addPoint(points[i]);
                }
                if(close)
                {
                    path.close();
                }
                gfx::getEps()->fillShape(path, gfx::getCurrentColor());
            }
            #endif      
        }
        
        
        /*
         void draw( const std::vector <Vec2> & points, bool close )
         {
         if( points.size() == 0 )
         return;
         
         glEnableClientState( GL_VERTEX_ARRAY );
         glVertexPointer( 2, GL_FLOAT, 0, &(points[0]) );
         if( close )
         glDrawArrays( GL_LINE_LOOP, 0, points.size() );
         else
         glDrawArrays( GL_LINE_STRIP, 0, points.size() );
         glDisableClientState( GL_VERTEX_ARRAY );
         }*/
        
        void draw( const Path2d &path2d, float approximationScale )
        {
            if( path2d.getNumSegments() == 0 || path2d.size() < 2 )
                return;
            std::vector<Vec2f> points = path2d.subdivide( approximationScale );
            glEnableClientState( GL_VERTEX_ARRAY );
            glVertexPointer( 2, GL_FLOAT, 0, &(points[0]) );
            glDrawArrays( GL_LINE_STRIP, 0, points.size() );
            glDisableClientState( GL_VERTEX_ARRAY );

            #ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                gfx::getEps()->strokeShape(path2d, gfx::getCurrentColor());
            }
            #endif 	
        }
        
        
        
        void draw( const Shape2d &shape2d, float approximationScale )
        {
            glEnableClientState( GL_VERTEX_ARRAY );
            for( std::vector<Path2d>::const_iterator contourIt = shape2d.getContours().begin(); contourIt != shape2d.getContours().end(); ++contourIt ) {
                if( contourIt->getNumSegments() == 0 )
                    continue;
                std::vector<Vec2f> points = contourIt->subdivide( approximationScale );
                glVertexPointer( 2, GL_FLOAT, 0, &(points[0]) );
                glDrawArrays( GL_LINE_STRIP, 0, points.size() );
            }
            glDisableClientState( GL_VERTEX_ARRAY );	

            #ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                gfx::getEps()->strokeShape(shape2d, gfx::getCurrentColor());
            }
            #endif  
        }
        
        void drawSolid( const Path2d &path2d, float approximationScale,int winding )
        {
            if(path2d.size()<3)
                return;
            
            draw( Triangulator( path2d,approximationScale ).calcMesh((Triangulator::Winding)winding) );

             #ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                gfx::getEps()->fillShape(path2d, gfx::getCurrentColor());
            }
            #endif 
        }
        
        void drawSolid( const Shape2d &shape2d, float approximationScale,int winding )
        {
            if(!shape2d.size())
                return;
            
            draw( Triangulator( shape2d,approximationScale ).calcMesh((Triangulator::Winding)winding) );

             #ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                gfx::getEps()->fillShape(shape2d, gfx::getCurrentColor());
            }
            #endif 
        }
        
        TriMesh2d makeMesh( const Path2d & path, float approximationScale, int winding  )
        {
            return Triangulator( path,approximationScale ).calcMesh((Triangulator::Winding)winding);
        }
        
        TriMesh2d makeMesh( const Shape2d & shape, float approximationScale, int winding )
        {
            return Triangulator( shape,approximationScale ).calcMesh((Triangulator::Winding)winding);
        }
        
        
        void draw( const Rectf & r )
        {
            drawRect(r.l,r.t,r.getWidth(),r.getHeight());
        }
        
        
        void drawSpiral( const Vec2 & pos, float a, float thetaInc, int n )
        {
            gfx::beginVertices( gfx::LINESTRIP );
            for( int i = 0; i < n; i++ )
            {
                gfx::vertex2( pos+spiralToCartesian(thetaInc*i,a));
            }
            gfx::endVertices();
        }
        
        

        void draw( const Contour &s )
        {
            glEnableClientState( GL_VERTEX_ARRAY );
            glVertexPointer( 2, GL_FLOAT, 0, &s.points[0] );
            if( s.isClosed() )
                glDrawArrays( GL_LINE_LOOP, 0, s.size() );
            else
                glDrawArrays( GL_LINE_STRIP, 0, s.size() );
            glDisableClientState( GL_VERTEX_ARRAY );
            
        #ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                gfx::getEps()->strokeShape(s, gfx::getCurrentColor());
            }
        #endif
        }
            
        void fill( const Contour &s, int winding )
        {
            gfx::draw( Triangulator( &s.points[0], s.size() ).calcMesh((Triangulator::Winding)winding) );
        #ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                gfx::getEps()->fillShape(s, gfx::getCurrentColor());
            }
        #endif
        }

        void draw( const Shape &s )
        {
            glEnableClientState( GL_VERTEX_ARRAY );
            for( int i = 0; i < s.size(); i++ )
            {
                const Contour & ctr = s[i];
                if( ctr.size() < 2 )
                    continue;
                glVertexPointer( 2, GL_FLOAT, 0, &(ctr.points[0]) );
                glDrawArrays( GL_LINE_STRIP, 0, ctr.size() );
            }
            glDisableClientState( GL_VERTEX_ARRAY );
            
        #ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                gfx::getEps()->strokeShape(s, gfx::getCurrentColor());
            }
        #endif
        }

        void fill( const Shape &s,int winding )
        {
            if(!s.size())
                return;
            
            draw( Triangulator( s ).calcMesh((Triangulator::Winding)winding) );

             #ifdef GFX_TO_EPS
            if(gfx::isRenderingToEps())
            {
                gfx::getEps()->fillShape(s, gfx::getCurrentColor());
            }
            #endif 
        }

    }
    
    
}