#ifndef LASER_H_
#define LASER_H_

#include "entity/FixedGeometry.h"
#include "assets/AssetManager.h"
#include "assets/Buffer.h"
#include "assets/Shader.h"
#include "entity/SceneGraph.h"
#include "util/AnimationHelper.h"

using namespace anengine;

class Laser : public FixedGeometry
{
    static StaticAsset<VertexBuffer> myVertexBuffer;

    AssetRef<Texture> myTextureRef;
    Asset<Texture> myTexture;
    protected:
    static StaticAsset<Program> LaserProgram;
    virtual void OnDraw(FrameTime elapsed);
    virtual void OnNewProgram();
    virtual void OnCreate();
    virtual void OnDestroy();
    virtual AssetRef<VertexBuffer> GetGeometry(AssetManager *manager)
    {
        return myVertexBuffer.Get(manager);
    }
    virtual void OnPropertyChanged(const PropertyInfo *info, bool implicit);

    public:
    static PropertyInfo LengthProperty;
    Property<real> Length;
    Laser(const AssetRef<Texture> &texture)
        : myTextureRef(texture), Length(&LengthProperty, this)
    {

    }
    virtual ~Laser() { }
};

class LaserAnimationData : public AnimationHelper::AnimationData
{
    public:
    LaserAnimationData(Laser *animated, real length, real speed, 
            AnimationHelper::AnimationCurve curve)
        : AnimationData(length*speed, curve), Animated(animated), Length(length) { }
    virtual ~LaserAnimationData() { }
    Laser *Animated;
    real Length;
    virtual void Update(real t);
};

#endif
