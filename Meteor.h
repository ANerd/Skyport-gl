#ifndef METEOR_H_
#define METEOR_H_
#include "entity/Sprite.h"
#include "util/AnimationHelper.h"

using namespace anengine;

class Meteor : public Sprite
{
    public:
    static StaticAsset<Program> MeteorProgram;
    static StaticAsset<Texture> MeteorTexture;

    virtual void OnCreate();
    virtual void OnNewProgram();

    Meteor() { }
    virtual ~Meteor() { }
};

class MeteorAnimationData : public AnimationHelper::AnimationData
{
    const real StartHeight = 5;
    const real EndHeight = -5;
    const real XSpeed = 4.1666666666666;
    public:
    MeteorAnimationData(Movable *animated, real duration,
            AnimationHelper::AnimationCurve curve)
        : AnimationData(duration, curve), Animated(animated)
    {
        Repeating = true;
    }
    virtual ~MeteorAnimationData() { }
    Movable *Animated;
    real StartX;
    virtual void Update(real t);
    virtual void Finalize();
};

#endif
