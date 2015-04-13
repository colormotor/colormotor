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


#include "cm.h"
#include "cmMouse.h"

namespace cm
{



int Mouse::currentButton[MAX_MICE];
int Mouse::buttonState[MAX_MICE];
int Mouse::mousex[MAX_MICE]; 
int Mouse::mousey[MAX_MICE]; 
int Mouse::mousedx[MAX_MICE];
int Mouse::mousedy[MAX_MICE];
int &Mouse::x = Mouse::mousex[0];
int &Mouse::y = Mouse::mousey[0];
int &Mouse::dx = Mouse::mousedx[0];
int &Mouse::dy = Mouse::mousedy[0];


void Mouse::init() 
{
	for( int i = 0; i < MAX_MICE; i++ )
	{
		currentButton[i] = 0;
		buttonState[i] = 0;
		mousex[i] = mousey[i] = mousedy[i] = mousedx[i] = 0;
	}
}

// Hack to automatically init mouse...
class MouseInitializer
{
public:
	MouseInitializer()
	{
		Mouse::init();
	}
};

static MouseInitializer m;


}

