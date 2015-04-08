///////////////////////////////////////////////////////////////////////////
//	 _________  __   ____  ___  __  _______  __________  ___
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.
//
//  Daniel Berio 2008-2011
//	http://www.enist.org/
//
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "gfx/cmGLInternal.h"
#include "gfx/cmRenderTarget.h"
#include "gfx/cmGfxUtils.h"

namespace cm
{
    
    
    static RenderTarget * currentRT = 0;
    
    static void releaseFbo( GLuint id )
    {
        CM_GLCONTEXT
        if(isGLReleaseManual())
            glDeleteFramebuffersEXT(1,&id);
        else
            releaseGLObject(GLObj(id,GLObj::FBO));
    }

    static void releaseRb( GLuint id )
    {
        CM_GLCONTEXT
        if(isGLReleaseManual())
            glDeleteRenderbuffersEXT(1,&id);
        else
            releaseGLObject(GLObj(id,GLObj::RENDERBUFFER));
    }
    
    ////////////////////////////////////////////////////////////
    
    
    RenderTarget::RenderTarget()
    {
        zero();
    }
    
    void RenderTarget::zero()
    {
        numColorAttachments = 0;
        hasDepthStencil = false;
        numSamples = 0;
        
        depthStencilId = 0;
        fbo = 0;
        fboTextures = 0;
        
        for( int i = 0; i < MAX_COLOR_ATTACHMENTS; i++ )
        {
            dirty[i] = false;
            textureAttachments[i] = 0;
            colorBuffers[i] = 0;
        }
        
        savedFramebuffer = 0;
    }
    
    RenderTarget::RenderTarget( int w, int h,
                               int pixelFormat,
                               bool depthStencil,
                               int depthFormat,
                               int numSamples )
    {
        zero();
        create2d(w,h,pixelFormat,depthStencil,depthFormat,numSamples);
    }
    
    
    
    ////////////////////////////////////////////////////////////
    
    
    
    RenderTarget::~RenderTarget()
    {
        release();
    }
    
    
    
    ////////////////////////////////////////////////////////////
    
    
    void	RenderTarget::release()
    {
        CM_GLCONTEXT;
        
        for( int i = 0; i < numColorAttachments; i++ )
        {
            if(colorBuffers[i])
                releaseRb(colorBuffers[i]);
            colorBuffers[i] = 0;
            if(textureAttachments[i])
                delete textureAttachments[i];
            textureAttachments[i] = 0;
        }
        
        if(fbo == fboTextures)
        {
            releaseFbo(fbo);
        }
        else
        {
            releaseFbo(fbo);
            releaseFbo(fboTextures);
        }
        
        if(depthStencilId)
            releaseRb(depthStencilId);
        depthStencilId = 0;
        
        savedFramebuffer = 0;
       
        zero();
    }
    

