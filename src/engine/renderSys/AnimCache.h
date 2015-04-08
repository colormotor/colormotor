/*
 *  AnimCache.h
 *  graffDancer
 *
 *  Created by ensta on 12/14/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "engine/renderSys/BVHParser.h"
#include "engine/renderSys/SkeletonAnimSource.h"

namespace cm
{

class AnimCache
{
	public:
		AnimCache()
		{
		}
		
		~AnimCache()
		{
			release();
		}
		
		void	release()
		{
			for( int i = 0; i < _anims.size(); i++ )
				delete _anims[i];
			_anims.clear();
		}
		
		int		getNumAnimations() const { return _anims.size(); }
		SkeletonAnimSource * getAnimation( int i ) { return _anims[i]; }
		SkeletonAnimSource * getRandomAnimation() { return _anims[random(0,_anims.size())]; }
		
		bool	loadBVHFolder( const char * path, Skeleton ** skeleton = 0 )
		{
			std::vector<std::string> files;
			getFilesInFolder(files,path);
			
			if(!files.size())
			{
				debugPrint("Could not load any bvh files in folder %s \n",path);
				return false;
			}
			
			BVHParser bvh;
			
			for( int i = 0; i < files.size(); i++ )
			{
				bvh.parse(files[i].c_str());
				if(!i && skeleton!=0)
					*skeleton = bvh.createSkeleton();
				_anims.push_back(bvh.createAnimation());
			}
			
			
			
			return true;
		}
		
	protected:
		std::vector <SkeletonAnimSource*> _anims;
};

}