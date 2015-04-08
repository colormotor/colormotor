/*
 *  Camera.cpp
 *
 *  Created by Daniel Berio on 7/7/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */


#include "Camera.h"


static cm::M44 infiniteFrustum(float left, float right,
								float bottom, float top,
								float zNear)
{
	cm::M44 m;
	m.identity();
	
	float e = 0.000001;
	m(0,0) = (2*zNear) / (right - left);
	m(0,2) = (right + left) / (right-left);
	
	m(1,1) = (2*zNear) / (top - bottom);
	m(1,2) = (top + bottom) / (top-bottom);
	
	m(2,2) = e-1.0f;
	m(2,3) = zNear*(e-2.0);
	
	m(3,2) = -1;
	m(3,3) = 0.0;
	
	return m;
}

static cm::M44 infinitePerspective(float fovy, float aspect, float zNear)
{
	float ymax = zNear * tan(fovy * PI / 360.0);
	float ymin = -ymax;

	float xmin = ymin * aspect;
	float xmax = ymax * aspect;
	
	
	return infiniteFrustum(xmin,xmax, ymin, ymax, zNear);
}


namespace cm
{


inline void lukat( cm::M44 & m, const cm::Vec3 & pos, const cm::Vec3 & target, const cm::Vec3 & up = cm::Vec3(0,1,0) )
{
CM_GLCONTEXT

	
	/*float vm[16];
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt(pos.x,pos.y,pos.z,target.x,target.y,target.z,up.x,up.y,up.z);
	glGetFloatv( GL_MODELVIEW_MATRIX, vm );
	*/
	
	m.identity();
	///calc axes.
	cm::Vec3 z = pos - target;
	z.normalize();
	
	cm::Vec3 x =  cross( up, z );
	x.normalize();
	
	cm::Vec3 y = ( cross( z, x ) );
	y.normalize();
	
	z.normalize();
	
	cm::Vec3 t = cm::Vec3( -pos.x, -pos.y, -pos.z );
	//cm::Vec3 t = cm::Vec3( -dot(x,pos), -dot(y,pos), -dot(z,pos) );
	
	m.m11 = x.x; m.m12 = x.y; m.m13 = x.z;
	m.m21 = y.x; m.m22 = y.y; m.m23 = y.z;
	m.m31 = z.x; m.m32 = z.y; m.m33 = z.z;
	
	//m.m41 = t.x; m.m42 = t.y; m.m43 = t.z;
	
	m.translate(t.x,t.y,t.z);
	printf("wait");
	
	/*
	m.m11 = x.x; m.m21 = x.y; m.m31 = x.z;
	m.m12 = y.x; m.m22 = y.y; m.m32 = y.z;
	m.m13 = z.x; m.m23 = z.y; m.m33 = z.z;
	m.m14 = t.x; m.m24 = t.y; m.m34 = t.z;*/
	
}

void Camera::set2DView( float w, float h, bool brotate )
{
CM_GLCONTEXT
	float halfFov, theTan, screenFov, aspect;
	screenFov 		= 60;
	
	if( brotate )
	{
		int tmp = w;
		w = h;
		h = tmp;
	}
	
	float eyeX 		= (float)w / 2.0;
	float eyeY 		= (float)h / 2.0;
	halfFov 		= PI * screenFov / 360.0;
	theTan 			= tanf(halfFov);
	float dist 		= eyeY / theTan;
	float nearDist 	= dist / 10.0;	// near / far clip plane
	float farDist 	= dist * 10.0;
	aspect 			= (float)w/(float)h;
	
	znear = nearDist;
	zfar = farDist;
	
	width = w;
	height = h;
	
	projTm.identity();
	projTm.perspective(radians(fov),width/height,znear,zfar);
	
	viewTm.identity();
	viewTm.lookAt(cm::Vec3(eyeX, eyeY, dist), cm::Vec3(eyeX, eyeY, 0.0));
	viewTm.rotateX(PID2*brotate);
	viewTm.scale(1, -1, 1);           // invert Y axis so increasing Y goes down.
	viewTm.translate(0, -h, 0);       // shift origin up to upper-left corner.
	
	worldTm = viewTm.inverted();
	applyProjection();
	applyView();
}

cm::Vec2 Camera::project( const cm::Vec3 & p, const cm::M44 & worldMat )
{
	cm::M44 pvw = projTm * viewTm * worldMat;
	Vec4 pw(p,1.0);
	return cm::project( p, pvw, width, height );
}	

void Camera::updateTransform()
{
		worldTm = nodeTm;
		if(infinite)
		{
			projTm = infinitePerspective(fov,width/height,znear);
		}
		else
		{
			projTm.perspective(radians(fov),width/height,znear,zfar);
		}
		
		viewTm = worldTm.inverted();
}

}