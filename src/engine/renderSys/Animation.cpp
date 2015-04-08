/*
 *  Animation.cpp
 *
 *  Created by Daniel Berio on 7/11/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */


#include "Animation.h"

namespace cm
{

void NodeAnimation::update()
{
	cm::Vec3 pos(0,0,0);
	cm::Vec3 scale(1,1,1);
	cm::Quat rot;
	rot.identity();
	
	// t r s
	if( posAnim )
	{
		cm::Vec3 a( posAnim->getValueA() );
		cm::Vec3 b( posAnim->getValueB() );
		pos = lerp(a,b,posAnim->getTweenValue());
	}

	if( rotAnim )
	{
		const cm::Quat &a = rotAnim->getValueA();
		const cm::Quat &b = rotAnim->getValueB();
		rot = slerp(a,b,rotAnim->getTweenValue());
	}

	if( scaleAnim )
	{
		cm::Vec3 a( scaleAnim->getValueA() );
		cm::Vec3 b( scaleAnim->getValueB() );
		scale = lerp(a,b,scaleAnim->getTweenValue());
	}
	
	cm::M44 m(rot,pos,scale);
	node->setNodeMatrix(m);
}

SceneAnimation::SceneAnimation()
:
position(0),
duration(0)
{
}

void SceneAnimation::release()
{
	DELETE_VECTOR( anims );
}

void SceneAnimation::addAnimation( Node * node, 
								AnimationChannelVec3 * pos,
								AnimationChannelQuat * rot,
								AnimationChannelVec3 * scale )
{
	if( pos )
		duration = std::max( duration, pos->getDuration() );
	if( rot )
		duration = std::max( duration, rot->getDuration() );
	if( scale )
		duration = std::max( duration, scale->getDuration() );
		
	NodeAnimation * anim = new NodeAnimation(node,pos,rot,scale);
	anims.push_back(anim);
}

void SceneAnimation::update()
{
	for( int i = 0; i < anims.size(); i++ )
	{
		anims[i]->update();
	}
}

void SceneAnimation::goTo( double time )
{
	if( time > duration )
		time = 0;
	
	position = time;
	
	for( int i = 0; i < anims.size(); i++ )
	{
		anims[i]->goTo(time);
	}
	
	update();
}


}