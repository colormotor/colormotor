/*
 *  Camera.h
 *
 *  Created by Daniel Berio on 7/7/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once

#include "engine/renderSys/Node.h"

namespace cm
{

class Camera : public Node
{
public:
	RTTIOBJECT( "Camera", Camera, Node )
	
	Camera()
	:
	fov(60),
	width(640.0),
	height(480.0),
	znear(0.1),
	zfar(700.0),
	infinite(false),
	active(true)
	{
	}
	
	~Camera()
	{
	}
	
	virtual void release()
	{
		Node::release();
	}
	
	virtual void setTransform( const cm::M44 & parentTm = cm::M44::identityMatrix() )
	{
		worldTm = parentTm*nodeTm;
		viewTm = worldTm.inverted();
		setModelViewMatrix(worldTm);
	}
	
	void update( float w, float h )
	{
		resize(w,h);
		updateTransform();
	}
	
	virtual void updateTransform();
	
	void apply()
	{
		applyProjection();
		applyView();
	}
	
	cm::Vec2 project( const cm::Vec3 & p, const cm::M44 & worldMat = cm::M44::identityMatrix() );
	
	void set2DView( float w, float h, bool brotate = false );
	
	// apply with specific far plane,
	// can be useful for rendering really distant objects as skydomes
	void apply( float farPlane )
	{
		cm::M44 proj;
		proj.perspective(radians(fov),width/height,znear,farPlane);
		setProjectionMatrix(proj);
		applyView();
	}
	
	void applyProjection()
	{
		setProjectionMatrix(projTm);
	}
	
	void applyView()
	{
		setModelViewMatrix(viewTm);
	}
	
	void resize( float w, float h )
	{
		width = w;
		height = h;
	}
	
	const cm::M44 & getViewMatrix() const { return viewTm; }
	const cm::M44 & getProjectionMatrix() const { return projTm; }

	bool active;
	
	float fov;
	float width;
	float height;
	float znear;
	float zfar;
	
	bool infinite;
protected:
	
	cm::M44 viewTm;
	cm::M44 projTm;
	
};


class FPSCamera : public Camera
{
public:
	RTTIOBJECT( "FPSCamera", FPSCamera, Camera )

	FPSCamera()
	:
	orientation(0,0,0),
	position(0,0,0),
	speed(0.2),
	flipY(false)
	{
		for( int i = 0; i < 4; i++ )
			keys[i] = false;
	}
	
	void move( const Vec3 & xyz )
	{
		if(!active)
			return;

		const cm::M44 & m = getNodeMatrix();
		position.x += xyz.x*speed;
		position.y += xyz.y*speed;
		position.z += xyz.z*speed;
	}

	void aim( const Vec2 & deltaXy )
	{
		if(!active)
			return;

		float fx = (float)deltaXy.x/width;
		float fy = (float)deltaXy.y/height;
		orientation += cm::Vec3( fy*180, fx*180, 0 );
	}

	void updateTransform()
	{
		if(!active)
			return;
			
		const cm::M44 & m = getNodeMatrix();
	
		identity();
		translate(position.x,position.y,position.z);
		rotate(orientation.x,orientation.y,orientation.z);
		
		if( flipY )
			worldTm.m22 = -worldTm.m22;
			
		Camera::updateTransform();
	}
	
	cm::Vec3 position;
	cm::Vec3 orientation;
	float speed;
	
	bool flipY;
	
	float mx;
	float my;
	
private:
	int keys[4];
	
};

class ArcBallCamera : public Camera
{
public:
    RTTIOBJECT( "ArcBallCamera", ArcBallCamera, Camera )
    
    ArcBallCamera()
    :
    distance(0),
    centerPoint(0,0,0)
    {
        
    }
    
    void updateTransform()
    {
        if(!active)
            return;
        identity();
        
        arcball.setup( cm::Vec2( width/2,height/2 ),width/2);
        arcball.update();
        
        M44 m;
        m.identity();
        m.translate(centerPoint);
        m *= arcball.getInertiaMatrix();
        m.translate(0,0,distance);
        
        setNodeMatrix(m);
        
        Camera::updateTransform();
    }
    
    
    void mouseMoved(int x, int y)
    {
    }
    
    void zoom( float dx, float dy )
    {
        if(!active)
            return;
        
        distance += dx*0.1;
    }
    
    void move( float x, float y )
    {
        if(!active)
            return;
        
        arcball.mouseMove(x,y);
    }
    
    void mouseDown( float x, float y )
    {
        if(!active)
            return;
        
        arcball.mouseDown(x,y);
    }
    
    void mouseUp( int x, int y )
    {
        arcball.mouseUp();
    }
    
    Vec3 centerPoint;
    float distance;
    ArcBall arcball;
};


}