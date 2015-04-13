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
#include "GL/glfw.h"

namespace cm
{


static GfxAppModule *_app = 0;
bool running = false;

//////////////////////////////////////////////////////////////////////////

static int glfwExit( void )
{
	running = false;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

static void glfwMouseButton( int button, int action )
{
	if(!_app)
		return;
		
	int x=0;
	int y=0;
	glfwGetMousePos(&x,&y);
	
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

static void glfwMousePos( int x, int y )
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
		
	static KEY conv[45] = 
	{
		KEY_ESCAPE,
		KEY_UNKNOWN, //#define GLFW_KEY_F1           (GLFW_KEY_SPECIAL+2)
		KEY_UNKNOWN, //#define GLFW_KEY_F2           (GLFW_KEY_SPECIAL+3)
		KEY_UNKNOWN, //#define GLFW_KEY_F3           (GLFW_KEY_SPECIAL+4)
		KEY_UNKNOWN, //#define GLFW_KEY_F4           (GLFW_KEY_SPECIAL+5)
		KEY_UNKNOWN, //#define GLFW_KEY_F5           (GLFW_KEY_SPECIAL+6)
		KEY_UNKNOWN, //#define GLFW_KEY_F6           (GLFW_KEY_SPECIAL+7)
		KEY_UNKNOWN, //#define GLFW_KEY_F7           (GLFW_KEY_SPECIAL+8)
		KEY_UNKNOWN, //#define GLFW_KEY_F8           (GLFW_KEY_SPECIAL+9)
		KEY_UNKNOWN, //#define GLFW_KEY_F9           (GLFW_KEY_SPECIAL+10)
		KEY_UNKNOWN, //#define GLFW_KEY_F10          (GLFW_KEY_SPECIAL+11)
		KEY_UNKNOWN, //#define GLFW_KEY_F11          (GLFW_KEY_SPECIAL+12)
		KEY_UNKNOWN, //#define GLFW_KEY_F12          (GLFW_KEY_SPECIAL+13)
		KEY_UNKNOWN, //#define GLFW_KEY_F13          (GLFW_KEY_SPECIAL+14)
		KEY_UNKNOWN, //#define GLFW_KEY_F14          (GLFW_KEY_SPECIAL+15)
		KEY_UNKNOWN, //#define GLFW_KEY_F15          (GLFW_KEY_SPECIAL+16)
		KEY_UNKNOWN, //#define GLFW_KEY_F16          (GLFW_KEY_SPECIAL+17)
		KEY_UNKNOWN, //#define GLFW_KEY_F17          (GLFW_KEY_SPECIAL+18)
		KEY_UNKNOWN, //#define GLFW_KEY_F18          (GLFW_KEY_SPECIAL+19)
		KEY_UNKNOWN, //#define GLFW_KEY_F19          (GLFW_KEY_SPECIAL+20)
		KEY_UNKNOWN, //#define GLFW_KEY_F20          (GLFW_KEY_SPECIAL+21)
		KEY_UNKNOWN, //#define GLFW_KEY_F21          (GLFW_KEY_SPECIAL+22)
		KEY_UNKNOWN, //#define GLFW_KEY_F22          (GLFW_KEY_SPECIAL+23)
		KEY_UNKNOWN, //#define GLFW_KEY_F23          (GLFW_KEY_SPECIAL+24)
		KEY_UNKNOWN, //#define GLFW_KEY_F24          (GLFW_KEY_SPECIAL+25)
		KEY_UNKNOWN, //#define GLFW_KEY_F25          (GLFW_KEY_SPECIAL+26)
		KEY_UP, //#define GLFW_KEY_UP           (GLFW_KEY_SPECIAL+27)
		KEY_DOWN, //#define GLFW_KEY_DOWN         (GLFW_KEY_SPECIAL+28)
		KEY_LEFT, //#define GLFW_KEY_LEFT         (GLFW_KEY_SPECIAL+29)
		KEY_RIGHT, //#define GLFW_KEY_RIGHT        (GLFW_KEY_SPECIAL+30)
		KEY_SHIFT_L, //#define GLFW_KEY_LSHIFT       (GLFW_KEY_SPECIAL+31)
		KEY_SHIFT_R, //#define GLFW_KEY_RSHIFT       (GLFW_KEY_SPECIAL+32)
		KEY_CTRL_L, //#define GLFW_KEY_LCTRL        (GLFW_KEY_SPECIAL+33)
		KEY_CTRL_R,//#define GLFW_KEY_RCTRL        (GLFW_KEY_SPECIAL+34)
		KEY_ALT_L, //#define GLFW_KEY_LALT         (GLFW_KEY_SPECIAL+35)
		KEY_ALT_R, //#define GLFW_KEY_RALT         (GLFW_KEY_SPECIAL+36)
		KEY_TAB, //#define GLFW_KEY_TAB          (GLFW_KEY_SPECIAL+37)
		KEY_ENTER, //#define GLFW_KEY_ENTER        (GLFW_KEY_SPECIAL+38)
		KEY_BACKSPACE,//#define GLFW_KEY_BACKSPACE    (GLFW_KEY_SPECIAL+39)
		KEY_INSERT,//#define GLFW_KEY_INSERT       (GLFW_KEY_SPECIAL+40)
		KEY_DELETE,//#define GLFW_KEY_DEL          (GLFW_KEY_SPECIAL+41)
		KEY_PAGE_UP, //#define GLFW_KEY_PAGEUP       (GLFW_KEY_SPECIAL+42)
		KEY_PAGE_DOWN,//#define GLFW_KEY_PAGEDOWN     (GLFW_KEY_SPECIAL+43)
		KEY_HOME,//#define GLFW_KEY_HOME         (GLFW_KEY_SPECIAL+44)
		KEY_END,//#define GLFW_KEY_END          (GLFW_KEY_SPECIAL+45)
	};
	
	if( key > GLFW_KEY_SPECIAL && key <= (GLFW_KEY_SPECIAL+45))
		return conv[key-GLFW_KEY_SPECIAL-1];
		
	return KEY_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////

static void glfwMouseWheel( int pos )
{
	if(!_app)
		return;
	
	_app->_gui->mouseWheel(pos);	
	_app->sendMouseWheel(pos);
	_app->mouseWheel(pos);
}

//////////////////////////////////////////////////////////////////////////

static void glfwKey( int key, int action )
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

static void glfwChar( int character, int action )
{
	_app->sendAscii( character );
	_app->ascii( character );
	if(_app->_gui && _app->_gui->visible )
		_app->_gui->ascii(character);
}

//////////////////////////////////////////////////////////////////////////

static void glfwWindowSize( int w, int h )
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
	
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwSwapInterval(1);
	//if(GLFWApplication::borderless)
	//	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, 1);
	
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE,1);
	if(!glfwOpenWindow(w,h,8,8,8,8,24,8, fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW))
		return false;
	
	glfwEnable(GLFW_MOUSE_CURSOR);
	
	// set GLFW callbacks.
	glfwSetWindowCloseCallback(&glfwExit);
	glfwSetMouseButtonCallback(&glfwMouseButton);
	glfwSetMousePosCallback(&glfwMousePos);
	glfwSetMouseWheelCallback(&glfwMouseWheel);
	glfwSetKeyCallback(&glfwKey);
	glfwSetCharCallback(&glfwChar);
	glfwSetWindowSizeCallback(glfwWindowSize);
	
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
		
		if( frameTime >= delta )
		{
			int x=0;
			int y=0;
			glfwGetMousePos(&x,&y);

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
		
		CM_GLCONTEXT
		
		glColor4f(1,1,1,1);
		app->draw();
		app->drawGui();
		
		// swap back and front buffers
		glfwSwapBuffers();
		
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
