///////////////////////////////////////////////////////////////////////////
//	 _________  __   ____  ___  __  _______  __________  ___
//	/ ___/ __ \/ /  / __ \/ _ \/  |/  / __ \/_  __/ __ \/ _ \
// / /__/ /_/ / /__/ /_/ / , _/ /|_/ / /_/ / / / / /_/ / , _/
// \___/\____/____/\____/_/|_/_/  /_/\____/ /_/  \____/_/|_|alpha.
//
//  Daniel Berio 2008-2014
//	http://www.enist.org/
//
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "engine/renderSys/SkeletonAnimSystem.h"
#include "engine/renderSys/BVHParser.h"
#include "engine/renderSys/SkeletonAnimSource.h"
#include "engine/renderSys/Skeleton.h"
#include "engine/renderSys/SkeletonAnimator.h"

namespace cm
{

struct Bone
{
    Bone( Joint * a, Joint * b, const M44 & transform = M44::identityMatrix() )
    :
    a(a),
    b(b),
    transform(transform)
    {
        matrix.identity();
    }
    
    Vec3 getEndPos() const;
    Vec3 getStartPos() const;
    float getLength() const;
    
    void update();
    
    M44 transform;
    
    M44 matrix;
    Joint * a;
    Joint * b;
};

class SkeletonAnimSystem
{
public:
    
    
    SkeletonAnimSystem();
    ~SkeletonAnimSystem();
    
    void addAnimSource( const std::string & name, SkeletonAnimSource * src );
    bool addBVHFile( const std::string & name, const std::string & path );
    void play( const std::string & name );
    
    void update( float msecs );
    
    /// Adds a bone to animate
    /// The bone is defined between two joints
    Bone * addBone( const std::string & joint1, const std::string & joint2, const M44 & transform = M44::identityMatrix() );
    Bone * getBone( int index ) const { return bones[index]; }
    
    std::vector<M44> getBoneMatrices() const;
    std::vector<float> getBoneLengths() const;
    int getNumBones() const { return bones.size(); }
    
    Joint * getJoint(int index);
    int getNumJoints() const;
    
    Skeleton * getSkeleton() const { return skel; }
private:
    std::vector<Bone*> bones;
    
    Skeleton * skel;
    typedef std::map<std::string,SkeletonAnimSource*> AnimMap;
    AnimMap animMap;
    SkeletonAnimLayer *skelAnim;

};

}