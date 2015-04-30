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

#ifndef CM_OFX

#include "cmGLFWApp.h"
#include "GLFW/glfw3.h"
#include "cmGfx.h"

namespace cm
{


static GfxAppModule *_app = 0;
static GLFWwindow * win = 0;

bool running = false;

//////////////////////////////////////////////////////////////////////////

static void glfwExit( GLFWwindow * ww )
{
	running = false;
}

//////////////////////////////////////////////////////////////////////////

static void glfwMouseButton( GLFWwindow * ww, int button, int action, int mods )
{
	if(!_app)
		return;
		
	double x=0;
	double y=0;
	glfwGetCursorPos(ww, &x, &y);
	
	switch( button )
	{
		case GLFW_MOUSE_BUTTON_LEFT:
			if( action == GLFW_PRESS )
			{
				cm::Mouse::buttonState[0]  |= (1<<MOUSE_LEFT);
				
				if(_app->_gui && _app->_gui->visible )
					_app->_gui->mousePressed(x,y,MOUSE_LEFT);
				if(!_app->_gui || !_app->_gui->visible || !_app->_gui->hasFocus())
				{
					_app->sendMousePressed(x,y,MOUSE_LEFT);
					_app->mousePressed(x,y,MOUSE_LEFT);
				}
			}
			else
			{
				cm::Mouse::buttonState[0]  &= ~ (1<<MOUSE_LEFT);
				if(_app->_gui && _app->_gui->visible )
					_app->_gui->mouseReleased(x,y,MOUSE_LEFT);
				if(!_app->_gui || !_app->_gui->visible || !_app->_gui->hasFocus())
				{
					_app->sendMouseReleased(x,y,MOUSE_LEFT);
					_app->mouseReleased(x,y,MOUSE_LEFT);
				}
			}
			break;
			
		case GLFW_MOUSE_BUTTON_MIDDLE:
			if( action == GLFW_PRESS )
			{
				cm::Mouse::buttonState[0]  |= (1<<MOUSE_MIDDLE);
				if(_app->_gui && _app->_gui->visible )
					_app->_gui->mousePressed(x,y,MOUSE_MIDDLE);
				if(!_app->_gui || !_app->_gui->visible || !_app->_gui->hasFocus())
				{
					_app->sendMousePressed(x,y,MOUSE_MIDDLE);
					_app->mousePressed(x,y,MOUSE_MIDDLE);
				}
			}
			else
			{
				cm::Mouse::buttonState[0]  &= ~ (1<<MOUSE_MIDDLE);
				if(_app->_gui && _app->_gui->visible )
					_app->_gui->mouseReleased(x,y,MOUSE_MIDDLE);
				if(!_app->_gui || !_app->_gui->visible || !_app->_gui->hasFocus())
				{
					_app->sendMouseReleased(x,y,MOUSE_MIDDLE);
					_app->mouseReleased(x,y,MOUSE_MIDDLE);
				}
			}
			break;
			
		case GLFW_MOUSE_BUTTON_RIGHT:
			if( action == GLFW_PRESS )
			{
				cm::Mouse::buttonState[0]  |= (1<<MOUSE_RIGHT);
				if(_app->_gui && _app->_gui->visible )
					_app->_gui->mousePressed(x,y,MOUSE_RIGHT);
				if(!_app->_gui || !_app->_gui->visible || !_app->_gui->hasFocus())
				{
					_app->sendMousePressed(x,y,MOUSE_RIGHT);
					_app->mousePressed(x,y,MOUSE_RIGHT);
				}
			}
			else
			{
				cm::Mouse::buttonState[0]  &= ~ (1<<MOUSE_RIGHT);
				if(_app->_gui && _app->_gui->visible )
					_app->_gui->mouseReleased(x,y,MOUSE_RIGHT);
				if(!_app->_gui || !_app->_gui->visible || !_app->_gui->hasFocus())
				{
					_app->sendMouseReleased(x,y,MOUSE_RIGHT);
					_app->mouseReleased(x,y,MOUSE_RIGHT);
				}
			}
			break;
	}
}

static void glfwMousePos( GLFWwindow * ww, double x, double y )
{
	if(!_app)
		return;
		
	// use flagging? & etc..
	unsigned int flags = 0;
	
	if( cm::Mouse::isButtonDown(MOUSE_LEFT) )
	{
		flags |= (1<<MOUSE_LEFT);
	}
	
	if( cm::Mouse::isButtonDown(MOUSE_MIDDLE) )
	{
		flags |= (1<<MOUSE_MIDDLE);
	}
	
	if( cm::Mouse::isButtonDown(MOUSE_RIGHT) )
	{
		flags |= (1<<MOUSE_RIGHT);
	}

	if(flags)
	{
		if(_app->_gui && _app->_gui->visible )
			_app->_gui->mouseDragged(x,y,flags);
		if(!_app->_gui || !_app->_gui->visible || !_app->_gui->hasFocus())
		{
			_app->sendMouseDragged(x,y,flags);
			_app->mouseDragged(x,y,flags);
		}
	}
	else
	{
		_app->sendMouseMoved(x,y);
		_app->mouseMoved(x,y);
	}
}

//////////////////////////////////////////////////////////////////////////

static int getKeyFromGLFW(int key)
{
	if(key>='0' && key<='9')
		return key;
	if(key>='A' && key<='Z')
		return tolower(key);
		

	static int conv[] =
	{
		KEY_ESCAPE   ,
		KEY_ENTER    ,
		KEY_TAB      ,
		KEY_BACKSPACE,
		KEY_INSERT   ,
		KEY_DELETE   ,
		KEY_RIGHT    ,
		KEY_LEFT     ,
		KEY_DOWN     ,
		KEY_UP       ,
		KEY_PAGE_UP  ,
		KEY_PAGE_DOWN,
		KEY_HOME     ,
		KEY_END      ,  //269
        
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        
		KEY_CAPS_LOCK   , //280
		KEY_SCROLL_LOCK ,
		KEY_NUM_LOCK    ,
		KEY_UNKNOWN		,
		KEY_PAUSE       , // 284
        
        
        
        KEY_UNKNOWN		, // 285
        KEY_UNKNOWN		, // 286
        KEY_UNKNOWN		, // 287
        KEY_UNKNOWN		, // 288
        KEY_UNKNOWN		, // 289

        
		KEY_UNKNOWN		, //KEY_F1          ,290
		KEY_UNKNOWN		, //KEY_F2          ,
		KEY_UNKNOWN		, //KEY_F3          ,
		KEY_UNKNOWN		, //KEY_F4          ,
		KEY_UNKNOWN		, //KEY_F5          ,
		KEY_UNKNOWN		, //KEY_F6          ,
		KEY_UNKNOWN		, //KEY_F7          ,
		KEY_UNKNOWN		, //KEY_F8          ,
		KEY_UNKNOWN		, //KEY_F9          ,
		KEY_UNKNOWN		, //KEY_F10         ,
		KEY_UNKNOWN		, //KEY_F11         ,
		KEY_UNKNOWN		, //KEY_F12         ,
		KEY_UNKNOWN		, //KEY_F13         ,
		KEY_UNKNOWN		, //KEY_F14         ,
		KEY_UNKNOWN		, //KEY_F15         ,
		KEY_UNKNOWN		, //KEY_F16         ,
		KEY_UNKNOWN		, //KEY_F17         ,
		KEY_UNKNOWN		, //KEY_F18         ,
		KEY_UNKNOWN		, //KEY_F19         ,
		KEY_UNKNOWN		, //KEY_F20         ,
		KEY_UNKNOWN		, //KEY_F21         ,
		KEY_UNKNOWN		, //KEY_F22         ,
		KEY_UNKNOWN		, //KEY_F23         ,
		KEY_UNKNOWN		, //KEY_F24         ,
		KEY_UNKNOWN		, //KEY_F25         , 314
        
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        
		KEY_UNKNOWN		, //KEY_KP_0        , 320
		KEY_UNKNOWN		, //KEY_KP_1        ,
		KEY_UNKNOWN		, //KEY_KP_2        ,
		KEY_UNKNOWN		, //KEY_KP_3        ,
		KEY_UNKNOWN		, //KEY_KP_4        ,
		KEY_UNKNOWN		, //KEY_KP_5        ,
		KEY_UNKNOWN		, //KEY_KP_6        ,
		KEY_UNKNOWN		, //KEY_KP_7        ,
		KEY_UNKNOWN		, //KEY_KP_8        ,
		KEY_UNKNOWN		, //KEY_KP_9        ,
		KEY_UNKNOWN		, //KEY_KP_DECIMAL  ,
		KEY_UNKNOWN		, //KEY_KP_DIVIDE   ,
		KEY_UNKNOWN		, //KEY_KP_MULTIPLY ,
		KEY_UNKNOWN		, //KEY_KP_SUBTRACT ,
		KEY_UNKNOWN		, //KEY_KP_ADD      ,
		KEY_UNKNOWN		, //KEY_KP_ENTER    ,
		KEY_UNKNOWN		, //KEY_KP_EQUAL    , 336
        
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        KEY_UNKNOWN		,
        
		KEY_SHIFT_L		, //KEY_LEFT_SHIFT  , 340
		KEY_CTRL_L		, //KEY_LEFT_CONTROL,
		KEY_ALT_L		, //KEY_LEFT_ALT    ,
		KEY_UNKNOWN		, //KEY_LEFT_SUPER  ,
		KEY_SHIFT_R		, //KEY_RIGHT_SHIFT ,
		KEY_CTRL_R		, //KEY_RIGHT_CONTROL,
		KEY_ALT_R		, //KEY_RIGHT_ALT   ,
		KEY_UNKNOWN		, //KEY_RIGHT_SUPER ,
		KEY_UNKNOWN		, //KEY_MENU        ,
	};
#define ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))
    int arsz = ARRAYSIZE(conv);
	
	if( key >= GLFW_KEY_ESCAPE && key <= (GLFW_KEY_MENU))
    {
        int ind = key-GLFW_KEY_ESCAPE;
        int res = conv[ind];
        int test;
        test = KEY_UNKNOWN;
        test = KEY_CTRL_L;
        return res; //conv[key-GLFW_KEY_ESCAPE];
    }
	return KEY_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////

static void glfwMouseWheel( GLFWwindow * ww, double pos, double y )
{
	if(!_app)
		return;
	
	_app->_gui->mouseWheel(pos);	
	_app->sendMouseWheel(pos);
	_app->mouseWheel(pos);
}

//////////////////////////////////////////////////////////////////////////

static void glfwKey( GLFWwindow * ww, int key, int scanCode, int action, int mods )
{
	if(!_app)
		return;
		
	int k = getKeyFromGLFW(key);
	if( action == GLFW_PRESS )
	{
		cm::Keyboard::setKeyPressed(k,true);
		
		if(_app->_gui && _app->_gui->visible )
			_app->_gui->keyPressed(k);
		if(!_app->_gui || !_app->_gui->visible || !_app->_gui->hasFocus())
		{
			_app->sendKeyPressed(k);	
			_app->keyPressed(k);
		}
		
	}
	else
	{
		cm::Keyboard::setKeyPressed(k,false);
		
		if(_app->_gui && _app->_gui->visible )
			_app->_gui->keyReleased(k);
		if(!_app->_gui || !_app->_gui->visible || !_app->_gui->hasFocus())
		{
			_app->sendKeyReleased(k);	
			_app->keyReleased(k);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

static void glfwChar( GLFWwindow * ww, unsigned int character )
{
	_app->sendAscii( character );
	_app->ascii( character );
	if(_app->_gui && _app->_gui->visible )
		_app->_gui->ascii(character);
}

//////////////////////////////////////////////////////////////////////////

static void glfwWindowSize( GLFWwindow * ww, int w, int h )
{
	_app->viewport.x = 0;
	_app->viewport.y = 0;
	_app->viewport.width = w;
	_app->viewport.height = h;
	gfx::setViewport(0,0,w,h);
	_app->resize( w, h );
	if(_app->_gui && _app->_gui->visible )
		_app->_gui->resize(w,h);
}


static int omx = 0;
static int omy = 0;

bool setupGlfwApp( GfxAppModule * app, int w, int h, bool fullscreen, const char * title )
{
	_app = app;
	
	if (glfwInit() != GL_TRUE)
		return false;
	
	
	glfwSwapInterval(1);
	//if(GLFWApplication::borderless)
	//	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, 1);
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_VISIBLE,GL_FALSE);
	// double buffering
	//glfwWindowHint(GLFW_AUX_BUFFERS,1);

	glfwWindowHint(GLFW_SAMPLES,4);
	glfwWindowHint(GLFW_RESIZABLE,0);
	//glfwWindowHint(GLFW_DECORATED, 1);
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	if(fullscreen)
	{
		win = glfwCreateWindow(w,h, "",  glfwGetPrimaryMonitor(), NULL );
	}
	else
	{
		win = glfwCreateWindow(w, h, title, NULL, NULL);
	}

	glfwShowWindow(win);

	//if(!glfwOpenWindow(w,h,8,8,8,8,24,8, fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW))
	//	return false;
	glfwMakeContextCurrent(win);
	
	// fix retina if necessary
	// from OF code
	int retinaw,retinah;
	glfwGetWindowSize(win, &w, &h );
    
    
	int framebufferW, framebufferH;
    glfwGetFramebufferSize(win, &framebufferW, &framebufferH);
    
    //this lets us detect if the window is running in a retina mode
    
    if( framebufferW != w ){
        float pixelScreenCoordScale = (float)framebufferW / w;
        gfx::setPixelScale(pixelScreenCoordScale);

        //have to update the windowShape to account for retina coords
        if( !fullscreen )
        {
            //glfwSetWindowSize(win, w/pixelScreenCoordScale, h/pixelScreenCoordScale);
        }
	}
	
	
	// set GLFW callbacks.
	glfwSetWindowCloseCallback(win, &glfwExit);
	glfwSetMouseButtonCallback(win, &glfwMouseButton);
	glfwSetCursorPosCallback(win, &glfwMousePos);
	glfwSetScrollCallback(win, &glfwMouseWheel);
	glfwSetKeyCallback(win, &glfwKey);
	glfwSetCharCallback(win, &glfwChar);
	glfwSetWindowSizeCallback(win, glfwWindowSize);
	
    glfwMakeContextCurrent(win);
	CM_GLCONTEXT
	glEnable(GL_MULTISAMPLE);
	//glfwSwapInterval(1);

	app->viewport.x = 0;
	app->viewport.y = 0;
	app->viewport.width = w;
	app->viewport.height = h;
	
	int buttons = 0;
	
	app->params.loadXmlFile("app.xml");
	app->paramsToSave.loadXmlFile("params.xml");
	
	if(!app->init())
		return false;
	
	// we load params twice cause some might be added after init...
	app->params.loadXmlFile("app.xml");
	app->paramsToSave.loadXmlFile("params.xml");
	
	app->createGui();
	app->resize(w,h);
	if( app->_gui )
		app->_gui->resize(w,h);
		
	return true;
}

bool beginGlfwLoop( GfxAppModule * app )
{
	static double curTime = getTickCount();
	int n = 0;
	double frameRate = 60;
	double delta = 1000.0 / frameRate;
	double frameTime = 0;
	
	running = true;
	while(running)
	{
		double t = getTickCount();
		double msecs = t-curTime;
		if( msecs > 1000 )
			msecs = 30;
		curTime = t;
		updateTime(t);
		
		frameTime += msecs;
		
		app->frameMsecs = msecs;
		
        glfwPollEvents();
        
		if( frameTime >= delta )
		{
			double x=0;
			double y=0;
			glfwGetCursorPos(win, &x, &y);

			cm::Mouse::mousex[0] = x;
			cm::Mouse::mousey[0] = y;
			cm::Mouse::mousedx[0] = x - omx;
			cm::Mouse::mousedy[0] = y - omy;
			
			app->mouseX = x;
			app->mouseY = y;
			app->mouseDX = cm::Mouse::mousedx[0];
			app->mouseDY = cm::Mouse::mousedy[0];
			
			omx = x;
			omy = y;
			
			//std::string title;
			//stdPrintf(title,"COLORMOTOR:%g fps",getFPS());
			//glfwSetWindowTitle(title.c_str());
			
			app->updateGui();
			
			app->update();
			
			frameTime = 0;
		}
		
		glfwMakeContextCurrent(win);
		CM_GLCONTEXT
		
        int framebufferW, framebufferH;
	    glfwGetFramebufferSize(win, &framebufferW, &framebufferH);
	    
	    //this lets us detect if the window is running in a retina mode
    
        float pixelScreenCoordScale = (float)framebufferW / app->viewport.width;
        gfx::setPixelScale(pixelScreenCoordScale);

        gfx::setViewport(0,0, gfx::getPixelScale()*app->viewport.width, gfx::getPixelScale()*app->viewport.height);
		// glColor4f(1,1,1,1);
		// glClearColor(1,0,0,1);
		// glClear(GL_COLOR_BUFFER_BIT);
	
		app->draw();
		app->drawGui();
		
        glFlush();
		// swap back and front buffers
		glfwSwapBuffers(win);
		
		usleep(5000);
	}
	
	app->params.saveXmlFile("app.xml");
	app->paramsToSave.saveXmlFile("params.xml");
	
	app->exit();
	
	if(_app->_gui && _app->_gui->visible )
		app->_gui->release();
	
	glfwTerminate();	
	
	 // Exit program
    exit( EXIT_SUCCESS );
}

bool runGlfwApp( GfxAppModule * app, int w, int h, bool fullscreen, const char * title )
{
	if(!setupGlfwApp( app,w,h,fullscreen,title ))
		return false;
	
	return beginGlfwLoop(app);
}
	
	
	
}

#endif
