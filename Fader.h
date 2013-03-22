#ifndef FADER_H_
#define FADER_H_

#include "entity/Sprite.h"
#include "util/AnimationHelper.h"
using namespace anengine;

class Fader : public Sprite
{
    static StaticAsset<VertexBuffer> FaderVertexBuffer;
    static StaticAsset<Program> FaderProgram;
    virtual void OnDraw(FrameTime elapsed);
    virtual void OnNewProgram();
    virtual void OnCreate();
    virtual void OnPropertyChanged(const PropertyInfo *info, bool implicit);

    public:
    static PropertyInfo FadingProperty;
    Property<real> Fading;
    Fader()
        : Fading(&FadingProperty, this) { }
    virtual ~Fader() { }

    virtual AssetRef<VertexBuffer> GetGeometry(AssetManager *manager)
    {
        return FaderVertexBuffer.Get(manager);
    }
};

class FaderAnimationData : public AnimationHelper::AnimationData
{
    public:
    FaderAnimationData(Fader *animated, real duration, bool reverse,
            AnimationHelper::AnimationCurve curve)
        : AnimationData(duration, curve), Animated(animated), Reverse(reverse) { }
    virtual ~FaderAnimationData() { }
    Fader *Animated;
    bool Reverse;
    virtual void Update(real t);
};

#endif
