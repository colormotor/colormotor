#pragma once
#include "cm.h"

namespace cm
{

int appInit( void *userData, int argc, char ** argv);
void appGui();
void appRender( float w, float h );
void appExit();
float appWidth();
float appHeight();
V2 appCenter();
    
class AppModule
{
public:
	/// The name will show up in the GUI eventually
	AppModule( const std::string name )
	:
    name(name),
	active(true)
	{
	}

	/// Initialize 
	virtual bool init() { return false; }
	virtual bool gui() { return false; }
	virtual void update() {}
	virtual void render() {}
	virtual void exit() {}

	virtual void activated() {}
	virtual void deactivated() {}
	

    std::string name;
	bool active;
};

}