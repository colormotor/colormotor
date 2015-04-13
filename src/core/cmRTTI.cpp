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

#include "cmRTTI.h"

namespace cm
{

bool RttiFactory::registerClass( const char * type , CreationFuncPtr func )
{
	_creationMap[std::string(type)] = func;
	return true;
}

RttiObject * RttiFactory::createObject( const char * type )
{
	CreationMap::iterator it = _creationMap.find( type );
	if( it==_creationMap.end())
		return 0;
	CreationFuncPtr func = _creationMap[type];//it->second();
	return (*func)();
}

CM_SINGLETON_IMPL( RttiFactory )


}