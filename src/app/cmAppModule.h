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
#include "cm.h"

namespace cm
{

class AppModule
{
	public:
		AppModule() { appName="app"; }
		
		virtual ~AppModule() {}
		
		virtual bool init() = 0;
		virtual void draw() {}
		virtual void update() {}
		virtual void exit() {}
		
		std::string appName;
};

}
