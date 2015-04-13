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

#pragma once

#include "gfx/cmGfxIncludes.h"
#include "gfx/cmTexture.h"


namespace cm
{
    
  	/// Depth stencil formats  
    class DepthStencil
    {
    public:
        
        enum FORMAT
        {
            FORMAT_UNKNOWN = -1,
            D16 = 0		,
            D24,
            D24_S8,
            D32,
            NUM_FORMATS
        };
    };
    
    
#define MAX_COLOR_ATTACHMENTS	4 // THIS CAN BE DINAMICALLY CREATED BASED ON HW.
    
    
#define DEFAULT_COLOR_FMT	Texture::A8R8G8B8
#define DEFAULT_DEPTH_FMT	DepthStencil::D24_S8


class RenderTarget : public GfxObject
{
public:
    RenderTarget();
    RenderTarget( int w, int h,
                 int pixelFormat = DEFAULT_COLOR_FMT,
                 bool depth = true,
                 int depthFormat = DEFAULT_DEPTH_FMT,
                 int numSamples = 0 );
    
    void zero();
    
    ~RenderTarget();
    
    /// Release render target
    void	release();
    
    /// Create a 2D render target
    /// \param w Width.
    /// \param h Height.
    /// \param pixelFormat Format for the render target texture.
    /// \param depth If true also a depth buffer is created for the render target.
    /// \param depthFormat Optional format for the depth buffer if present.
    bool createMrt( int w, int h,
                     std::vector<int> formats,
                     bool hasDepthStencil = true,
                     int depthStencilFormat = DEFAULT_DEPTH_FMT,
                     int numSamples = 0 );
    
    bool create2d( int w, int h,
                     int pixelFormat = DEFAULT_COLOR_FMT,
                     bool depth = true,
                     int depthFormat = DEFAULT_DEPTH_FMT,
                     int numSamples = 0 )
    {
        std::vector<int> fmts;
        fmts.push_back(pixelFormat);
        return createMrt( w,h,fmts,depth,depthFormat,numSamples);
    }
    
    
    /// Returns true if render target has been initialized
    bool	isValid() const;
    
    /// Resizes render target
    bool	resize( int w, int h );
    
    /// Sets render target as current
    void	bind( bool viewPort = true );
    void	unbind( bool viewPort = true );
    
    Texture * getTexture( int ind = 0 );

    void updateTexture( int ind = 0 );
        
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    

protected:
    GLuint createAndAttachRenderbuffer(GLenum internalFormat, GLenum attachmentPoint);
    void createAndAttachTexture(int format, int attachmentPoint);
    bool bindDepth();
    
    bool valid;
    GLint savedFramebuffer;
    
    bool dirty[MAX_COLOR_ATTACHMENTS];
    Texture * textureAttachments[MAX_COLOR_ATTACHMENTS];
    GLuint colorBuffers[MAX_COLOR_ATTACHMENTS];
    int numColorAttachments;
    
    std::vector<int> formats;
    bool hasDepthStencil;
    int depthStencilFormat;
    int numSamples;

    int depthStencilId;
    GLuint fbo;
    GLuint fboTextures;
    
    int width;
    int height;

};
    
    
}

