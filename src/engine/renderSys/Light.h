//////////////////////////////////////////////////////////////////////////                                                     
//	 _________  __   ____  ___  __  _______  __________  ___			
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \			
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/			
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.		
//																		
//	© Daniel Berio 2008													
//	  http://www.enist.org/												
//																	
//																		
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "CM.h"
#include "engine/renderSys/SceneNode.h"
#include "engine/renderSys/Renderable.h"

namespace CM
{

	
///////////////////////////////////////////////////////////////////////////

enum LIGHT_TYPE
{
	LIGHT_POINT = 0,
	LIGHT_SPOT,
	LIGHT_DIRECTIONAL,
	LIGHT_AREA
};

class COLORMOTOR_API	LightNode : public SceneNode
{
public:
	CM_RTTI(LightNode,SceneNode)

	LightNode()
	{
		setType(LIGHT_POINT);
	}

	/// set light type
	void		setType( LIGHT_TYPE type ) { _type = type; }
	/// get light type
	LIGHT_TYPE	getType() const { return _type; }


	
protected:
	float		_radius;
	LIGHT_TYPE	_type;
};


}