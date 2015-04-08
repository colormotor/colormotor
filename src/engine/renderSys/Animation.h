/*
 *  Animation.h
 *
 *  Created by Daniel Berio on 7/11/11.
 *  http://www.enist.org
 *  Copyright 2011. All rights reserved.
 *
 */

#pragma once

#include "Node.h"
#include "Timeline.h"

namespace cm
{
	template <class T>
	class AnimationChannel : public Base
	{
		public:
			AnimationChannel()
			{
			}
			
			~AnimationChannel()
			{
				release();
			}
			
			virtual void release()
			{
			}
			
			void addKey( const T & v, double time )
			{
				Keyframe<T> * k = new Keyframe<T>;
				k->t = time;
				k->val = v;
				timeline.addKey(k);
			}
			
			double getDuration() const { return timeline.getDuration(); }
			Timeline <T> timeline;
	};
	
	typedef AnimationChannel<cm::Vec3> AnimationChannelVec3;
	typedef AnimationChannel<double> AnimationChannelDouble;
	typedef AnimationChannel<cm::Quat> AnimationChannelQuat;
	
	template <class T>
	class Animation : public Base
	{
	public:
		Animation( AnimationChannel<T> * channel )
		:
		channel(channel)
		{
		}
		
		~Animation()
		{
			release();
		}
		
		virtual void release()
		{
			channel = 0;
		}
		
		void goTo( double time )
		{
			channel->timeline.goTo(&cursor,time);
		}
		
		const T & getValueA() const { return cursor.getValueA(); }
		const T & getValueB() const { return cursor.getValueB(); }
		double getTweenValue() const { return cursor.t; }
		double getPosition() const { return cursor.position; }
		double getDuration() const { return channel->timeline.duration; }
		
		Cursor <T> cursor;
		RefPtr < AnimationChannel<T> >  channel;
	};
	
	typedef Animation<cm::Vec3> AnimationVec3;
	typedef Animation<double> AnimationDouble;
	typedef Animation<cm::Quat> AnimationQuat;

	class NodeAnimation : public Base
	{
	public:
		NodeAnimation( Node * node, AnimationChannelVec3 * posChannel,
									AnimationChannelQuat * rotChannel,
									AnimationChannelVec3 * scaleChannel )
		:
		posChannel(posChannel),
		rotChannel(rotChannel),
		scaleChannel(scaleChannel),
		node(node),
		posAnim(0),
		rotAnim(0),
		scaleAnim(0)
		{
			if(posChannel)
				posAnim = new AnimationVec3 (posChannel);
			if(rotChannel)
				rotAnim = new AnimationQuat (rotChannel);
			if(scaleChannel)
				scaleAnim = new AnimationVec3 (scaleChannel);
		}
		
		~NodeAnimation()
		{
			release();
		}
		
		virtual void release()
		{
			node = 0;
			posChannel = 0;
			rotChannel = 0;
			scaleChannel = 0;

			SAFE_DELETE(posAnim);
			SAFE_DELETE(rotAnim);
			SAFE_DELETE(scaleAnim);
		}
		
		void goTo( double t )
		{
			if(posAnim)
				posAnim->goTo(t);
			if(rotAnim)
				rotAnim->goTo(t);
			if(scaleAnim)
				scaleAnim->goTo(t);
		}
		
		double getDuration() const
		{
			double d = 0;
			d = std::max( posChannel->getDuration(), rotChannel->getDuration() );
			d = std::max( d, scaleChannel->getDuration() );
			return d;
		}
		
		void update();
		
		RefPtr < AnimationChannelVec3 > posChannel;
		RefPtr < AnimationChannelQuat > rotChannel;
		RefPtr < AnimationChannelVec3 > scaleChannel;
		AnimationVec3 *posAnim;
		AnimationQuat *rotAnim;
		AnimationVec3 *scaleAnim;
		RefPtr <Node> node;
	};
	
	
	class SceneAnimation : public Base
	{
		public:
			SceneAnimation();
			~SceneAnimation() { release(); }
			
			virtual void release();
			
			void addAnimation( Node * node, 
								AnimationChannelVec3 * pos,
								AnimationChannelQuat * rot,
								AnimationChannelVec3 * scale );
								
			double getDuration() const { return duration; }
			double getPosition() const { return position; }
			int getNumAnimations() const { return anims.size(); }
			NodeAnimation * getAnimation( int i ) { return anims[i]; }
			
			void goTo( double time );
			
			void update();
			
		protected:
			double position;
			double duration;
		
			std::vector < NodeAnimation * > anims;
		
	};
}