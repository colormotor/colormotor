#pragma once
#include "colormotor.h"
using namespace cm;

float radius=300.0;
float dist=2;
class App : public AppModule
{
public:
	// Used to access the module externally (MainModule::instance)
	static App * instance;
    ParamList params;
    ArcBall ball;
    
	App()
	:
	AppModule("Main Module")
	{
		instance = this;
        
        params.addFloat("Radius",&radius, 10.0, 600.0);
        params.addFloat("distance",&dist,0,10);

        params.addFloat("foo", 0.0, 0.0, 1.0); // A parameter not connected to a variable
        											 // can access it with params.getFloat("foo")
        params.addBool("foo b", true); // Bool case, use params.getBool("foo b")
        
	}

	bool init()
	{
        return true;
	}

	void exit()
	{
	}

	bool gui()
	{
        imgui(params); // Creates a UI for the parameters
        if( ImGui::Button("Hello world") )
        {
            printf("Hello world\n");
        }

		return false; 
	}
    
	void setTransform()
	{
        float w = appWidth();
        float h = appHeight();
        
		gfx::setPerspectiveProjection(60.0,w/h, 0.01, 1000.0);
		gfx::identity();
        V3 p1, p2;
        if( Mouse::down() )
        {
            ball.update( Mouse::pos(), Mouse::delta(), appCenter(), radius );
        }
        
        M44 m = trans3d(V3(0.0f, 0.0f, -dist)) * ball.mat;
        gfx::setModelViewMatrix(m);
        
        gfx::color(1,0,0);
        gfx::drawSphere(p1, 0.05);
        gfx::color(0,0,1);
        gfx::drawSphere(p2, 0.05);
	}
	
	void update()
	{
		// Gets called every frame before render
	}
	
	void render()
	{
        float w = appWidth();
        float h = appHeight();
        
        gfx::clear(1,1,1,1);
        
		gfx::setBlendMode(gfx::BLENDMODE_ALPHA);
		gfx::enableDepthBuffer(false);
		setTransform();
        
		gfx::color(0.8);
		gfx::drawWireCube(V3(0,0,0),V3(1,1,1));//{1.0f,1.0f,1.0f});
		arma::mat ey = arma::eye(4,4);
		gfx::drawAxis(ey, 0.1f);
        
        gfx::setOrtho(w, h);
        gfx::color(1,0,0);
        gfx::drawCircle(appCenter(), radius);
        
	}
};

// allows us to access this instance externally if necessary.
#ifndef MAIN_MODULE_SINGLETON
#define MAIN_MODULE_SINGLETON
App * App::instance = 0;
#endif


	
