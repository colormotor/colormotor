///////////////////////////////////////////////////////////////////////////                                                     
//	 _________  __   ____  ___  __  _______  __________  ___			
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \			
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/			
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.		
//																		
//  Daniel Berio
//	http://www.enist.org/												
//																	
//																		
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma once


#include "math/cmVector.h"
#include "math/cmAABox.h"
#include "math/cmPlane.h"

namespace cm
{
	//#define TESTME
	
	class Frustum  {
	public:
		enum
		{
			OUTSIDE = 0,
			INSIDE,
			INTERSECT
		};
		
		enum {
			LEFT	= 0,	
			RIGHT,		
			TOP,		
			BOTTOM,		
			FRONT,		
			BACK		
		}; 

		Frustum(){}
		virtual ~Frustum(){}

		void update(const M44 & viewProjection);

		int pointIn(const Vec3 &point) const;
		int sphereIn(const Vec3 &center,  float radius) const;
		int aaboxIn( const AABox &box ) const;
		
		/// Returns true if segment is contained in frustum, false if segment is completely out
		bool clipSegment( segment3 * res, const Vec3 & a, const Vec3 & b) const;

		Plane	frustum[6];
	private:
		 Vec3 getAABBVertexPos( const AABox & box, const Vec3 & n ) const;
		 Vec3 getAABBVertexNeg( const AABox & box, const Vec3 & n ) const;
	
	};

}