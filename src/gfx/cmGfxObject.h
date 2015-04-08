

#pragma once 

#include "cmCore.h"

namespace cm
{

class GfxObject : public RefCounter
{
public:
	bool foo() { return false; };
	virtual ~GfxObject() {};	
};

}
