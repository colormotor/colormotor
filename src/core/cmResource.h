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
#include "cmUtils.h"
#include "cmRTTI.h"

namespace cm
{
	class Resource : public RttiObject
	{
		public:
			RTTIOBJECT("Resource",Resource,RttiObject)
			
			Resource() {}
			virtual ~Resource() {}
			
			bool load();
			bool save();
			
			virtual bool load( const std::string & path ) { return false; }
			virtual bool save( const std::string & path ) { return false; }
	};
}