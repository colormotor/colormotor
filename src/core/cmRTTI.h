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

#include "cmCoreIncludes.h"
#include "cmSingleton.h"

namespace cm
{


/// Define this in class definition for RTTI implementation
#define RTTIOBJECT(typestr,theclass,baseClass) \
	\
	static int getNumRTTIParams() { return 0; } \
	static const char * classType() { static const char *cname = typestr; return cname; } \
	virtual const char * getClassType() { return theclass::classType(); } \
	static theclass *	 instantiate() { return new theclass(); } \
	virtual bool		 isOfClassType( const char* type ) { if(strcmp(getClassType(),type)==0) return true; return baseClass::isOfClassType(type); }\
	static RttiObject *  creationFunc() { return new theclass(); } \
	static bool			 registerClass() { return RttiFactory::getInstance()->registerClass(classType(),&theclass::creationFunc); }

/// Define this in class definition for RTTI implementation in a class with pure virtual methods
#define RTTIOBJECT_INTERFACE(typestr,theclass,baseClass) \
	static int getNumRTTIParams() { return 0; } \
	static const char * classType() { static const char *cname = typestr; return cname; } \
	virtual const char * getClassType() { return theclass::classType(); } \
	static theclass *	 instantiate() { return 0; } \
	virtual bool		 isOfClassType( const char* type ) { if(strcmp(getClassType(),type)==0) return true; return baseClass::isOfClassType(type); }\
	static RttiObject *  creationFunc() { return 0; } \
	static bool			 registerClass() { return RttiFactory::getInstance()->registerClass(classType(),&theclass::creationFunc); }


/// Derive from this to add simple RTTI functionality
class  RttiObject
{
public:	
	RttiObject()
	{
	}

	virtual ~RttiObject()
	{
	}

public:
	/// Is object of class type 
	virtual bool		 isOfClassType( const char* type ) { return false; }
	/// get class type
	virtual const char*	 getClassType() { static const char * t="NONE"; return t; } 
};

typedef RttiObject * (*CreationFuncPtr)();

class  RttiFactory
{
public:
	RttiFactory () 
	{
		
	}

	~RttiFactory()
	{

	}
	
	/// Register a class, this should be done with the static ::registerClass method defined in RTTI objects
	bool		registerClass( const char * type , CreationFuncPtr func );

	/// Create a object of type described with string
	RttiObject *createObject( const char * str );

	CM_SINGLETON( RttiFactory )
	
protected:
	typedef std::map < std::string , CreationFuncPtr > CreationMap;
	CreationMap  _creationMap;
};


}


