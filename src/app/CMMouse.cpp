//////////////////////////////////////////////////////////////////////////                                                     
//	 _________  __   ____  ___  __  _______  __________  ___			
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \			
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/			
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.		
//																		
//	� Daniel Berio 2008													
//	  http://www.enist.org/												
//																	
//																		
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#include "CM.h"
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

