#ifndef MORTARANIMATION_H_
#define MORTARANIMATION_H_

#include "util/AnimationHelper.h"

using namespace anengine;
class MortarAnimationData : public AnimationHelper::AnimationData
{
    static const real StartHeight;
    public:
    MortarAnimationData(Movable *animated, real duration, VectorF4 start, 
            VectorF4 end, real height,
            AnimationHelper::AnimationCurve curve)
        : AnimationData(duration, curve), Animated(animated), myStart(start),
        myEnd(end), myHeight(height) { }
    virtual ~MortarAnimationData() { }
    Movable *Animated;
    VectorF4 myStart;
    VectorF4 myEnd;
    real myHeight;

    virtual void Update(real t);
};

#endif
