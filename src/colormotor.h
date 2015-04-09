

#pragma once

#include "cmCore.h"
#include "cmMath.h"
#include "cmGfx.h"
#include "cmGeom.h"
#include "cmApp.h"

#ifndef CM_DONT_AUTOUSE_NAMESPACE
using namespace cm;
#endif

#ifdef CM_OFX

class ColormotorEventBridge
{
public:
	ColormotorEventBridge()
	:
	active(true)
	{

	}

	void addMouseListener( MouseListener * l )
	{
		mouseListeners.push_back(l);
	}

	void addKeyboardListener( KeyboardListener * l )
	{
		keyListeners.push_back(l);
	}

    void setup(){
        ofRegisterMouseEvents(this);
        ofRegisterKeyEvents(this);
    }

    virtual void mouseMoved(ofMouseEventArgs &a)
    { 
    	if(!active) return;

    	for( int i = 0; i < mouseListeners.size(); i++ )
    		mouseListeners[i]->mouseMoved(a.x, a.y);
    };

    virtual void mouseDragged(ofMouseEventArgs &a)
    {
    	if(!active) return;
    	
    	for( int i = 0; i < mouseListeners.size(); i++ )
    		mouseListeners[i]->mouseDragged(a.x, a.y, 0);
    }

    virtual void mousePressed(ofMouseEventArgs &a)
    {
    	if(!active) return;
    	
    	for( int i = 0; i < mouseListeners.size(); i++ )
    		mouseListeners[i]->mousePressed(a.x, a.y, 0);
    }

    virtual void mouseReleased(ofMouseEventArgs &a)
    {
    	if(!active) return;
    	
    	for( int i = 0; i < mouseListeners.size(); i++ )
    		mouseListeners[i]->mouseReleased(a.x, a.y, 0);
    }

    virtual void mouseScrolled(ofMouseEventArgs &a) 
    {
    	if(!active) return;
    	
    }

    virtual void keyPressed(ofKeyEventArgs & key)
    {
    	if(!active) return;

    	for( int i = 0; i < keyListeners.size(); i++ )
    		keyListeners[i]->ascii(key.key);
    }

	virtual void keyReleased(ofKeyEventArgs & key)
	{
		if(!active) return;
    	
	}


    std::vector<cm::MouseListener*> mouseListeners;
  	std::vector<cm::KeyboardListener*> keyListeners;
    
    bool active;
};


#endif