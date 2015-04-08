///////////////////////////////////////////////////////////////////////
/// RESAPI	
///
/// © Daniel Berio 2010
/// http://www.enist.org
///
///////////////////////////////////////////////////////////////////////


#pragma once
#include "CM.h"

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
