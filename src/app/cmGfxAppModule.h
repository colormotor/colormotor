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

#include "cmCore.h"
#include "cmGfx.h"
#include "cmAppModule.h"
#include "cmKeyboard.h"
#include "cmMouse.h"
#include "cmGui.h"

namespace cm //cmon
{

class GfxAppModule : 
public AppModule, 
public HasParams,
public KeyboardListener,
public MouseListener,
public KeyboardBroadcaster,
public MouseBroadcaster
{
	public:
		struct Viewport
		{
			int x;
			int y;
			int width;
			int height;
		};
		
		struct InputTextureInfo
		{
			std::string name;
			Texture * tex;
		};
		
		GfxAppModule() 
		:
		initialized(false),
		_gui(0)
		{}
		virtual ~GfxAppModule() {}
		
		
		int	getNumInputTextures() const { return inputTextures.size(); }
		
		Texture * getInputTexture( int i ) { return inputTextures[i]->tex; }
		InputTextureInfo * getInputTextureInfo( int i ) { return inputTextures[i]; }
			
		void releaseInputTextures() 
		{
			for( int i=0; i < getNumInputTextures(); i++ )
			{
				InputTextureInfo * info = getInputTextureInfo(i);
				if( info->tex )
					delete info->tex;
				delete info;
			}
			
			inputTextures.clear();
		}
		
		void addInputTexture( const char * name, Texture * tex = 0 ) 
		{ 
			InputTextureInfo * info = new InputTextureInfo();
			info->name = name;
			info->tex = tex;
			
			inputTextures.push_back(info);
		}

		Texture* loadInputTexture( const char * path)
		{
			Texture * tex = new Texture();
			if( !tex->load(path) )
			{
				delete tex;
				return 0;
			}
			
			InputTextureInfo * info = new InputTextureInfo();
			info->name = path;
			info->tex = tex;
			
			inputTextures.push_back(info);
			
			return tex;
		}
		
		virtual void setWindowSize( int w, int h ) {}
		virtual void setFullScreen( bool b = true ) {}
	
		virtual void resize( int w, int h ) {}
		virtual void createGui() {}
		virtual void updateGui() {}
		virtual void drawGui() {}
		
		Viewport viewport;
		
		std::vector <InputTextureInfo*> inputTextures;
		
		bool initialized;
		
		int mouseX;
		int mouseY;
		float mouseDX;
		float mouseDY;
		ParamList paramsToSave;
		
		Gui * _gui;
		
		double frameMsecs;
		
		void * windowHandle;

	protected:
		
		
		
};

}