    ////////////////////////////////////////////////////////////
    bool RenderTarget::createMrt( int w, int h,
                                 std::vector<int> formats,
                                 bool hasDepthStencil,
                                 int depthStencilFormat,
                                 int numSamples )
 {
        release();
     
     
         // check if non power of 2 depth buffers are supported?
         if( cmAreNonPowerOfTwoTexturesSupported() )
         {
             width = w;
             height = h;
         }
         else
         {
             width = NPOT(w);
             height = NPOT(h);
         }

        this->numSamples = numSamples;
        this->hasDepthStencil = hasDepthStencil;
        this->depthStencilFormat = depthStencilFormat;
        this->formats = formats;
     
     
        // create main fbo
        // this is the main one we bind for drawing into
        // all the renderbuffers are attached to this (whether MSAA is enabled or not)
        glGenFramebuffersEXT(1, &fbo);
        bind(false);
        
     
        //currently depth only works if stencil is enabled.
        // http://forum.openframeworks.cc/index.php/topic,6837.0.html
     
        GLenum glDepthAttachment = GL_DEPTH_ATTACHMENT_EXT;
        if( depthStencilFormat == DepthStencil::D24_S8)
            glDepthAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
            
        static GLint glFmts[DepthStencil::NUM_FORMATS] =
        {
            GL_DEPTH_COMPONENT16,
            GL_DEPTH_COMPONENT24,
            GL_DEPTH24_STENCIL8_EXT,
            GL_DEPTH_COMPONENT32
        };

        GLint glDepthStencilFormat = glFmts[depthStencilFormat];
            
        //- USE REGULAR RENDER BUFFER
        if(hasDepthStencil)
        {
            depthStencilId = createAndAttachRenderbuffer(glDepthStencilFormat, glDepthAttachment);
        }
     
        // if we want MSAA, create a new fbo for textures
		if(numSamples){
			glGenFramebuffersEXT(1, &fboTextures);
		}else{
			fboTextures = fbo;
		}
     
        numColorAttachments = formats.size();
     
        // now create all textures and color buffers
        if(formats.size())
        {
            for(int i=0; i<formats.size(); i++)
                createAndAttachTexture(formats[i], i);
        }
     
        // if textures are attached to a different fbo (e.g. if using MSAA) check it's status
        if(fbo != fboTextures)
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboTextures);
        }
        
        // check everything is ok with this fbo
        valid = cmCheckFramebufferStatus();
        
        // unbind it
        unbind(false);
     
        return valid;
    }
    
    
    
    bool	RenderTarget::resize( int w, int h )
    {
        if( !valid )
        {
            debugPrint( "Render target not initialized, can't resize\n");
            return false;
        }
        
        return createMrt(w,h,formats,hasDepthStencil,depthStencilFormat,numSamples);
    }

    
    
    void RenderTarget::bind(bool viewPort)
    {
        CM_GLCONTEXT
        
        // set viewport with first color buffer size ( we assume that all buffers have same size )
        //NOTE: shouldn't the FBO be bound before calling setViewport ?
        if(viewPort)
            setViewport(0,0,textureAttachments[0]->getWidth(),textureAttachments[0]->getHeight(),false);
        
		glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &savedFramebuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    }
    
    
    void RenderTarget::unbind(bool viewPort)
    {
        CM_GLCONTEXT
        
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, savedFramebuffer);
        for( int i = 0; i < numColorAttachments; i++ )
            dirty[i] = true;

        if(viewPort)
            resetViewport();
    }
    
    Texture * RenderTarget::getTexture( int ind )
    {
        updateTexture(ind);
        return textureAttachments[ind];
    }
    
    void RenderTarget::updateTexture(int attachmentPoint)
    {
        if(!valid)
            return;
        
        if(fbo != fboTextures && dirty[attachmentPoint])
        {
            glGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &savedFramebuffer );
            
            glPushAttrib(GL_COLOR_BUFFER_BIT);
            
            // save current readbuffer
            GLint readBuffer;
            glGetIntegerv(GL_READ_BUFFER, &readBuffer);
            
            glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, fbo);
            glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, fboTextures);
            glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT + attachmentPoint);
            glReadBuffer(GL_COLOR_ATTACHMENT0_EXT + attachmentPoint);
            glBlitFramebufferEXT(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            
            glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, savedFramebuffer);
            glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, savedFramebuffer);
            glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, savedFramebuffer );
            
            // restore readbuffer
            glReadBuffer(readBuffer);
            
            glPopAttrib();
            
            dirty[attachmentPoint] = false;
        }
    }
    
    
    
    GLuint RenderTarget::createAndAttachRenderbuffer(GLenum internalFormat, GLenum attachmentPoint)
    {
        CM_GLCONTEXT
        
        GLuint buffer;
        glGenRenderbuffersEXT(1, &buffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, buffer);
        if(numSamples==0)
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, internalFormat, width, height);
        else
            glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, numSamples, internalFormat, width, height);
        
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachmentPoint, GL_RENDERBUFFER_EXT, buffer);
        return buffer;
    }
    
    
    void RenderTarget::createAndAttachTexture(int format, int attachmentPoint)
    {
        CM_GLCONTEXT
        
        // bind fbo for textures (if using MSAA this is the newly created fbo, otherwise its the same fbo as before)
        GLint temp;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &temp);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboTextures);
        
        Texture * tex = new Texture();
        tex->create(width,height,format,1);
        
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + attachmentPoint, GL_TEXTURE_2D, tex->info.glId,0);
        textureAttachments[attachmentPoint] = tex;
        
        // if MSAA, bind main fbo and attach renderbuffer
        if(numSamples) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
            
            GLuint colorBuffer = createAndAttachRenderbuffer(tex->info.glFormat, GL_COLOR_ATTACHMENT0_EXT + attachmentPoint);
            colorBuffers[attachmentPoint] = colorBuffer;
        }
        
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, temp);
    }
    
    
    //////////////////////////////////////////////////////////////////
    
    bool RenderTarget::isValid() const
    {
        return valid;
    }

    /*RenderTarget * RenderTarget::getCurrent()
    {
        return currentRT;
    }*/
    
}